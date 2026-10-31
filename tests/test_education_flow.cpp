// Builder, director and the full marking flow for the Education scenario,
// using the real phases (Intake -> Active -> Review -> Publish) and the
// real item states.
//
// The coordinator's on...() methods are called by hand after each item
// change, the way Item does through its mediator. Markers are assigned by the
// coordinator (openAssignment) through the assignment engine.

#include <gtest/gtest.h>

#include "AssignmentMarkingBuilder.h"
#include "AssignmentMarkingCoordinator.h"
#include "AssignmentMarkingDirector.h"
#include "Container.h"
#include "EducationPolicies.h"
#include "EducationSeed.h"
#include "InvalidTransition.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Root.h"
#include "Submission.h"
#include "WorkRegistry.h"
#include "WorkflowBuilder.h"
#include "WorkflowDirector.h"

#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using education::AssignmentMarkingBuilder;
using education::AssignmentMarkingCoordinator;
using education::AssignmentMarkingDirector;
using education::EducationRoles;
using education::EducationSeed;
using education::Submission;

// ---- builder and director ---------------------------------------------------

TEST(AssignmentMarkingBuilderTest, BuildsModuleAssignmentAndSubmissions)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");
    builder.buildItem("SUB-A", "Student A");
    builder.buildItem("SUB-B", "Student B");

    Root* root = builder.getRoot();
    ASSERT_NE(nullptr, root);
    EXPECT_EQ("COS 214", root->getName());
    ASSERT_EQ(1u, root->getChildren().size());

    Component* assignment = root->getChildren().front();
    EXPECT_EQ("A1", assignment->getId());
    EXPECT_EQ("assignment", assignment->getType());
    ASSERT_EQ(2u, assignment->getChildren().size());
    EXPECT_EQ("SUB-A", assignment->getChildren()[0]->getId());
    EXPECT_EQ(assignment, assignment->getChildren()[0]->getParent());
    EXPECT_NE(nullptr, dynamic_cast<Submission*>(assignment->getChildren()[0]));

    EXPECT_EQ(4u, registry.size()); // root + assignment + 2 submissions
}

TEST(AssignmentMarkingBuilderTest, DefaultConstructedBuilderNeedsARegistry)
{
    AssignmentMarkingBuilder builder; // what a factory creates with no arguments
    EXPECT_THROW(builder.buildRoot("COS 214"), std::logic_error);

    WorkRegistry registry;
    builder.setRegistry(registry);
    builder.buildRoot("COS 214");

    EXPECT_NE(nullptr, builder.getRoot());
    EXPECT_EQ(1u, registry.size());
}

TEST(AssignmentMarkingBuilderTest, ContainerBeforeRootThrows)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    EXPECT_THROW(builder.buildContainer("A1", "Assignment 1"), std::logic_error);
}

TEST(AssignmentMarkingBuilderTest, ItemBeforeContainerThrows)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    EXPECT_THROW(builder.buildItem("SUB-A", "Student A"), std::logic_error);
}

TEST(AssignmentMarkingBuilderTest, DuplicateItemIdIsRefusedAndLeavesNoOrphan)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");
    builder.buildItem("SUB-A", "Student A");
    const size_t before = registry.size();

    EXPECT_THROW(builder.buildItem("SUB-A", "Student A again"), std::invalid_argument);

    EXPECT_EQ(before, registry.size());
    EXPECT_EQ(1u, builder.getCurrentContainer()->getChildren().size());
}


// ---- adding to a named assignment (parent id) ---------------------------------

namespace
{
    // A builder that is not ours: it only records what it was asked to build.
    class RecordingBuilder : public WorkflowBuilder
    {
        public:
            std::vector<std::string> calls;

            virtual void buildRoot(const std::string& rootName) { calls.push_back("root:" + rootName); }
            virtual void buildContainer(const std::string& containerId, const std::string&)
            {
                calls.push_back("container:" + containerId);
            }
            virtual void buildItem(const std::string& itemId, const std::string&)
            {
                calls.push_back("item:" + itemId);
            }
            virtual Root* getRoot() const { return nullptr; }
    };

