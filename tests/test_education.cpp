#include <gtest/gtest.h>

#include "EducationPolicies.h"
#include "EducationSeed.h"
#include "Participant.h"
#include "ParticipantRegistry.h"

using education::EducationPolicies;
using education::EducationRoles;
using education::EducationSeed;

// Mark validation

TEST(EducationPoliciesTest, AcceptsMarksFromZeroToHundred)
{
    EXPECT_TRUE(EducationPolicies::isValidMark(0));
    EXPECT_TRUE(EducationPolicies::isValidMark(55));
    EXPECT_TRUE(EducationPolicies::isValidMark(100));
}

TEST(EducationPoliciesTest, RefusesMarksOutsideZeroToHundred)
{
    EXPECT_FALSE(EducationPolicies::isValidMark(-1));
    EXPECT_FALSE(EducationPolicies::isValidMark(101));
    EXPECT_FALSE(EducationPolicies::isValidMark(-300));
}

// Approval bands (boundaries)

TEST(EducationPoliciesTest, MarkBelowFortyIsNotAutoApprovable)
{
    EXPECT_FALSE(EducationPolicies::isAutoApprovable(0));
    EXPECT_FALSE(EducationPolicies::isAutoApprovable(39));
    EXPECT_EQ("", EducationPolicies::approverRoleFor(39));
}

TEST(EducationPoliciesTest, FortyIsTheLowestApprovableMark)
{
    EXPECT_TRUE(EducationPolicies::isAutoApprovable(40));
    EXPECT_EQ(EducationRoles::moderator(), EducationPolicies::approverRoleFor(40));
}

TEST(EducationPoliciesTest, FortyNineBelongsToModerator)
{
    EXPECT_EQ(EducationRoles::moderator(), EducationPolicies::approverRoleFor(49));
}

TEST(EducationPoliciesTest, FiftyToSeventyBelongsToTA)
{
    EXPECT_EQ(EducationRoles::ta(), EducationPolicies::approverRoleFor(50));
    EXPECT_EQ(EducationRoles::ta(), EducationPolicies::approverRoleFor(60));
    EXPECT_EQ(EducationRoles::ta(), EducationPolicies::approverRoleFor(70));
}

TEST(EducationPoliciesTest, SeventyOneAndAboveBelongsToLecturer)
{
    EXPECT_EQ(EducationRoles::lecturer(), EducationPolicies::approverRoleFor(71));
    EXPECT_EQ(EducationRoles::lecturer(), EducationPolicies::approverRoleFor(100));
}

TEST(EducationPoliciesTest, InvalidMarkHasNoApprover)
{
    EXPECT_EQ("", EducationPolicies::approverRoleFor(-1));
    EXPECT_EQ("", EducationPolicies::approverRoleFor(101));
    EXPECT_FALSE(EducationPolicies::isAutoApprovable(101));
}

TEST(EducationPoliciesTest, EveryValidMarkMapsToExactlyOneOutcome)
{
    for (int mark = 0; mark <= 100; ++mark)
    {
        const std::string role = EducationPolicies::approverRoleFor(mark);
        if (mark < 40)
        {
            EXPECT_TRUE(role.empty()) << "mark " << mark;
        }
        else
        {
            EXPECT_FALSE(role.empty()) << "mark " << mark;
        }
    }
}

// Approver selection

class EducationApproverTest : public ::testing::Test
{
    protected:
        ParticipantRegistry registry;

        void SetUp()
        {
            EducationSeed::seedParticipants(registry);
        }
};

TEST_F(EducationApproverTest, SeedCreatesTheStandardCast)
{
    EXPECT_EQ(3u, registry.getByRole(EducationRoles::ta()).size());
    EXPECT_EQ(1u, registry.getByRole(EducationRoles::lecturer()).size());
    EXPECT_EQ(1u, registry.getByRole(EducationRoles::moderator()).size());
    EXPECT_EQ(5u, registry.size());
}

TEST_F(EducationApproverTest, TABandApproverIsADifferentTA)
{
    Participant* assignee = registry.getByRole(EducationRoles::ta()).front();

    Participant* approver = EducationPolicies::selectApprover(60, assignee, registry);

    ASSERT_NE(nullptr, approver);
    EXPECT_EQ(EducationRoles::ta(), approver->getRole());
    EXPECT_NE(assignee->getId(), approver->getId());
}

TEST_F(EducationApproverTest, LecturerBandReturnsTheLecturer)
{
    Participant* assignee = registry.getByRole(EducationRoles::ta()).front();

    Participant* approver = EducationPolicies::selectApprover(85, assignee, registry);

    ASSERT_NE(nullptr, approver);
    EXPECT_EQ(EducationRoles::lecturer(), approver->getRole());
}

TEST_F(EducationApproverTest, ModeratorBandReturnsTheModerator)
{
    Participant* assignee = registry.getByRole(EducationRoles::ta()).front();

    Participant* approver = EducationPolicies::selectApprover(45, assignee, registry);

    ASSERT_NE(nullptr, approver);
    EXPECT_EQ(EducationRoles::moderator(), approver->getRole());
}

TEST_F(EducationApproverTest, MarkBelowFortyHasNoApprover)
{
    Participant* assignee = registry.getByRole(EducationRoles::ta()).front();

    EXPECT_EQ(nullptr, EducationPolicies::selectApprover(39, assignee, registry));
}

TEST_F(EducationApproverTest, PrefersTheTAWithFewestPendingTasks)
{
    std::vector<Participant*> tas = registry.getByRole(EducationRoles::ta());
    ASSERT_EQ(3u, tas.size());

    Participant* assignee = tas[0];
    tas[1]->incrementPendingTasks();
    tas[1]->incrementPendingTasks();
    // tas[2] has none, so it should be chosen over tas[1]

    Participant* approver = EducationPolicies::selectApprover(55, assignee, registry);

    ASSERT_NE(nullptr, approver);
    EXPECT_EQ(tas[2]->getId(), approver->getId());
}

TEST(EducationApproverIsolationTest, NoOtherTAMeansNoApprover)
{
    ParticipantRegistry registry;
    Participant* onlyTA = registry.add("Solo", EducationRoles::ta());

    EXPECT_EQ(nullptr, EducationPolicies::selectApprover(60, onlyTA, registry));
}

TEST(EducationApproverIsolationTest, NullAssigneeStillFindsAnApprover)
{
    ParticipantRegistry registry;
    EducationSeed::seedParticipants(registry);

    EXPECT_NE(nullptr, EducationPolicies::selectApprover(60, nullptr, registry));
}

// Demo data

TEST(EducationSeedTest, DemoSubmissionsCoverEveryBand)
{
    std::vector<education::DemoSubmission> subs = EducationSeed::demoSubmissions();
    ASSERT_EQ(8u, subs.size());

    bool rejected = false, moderator = false, ta = false, lecturer = false;
    for (size_t i = 0; i < subs.size(); ++i)
    {
        const std::string role = EducationPolicies::approverRoleFor(subs[i].mark);
        if (role.empty()) rejected = true;
        if (role == EducationRoles::moderator()) moderator = true;
        if (role == EducationRoles::ta()) ta = true;
        if (role == EducationRoles::lecturer()) lecturer = true;
        EXPECT_TRUE(EducationPolicies::isValidMark(subs[i].mark));
    }
    EXPECT_TRUE(rejected);
    EXPECT_TRUE(moderator);
    EXPECT_TRUE(ta);
    EXPECT_TRUE(lecturer);
}
