// Tests for the approval chain and the coordinator's event handling.
// Items here are detached (no container), so the phase checks in Item do not
// apply; the full phase flow is covered in test_education_flow.cpp.

#include <gtest/gtest.h>

#include "AssignmentMarkingCoordinator.h"
#include "EducationApprovalChain.h"
#include "EducationPolicies.h"
#include "EducationSeed.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Submission.h"

#include <algorithm>

using education::AssignmentMarkingCoordinator;
using education::EducationApprovalChain;
using education::EducationRoles;
using education::EducationSeed;
using education::Submission;

namespace
{
    // Walks a detached submission through the item lifecycle to Completed.
    void completeWithMark(Submission& item, Participant* assignee, int mark)
    {
        item.assign(assignee);
        item.start();
        item.complete(mark);
    }
}

class EducationChainTest : public ::testing::Test
{
    protected:
        ParticipantRegistry registry;
        Participant* assignee;

        void SetUp()
        {
            EducationSeed::seedParticipants(registry);
            assignee = registry.getByRole(EducationRoles::ta()).front();
        }
};

TEST_F(EducationChainTest, TAMarkIsApprovedByADifferentTA)
{
    EducationApprovalChain chain(registry);
    Submission item("SUB-B", "Student B");
    completeWithMark(item, assignee, 60);

    EXPECT_TRUE(chain.approve(item, 60));

    ASSERT_NE(nullptr, item.getReviewer());
    EXPECT_EQ(EducationRoles::ta(), item.getReviewer()->getRole());
    EXPECT_NE(assignee->getId(), item.getReviewer()->getId());
    EXPECT_EQ("Batch", item.getReviewMethod());
    EXPECT_TRUE(item.isReviewed());
}

TEST_F(EducationChainTest, HighMarkIsApprovedByTheLecturer)
{
    EducationApprovalChain chain(registry);
    Submission item("SUB-A", "Student A");
    completeWithMark(item, assignee, 78);

    EXPECT_TRUE(chain.approve(item, 78));

    ASSERT_NE(nullptr, item.getReviewer());
    EXPECT_EQ(EducationRoles::lecturer(), item.getReviewer()->getRole());
}

TEST_F(EducationChainTest, LowPassMarkIsApprovedByTheModerator)
{
    EducationApprovalChain chain(registry);
    Submission item("SUB-C", "Student C");
    completeWithMark(item, assignee, 45);

    EXPECT_TRUE(chain.approve(item, 45));

    ASSERT_NE(nullptr, item.getReviewer());
    EXPECT_EQ(EducationRoles::moderator(), item.getReviewer()->getRole());
}

TEST_F(EducationChainTest, MarkBelowFortyFallsOffTheChain)
{
    EducationApprovalChain chain(registry);
    Submission item("SUB-G", "Student G");
    completeWithMark(item, assignee, 39);

    EXPECT_FALSE(chain.approve(item, 39));
    EXPECT_EQ(nullptr, item.getReviewer());
}

TEST_F(EducationChainTest, EveryBoundaryMarkGetsTheExpectedRole)
{
    EducationApprovalChain chain(registry);
    const int marks[] = { 40, 49, 50, 70, 71, 100 };
    const std::string roles[] = {
        EducationRoles::moderator(), EducationRoles::moderator(),
        EducationRoles::ta(), EducationRoles::ta(),
        EducationRoles::lecturer(), EducationRoles::lecturer()
    };

    for (unsigned i = 0; i < sizeof(marks) / sizeof(marks[0]); ++i)
    {
        Submission item("SUB", "Student");
        completeWithMark(item, assignee, marks[i]);
        ASSERT_TRUE(chain.approve(item, marks[i])) << "mark " << marks[i];
        EXPECT_EQ(roles[i], item.getReviewer()->getRole()) << "mark " << marks[i];
    }
}

TEST(EducationChainIsolationTest, OnlyOneTAMeansTAMarkIsNotApproved)
{
    ParticipantRegistry registry;
    Participant* solo = registry.add("Solo", EducationRoles::ta());
    registry.add("Dr Dlamini", EducationRoles::lecturer());
    EducationApprovalChain chain(registry);

    Submission item("SUB-B", "Student B");
    completeWithMark(item, solo, 60);

    EXPECT_FALSE(chain.approve(item, 60));
    EXPECT_EQ(nullptr, item.getReviewer());
}

// ---- coordinator: events -----------------------------------------------

class AssignmentMarkingCoordinatorTest : public ::testing::Test
{
    protected:
        ParticipantRegistry registry;
        AssignmentMarkingCoordinator* coordinator;

        void SetUp()
        {
            EducationSeed::seedParticipants(registry);
            coordinator = new AssignmentMarkingCoordinator(registry);
        }
        void TearDown()
        {
            delete coordinator;
        }

        bool historyContains(const std::string& text) const
        {
            const std::vector<std::string> history = coordinator->getHistoryAsStrings();
            for (unsigned i = 0; i < history.size(); ++i)
            {
                if (history[i].find(text) != std::string::npos)
                {
                    return true;
                }
            }
            return false;
        }
};

TEST_F(AssignmentMarkingCoordinatorTest, ItemCompletedRecordsTheMark)
{
    Submission item("SUB-A", "Student A");
    EXPECT_FALSE(item.hasMark());

    coordinator->onItemCompleted(&item, 64);

    EXPECT_TRUE(item.hasMark());
    EXPECT_EQ(64, item.getMark());
    EXPECT_TRUE(historyContains("SUB-A marked 64"));
}

TEST_F(AssignmentMarkingCoordinatorTest, OverrideReplacesTheMark)
{
    Submission item("SUB-A", "Student A");
    coordinator->onItemCompleted(&item, 35);

    Participant* lecturer = registry.getByRole(EducationRoles::lecturer()).front();
    coordinator->onItemOverridden(&item, lecturer, 52, "Re-marked");

    EXPECT_EQ(52, item.getMark());
    EXPECT_TRUE(historyContains("settled by"));
}

TEST_F(AssignmentMarkingCoordinatorTest, InvalidMarkIsFlaggedInTheHistory)
{
    Submission item("SUB-A", "Student A");

    coordinator->onItemCompleted(&item, 150);

    EXPECT_TRUE(historyContains("invalid mark"));
}

TEST_F(AssignmentMarkingCoordinatorTest, EventsWithoutASubmissionAreHarmless)
{
    coordinator->onItemCompleted(nullptr, 50);
    coordinator->onPhaseAdvanced(nullptr);
    coordinator->onContainerApprovalRequested(nullptr, nullptr);
    coordinator->onItemRejected(nullptr, "reason");

    EXPECT_TRUE(coordinator->getHistoryAsStrings().size() >= 1u);
}

TEST(SubmissionTest, StartsWithoutAMark)
{
    Submission item("SUB-A", "Student A");

    EXPECT_FALSE(item.hasMark());
    EXPECT_EQ(-1, item.getMark());
    EXPECT_EQ("Student A", item.getStudentName());
    EXPECT_EQ("submission", item.getType());
}