    std::vector<std::string> childIds(Component* parent)
    {
        std::vector<std::string> ids;
        const std::vector<Component*> children = parent->getChildren();
        for (unsigned i = 0; i < children.size(); ++i)
        {
            ids.push_back(children[i]->getId());
        }
        return ids;
    }
}

TEST(AssignmentMarkingBuilderParentTest, SubmissionGoesIntoTheNamedAssignmentNotTheCurrentOne)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");
    builder.buildContainer("A2", "Assignment 2"); // A2 is now current

    builder.buildSubmission("A1", "SUB-A", "Student A", "u1001");

    EXPECT_EQ(1u, registry.get("A1")->getChildren().size());
    EXPECT_TRUE(registry.get("A2")->getChildren().empty());
    EXPECT_EQ(registry.get("A1"), registry.get("SUB-A")->getParent());
    EXPECT_EQ("u1001", dynamic_cast<Submission*>(registry.get("SUB-A"))->getStudentId());
}

TEST(AssignmentMarkingBuilderParentTest, NamedAssignmentBecomesTheCurrentOne)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");
    builder.buildContainer("A2", "Assignment 2");

    builder.buildSubmission("A1", "SUB-A", "Student A", "u1001");
    builder.buildItem("SUB-B", "Student B"); // continues in A1

    EXPECT_EQ(2u, registry.get("A1")->getChildren().size());
    EXPECT_EQ(builder.getCurrentContainer(), registry.get("A1"));
}

TEST(AssignmentMarkingBuilderParentTest, UnknownParentIsRefusedAndNothingIsRegistered)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");
    const size_t before = registry.size();

    EXPECT_THROW(builder.buildSubmission("NOPE", "SUB-A", "Student A", "u1001"),
                 std::invalid_argument);

    EXPECT_EQ(before, registry.size());
    EXPECT_EQ(builder.getCurrentContainer(), registry.get("A1"));
}

TEST(AssignmentMarkingBuilderParentTest, ParentThatIsNotAnAssignmentIsRefused)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");
    builder.buildItem("SUB-A", "Student A");

    EXPECT_THROW(builder.buildSubmission("SUB-A", "SUB-B", "Student B", "u1002"),
                 std::invalid_argument); // an item
    EXPECT_THROW(builder.buildSubmission("COS 214", "SUB-B", "Student B", "u1002"),
                 std::invalid_argument); // the module
}

TEST(AssignmentMarkingBuilderParentTest, ParentBeforeRootThrows)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);

    EXPECT_THROW(builder.buildSubmission("A1", "SUB-A", "Student A", "u1001"), std::logic_error);
    EXPECT_THROW(builder.selectContainer("A1"), std::logic_error);
}

TEST(AssignmentMarkingBuilderParentTest, SelectContainerSwitchesWhereBuildItemAdds)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");
    builder.buildContainer("A2", "Assignment 2");

    builder.selectContainer("A1");
    builder.buildItem("SUB-A", "Student A");

    EXPECT_EQ(1u, registry.get("A1")->getChildren().size());
    EXPECT_TRUE(registry.get("A2")->getChildren().empty());
    EXPECT_THROW(builder.selectContainer("NOPE"), std::invalid_argument);
}

TEST(AssignmentMarkingBuilderParentTest, DuplicateSubmissionKeepsTheCurrentAssignment)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");
    builder.buildContainer("A2", "Assignment 2");
    builder.buildSubmission("A1", "SUB-A", "Student A", "u1001");
    builder.selectContainer("A2");
    const size_t before = registry.size();

    EXPECT_THROW(builder.buildSubmission("A1", "SUB-A", "Student A", "u1001"),
                 std::invalid_argument);

    EXPECT_EQ(before, registry.size());
    EXPECT_EQ(builder.getCurrentContainer(), registry.get("A2")); // not switched
}

TEST(AssignmentMarkingBuilderParentTest, OpenedAssignmentRefusesNewSubmissionsAndLeavesNoOrphan)
{
    WorkRegistry registry;
    ParticipantRegistry participants;
    EducationSeed::seedParticipants(participants);
    AssignmentMarkingCoordinator coordinator(participants);

    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");
    builder.buildItem("SUB-A", "Student A");
    coordinator.openAssignment(*builder.getCurrentContainer()); // Intake -> Active
    const size_t before = registry.size();

    EXPECT_THROW(builder.buildSubmission("A1", "SUB-B", "Student B", "u1002"),
                 InvalidTransition);

    EXPECT_EQ(before, registry.size());
    EXPECT_EQ(1u, registry.get("A1")->getChildren().size());
}

// ---- director extends WorkflowDirector ----------------------------------------

TEST(AssignmentMarkingDirectorTest, IsAWorkflowDirector)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    AssignmentMarkingDirector director(builder);

    WorkflowDirector* generic = &director;

    EXPECT_NE(nullptr, generic);
}

TEST(AssignmentMarkingDirectorTest, BuildDemoTreeOnOurBuilderKeepsStudentNumbers)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    AssignmentMarkingDirector director(builder);
    WorkflowDirector& generic = director;

    Root* root = generic.buildDemoTree(builder, "COS 214");

    ASSERT_NE(nullptr, root);
    EXPECT_EQ(builder.getRoot(), root);
    const std::vector<education::DemoSubmission> rows = EducationSeed::demoSubmissions();
    ASSERT_EQ(1u, root->getChildren().size());
    EXPECT_EQ(rows.size(), root->getChildren().front()->getChildren().size());
    Submission* first = dynamic_cast<Submission*>(registry.get(rows.front().itemId));
    ASSERT_NE(nullptr, first);
    EXPECT_EQ(rows.front().studentId, first->getStudentId());
}

TEST(AssignmentMarkingDirectorTest, BuildDemoTreeOnAnotherBuilderUsesBuildItem)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder ours(registry);
    AssignmentMarkingDirector director(ours);
    RecordingBuilder other;

    director.buildDemoTree(other, "Some module");

    ASSERT_GE(other.calls.size(), 3u);
    EXPECT_EQ("root:Some module", other.calls[0]);
    EXPECT_EQ("container:A1", other.calls[1]);
    EXPECT_EQ("item:" + EducationSeed::demoSubmissions().front().itemId, other.calls[2]);
    EXPECT_EQ(2u + EducationSeed::demoSubmissions().size(), other.calls.size());
    EXPECT_EQ(0u, registry.size()); // our own builder was not used
}

TEST(AssignmentMarkingDirectorTest, BuildTreePutsEachItemUnderTheContainerItNames)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    AssignmentMarkingDirector director(builder);

    std::vector<std::pair<std::string, std::string>> containers;
    containers.push_back(std::make_pair("A1", "Assignment 1"));
    containers.push_back(std::make_pair("A2", "Assignment 2"));
    std::vector<std::tuple<std::string, std::string, std::string>> items;
    items.push_back(std::make_tuple("A2", "SUB-C", "Student C")); // out of order on purpose
    items.push_back(std::make_tuple("A1", "SUB-A", "Student A"));
    items.push_back(std::make_tuple("A2", "SUB-D", "Student D"));
    items.push_back(std::make_tuple("A1", "SUB-B", "Student B"));

    Root* root = director.buildTree(builder, "COS 214", containers, items);

    ASSERT_NE(nullptr, root);
    EXPECT_EQ(2u, root->getChildren().size());
    const std::vector<std::string> a1 = childIds(registry.get("A1"));
    const std::vector<std::string> a2 = childIds(registry.get("A2"));
    ASSERT_EQ(2u, a1.size());
    ASSERT_EQ(2u, a2.size());
    EXPECT_EQ("SUB-A", a1[0]);
    EXPECT_EQ("SUB-B", a1[1]);
    EXPECT_EQ("SUB-C", a2[0]);
    EXPECT_EQ("SUB-D", a2[1]);
}

TEST(AssignmentMarkingDirectorTest, BuildTreeWithAnUnknownContainerBuildsNothing)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    AssignmentMarkingDirector director(builder);

    std::vector<std::pair<std::string, std::string>> containers;
    containers.push_back(std::make_pair("A1", "Assignment 1"));
    std::vector<std::tuple<std::string, std::string, std::string>> items;
    items.push_back(std::make_tuple("A1", "SUB-A", "Student A"));
    items.push_back(std::make_tuple("A9", "SUB-B", "Student B")); // no such container

    EXPECT_THROW(director.buildTree(builder, "COS 214", containers, items),
                 std::invalid_argument);

    EXPECT_EQ(0u, registry.size());
}

TEST(AssignmentMarkingDirectorTest, BuildTreeWithNoItemsBuildsEmptyAssignments)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    AssignmentMarkingDirector director(builder);

    std::vector<std::pair<std::string, std::string>> containers;
    containers.push_back(std::make_pair("A1", "Assignment 1"));
    std::vector<std::tuple<std::string, std::string, std::string>> items;

    Root* root = director.buildTree(builder, "COS 214", containers, items);

    ASSERT_NE(nullptr, root);
    EXPECT_TRUE(registry.get("A1")->getChildren().empty());
}

TEST(AssignmentMarkingDirectorTest, EmptyWorkflowHasAnAssignmentAndNoSubmissions)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    AssignmentMarkingDirector director(builder);

    director.buildEmptyWorkflow();

    ASSERT_NE(nullptr, builder.getCurrentContainer());
    EXPECT_TRUE(builder.getCurrentContainer()->getChildren().empty());
    EXPECT_EQ(2u, registry.size());
}

TEST(AssignmentMarkingBuilderTest, BuildSubmissionStoresTheStudentNumber)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");

    builder.buildSubmission("SUB-A", "Student A", "u1001");
    builder.buildItem("SUB-B", "Student B"); // the generic call has no student number

    Submission* a = dynamic_cast<Submission*>(registry.get("SUB-A"));
    Submission* b = dynamic_cast<Submission*>(registry.get("SUB-B"));
    ASSERT_NE(nullptr, a);
    ASSERT_NE(nullptr, b);
    EXPECT_EQ("u1001", a->getStudentId());
    EXPECT_EQ("SUB-B", b->getStudentId()); // falls back to the item id
}

TEST(AssignmentMarkingDirectorTest, DemoWorkflowHasOneSubmissionPerDemoRow)
{
    WorkRegistry registry;
    AssignmentMarkingBuilder builder(registry);
    AssignmentMarkingDirector director(builder);

    director.buildDemoWorkflow();

    const std::vector<education::DemoSubmission> rows = EducationSeed::demoSubmissions();
    EXPECT_EQ(rows.size(), builder.getCurrentContainer()->getChildren().size());
    Submission* first = dynamic_cast<Submission*>(registry.get(rows.front().itemId));
    ASSERT_NE(nullptr, first);
    EXPECT_EQ(rows.front().studentId, first->getStudentId());
}

// ---- full flow ----------------------------------------------------------------

class AssignmentFlowTest : public ::testing::Test
{
    protected:
        // Declared first so they are destroyed last: items release their
        // participants when the registry frees them.
        ParticipantRegistry participants;
        WorkRegistry registry;
        AssignmentMarkingCoordinator* coordinator;
        Container* assignment;
        Submission* subA;
        Submission* subB;
        Submission* subG;
        Participant* lecturer;
        std::vector<Participant*> tas;

        AssignmentFlowTest()
            : coordinator(nullptr), assignment(nullptr),
              subA(nullptr), subB(nullptr), subG(nullptr), lecturer(nullptr)
        {
        }

        void SetUp()
        {
            EducationSeed::seedParticipants(participants);
            coordinator = new AssignmentMarkingCoordinator(participants);
            lecturer = participants.getByRole(EducationRoles::lecturer()).front();
            tas = participants.getByRole(EducationRoles::ta());

            AssignmentMarkingBuilder builder(registry);
            builder.buildRoot("COS 214");
            builder.buildContainer("A1", "Assignment 1");
            builder.buildItem("SUB-A", "Student A");
            builder.buildItem("SUB-B", "Student B");
            builder.buildItem("SUB-G", "Student G");
            assignment = builder.getCurrentContainer();

            subA = dynamic_cast<Submission*>(registry.get("SUB-A"));
            subB = dynamic_cast<Submission*>(registry.get("SUB-B"));
            subG = dynamic_cast<Submission*>(registry.get("SUB-G"));

            coordinator->openAssignment(*assignment); // Intake -> Active, TAs assigned
        }

        void TearDown()
        {
            delete coordinator;
        }

        void markItem(Submission* item, int mark)
        {
            item->start();
            item->complete(mark);
            coordinator->onItemCompleted(item, mark);
        }

        void markAll()
        {
            markItem(subA, 78);
            markItem(subB, 65);
            markItem(subG, 35);
        }

        void approveBatch()
        {
            coordinator->onContainerApprovalRequested(assignment, lecturer);
        }
};

TEST_F(AssignmentFlowTest, OpeningTheAssignmentGivesEverySubmissionADifferentTA)
{
    EXPECT_EQ("Active", assignment->getPhaseName());

    Submission* all[] = { subA, subB, subG };
    for (unsigned i = 0; i < 3; ++i)
    {
        EXPECT_EQ("Assigned", all[i]->getStateName());
        ASSERT_NE(nullptr, all[i]->getAssignee());
        EXPECT_EQ(EducationRoles::ta(), all[i]->getAssignee()->getRole());
    }
    EXPECT_NE(subA->getAssignee(), subB->getAssignee());
    EXPECT_NE(subB->getAssignee(), subG->getAssignee());
}

TEST_F(AssignmentFlowTest, OpeningAnAssignmentTwiceThrows)
{
    EXPECT_THROW(coordinator->openAssignment(*assignment), InvalidTransition);
}

TEST_F(AssignmentFlowTest, AssignmentWhereEverythingIsApprovedPublishesAutomatically)
{
    markItem(subA, 78);
    markItem(subB, 65);
    markItem(subG, 45); // moderator band, auto-approvable

    approveBatch();

    EXPECT_EQ(0u, coordinator->getLastBatch().rejectedIds.size());
    EXPECT_EQ("Publish", assignment->getPhaseName());
}

TEST_F(AssignmentFlowTest, AssignmentStaysActiveUntilEverySubmissionIsMarked)
{
    markItem(subA, 78);
    markItem(subB, 65);

    EXPECT_EQ("Active", assignment->getPhaseName());
}

TEST_F(AssignmentFlowTest, LastMarkMovesTheAssignmentToReview)
{
    markAll();

    EXPECT_EQ("Review", assignment->getPhaseName());
    EXPECT_EQ(78, subA->getMark());
    EXPECT_EQ(35, subG->getMark());
}

TEST_F(AssignmentFlowTest, BatchApprovesInRangeMarksAndRejectsLowOnes)
{
    markAll();

    approveBatch();

    const AssignmentMarkingCoordinator::BatchResult& result = coordinator->getLastBatch();
    EXPECT_EQ(2u, result.approvedIds.size());
    ASSERT_EQ(1u, result.rejectedIds.size());
    EXPECT_EQ("SUB-G", result.rejectedIds.front());
    EXPECT_EQ("Rejected", subG->getStateName());
    EXPECT_EQ(nullptr, subG->getReviewer());
    EXPECT_NE(nullptr, subA->getReviewer());
    EXPECT_EQ("Batch", subA->getReviewMethod());
}

TEST_F(AssignmentFlowTest, AssignmentCannotPublishWhileASubmissionIsRejected)
{
    markAll();
    approveBatch();

    EXPECT_FALSE(coordinator->isReadyToPublish(*assignment));
    EXPECT_FALSE(assignment->tryPublish());
    EXPECT_EQ("Review", assignment->getPhaseName());
}

TEST_F(AssignmentFlowTest, LecturerAcceptsTheLowMarkAndTheAssignmentPublishes)
{
    markAll();
    approveBatch();

    EXPECT_TRUE(coordinator->resolveRejected(*subG, true, 0, lecturer));

    EXPECT_EQ(35, subG->getMark());
    EXPECT_EQ(lecturer, subG->getReviewer());
    EXPECT_EQ("Personal", subG->getReviewMethod());
    EXPECT_TRUE(coordinator->isReadyToPublish(*assignment));
    EXPECT_EQ("Publish", assignment->getPhaseName()); // published automatically
}

TEST_F(AssignmentFlowTest, LecturerEntersTheirOwnMark)
{
    markAll();
    approveBatch();

    EXPECT_TRUE(coordinator->resolveRejected(*subG, false, 52, lecturer));

    EXPECT_EQ(52, subG->getMark());
    EXPECT_TRUE(coordinator->isReadyToPublish(*assignment));
}

TEST_F(AssignmentFlowTest, OnlyALecturerMayResolve)
{
    markAll();
    approveBatch();

    EXPECT_FALSE(coordinator->resolveRejected(*subG, true, 0, tas[0]));
    EXPECT_FALSE(coordinator->resolveRejected(*subG, true, 0, nullptr));
    EXPECT_FALSE(coordinator->isReadyToPublish(*assignment));
}

TEST_F(AssignmentFlowTest, ResolvingWithAnInvalidMarkIsRefused)
{
    markAll();
    approveBatch();

    EXPECT_FALSE(coordinator->resolveRejected(*subG, false, 101, lecturer));
    EXPECT_EQ("Rejected", subG->getStateName());
}

TEST_F(AssignmentFlowTest, ResolvingAnApprovedSubmissionIsRefused)
{
    markAll();
    approveBatch();

    EXPECT_FALSE(coordinator->resolveRejected(*subA, false, 90, lecturer));
}

TEST_F(AssignmentFlowTest, SecondBatchSkipsSubmissionsAlreadyDealtWith)
{
    markAll();
    approveBatch();

    AssignmentMarkingCoordinator::BatchResult again = coordinator->approveBatch(*assignment);

    EXPECT_TRUE(again.approvedIds.empty());
    EXPECT_TRUE(again.rejectedIds.empty());
    EXPECT_EQ(3u, again.skippedIds.size());
}

TEST_F(AssignmentFlowTest, BatchApprovalOutsideReviewThrows)
{
    markItem(subA, 78); // assignment is still Active

    EXPECT_THROW(coordinator->approveBatch(*assignment), InvalidTransition);
}

TEST_F(AssignmentFlowTest, BatchApprovalRequestedTooEarlyIsLoggedNotThrown)
{
    markItem(subA, 78);

    EXPECT_NO_THROW(approveBatch());

    EXPECT_EQ(nullptr, subA->getReviewer());
}

TEST_F(AssignmentFlowTest, AMarkAboveHundredEndsUpRejectedForTheLecturer)
{
    markItem(subA, 78);
    markItem(subB, 65);
    markItem(subG, 150);

    approveBatch();

    EXPECT_EQ("Rejected", subG->getStateName());
}

TEST_F(AssignmentFlowTest, HistoryRecordsTheWholeFlow)
{
    markAll();
    approveBatch();

    const std::vector<std::string> history = coordinator->getHistoryAsStrings();
    bool sawMark = false, sawBatch = false;
    for (unsigned i = 0; i < history.size(); ++i)
    {
        if (history[i].find("SUB-G marked 35") != std::string::npos) sawMark = true;
        if (history[i].find("Batch approval") != std::string::npos) sawBatch = true;
    }
    EXPECT_TRUE(sawMark);
    EXPECT_TRUE(sawBatch);
}

// ---- no TAs available ---------------------------------------------------------

TEST(AssignmentMarkingCoordinatorNoMarkersTest, SubmissionsStayAvailableAndTheFailureIsLogged)
{
    ParticipantRegistry participants;
    participants.add("Dr Dlamini", EducationRoles::lecturer()); // no TAs
    WorkRegistry registry;
    AssignmentMarkingCoordinator coordinator(participants);

    AssignmentMarkingBuilder builder(registry);
    builder.buildRoot("COS 214");
    builder.buildContainer("A1", "Assignment 1");
    builder.buildItem("SUB-A", "Student A");

    EXPECT_EQ(0, coordinator.openAssignment(*builder.getCurrentContainer()));

    EXPECT_EQ("Available", registry.get("SUB-A") == nullptr ? "" :
              dynamic_cast<Submission*>(registry.get("SUB-A"))->getStateName());
    const std::vector<std::string> history = coordinator.getHistoryAsStrings();
    bool logged = false;
    for (unsigned i = 0; i < history.size(); ++i)
    {
        if (history[i].find("No marker available") != std::string::npos) logged = true;
    }
    EXPECT_TRUE(logged);
}
