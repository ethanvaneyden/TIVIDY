// EducationFacade driven the way the REST server and UI drive it.

#include <gtest/gtest.h>

#include "EducationFacade.h"
#include "EducationPolicies.h"
#include "EducationSeed.h"
#include "Container.h"
#include "Item.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Root.h"

#include <string>
#include <vector>

using education::EducationRoles;
using education::EducationSeed;

namespace
{
    bool historyHas(const std::vector<std::string>& history, const std::string& text)
    {
        for (unsigned i = 0; i < history.size(); ++i)
        {
            if (history[i].find(text) != std::string::npos) return true;
        }
        return false;
    }

    Component* child(Root* root, const std::string& id)
    {
        Component* assignment = root->getChildren().front();
        const std::vector<Component*> children = assignment->getChildren();
        for (unsigned i = 0; i < children.size(); ++i)
        {
            if (children[i]->getId() == id) return children[i];
        }
        return nullptr;
    }

    Item* item(Root* root, const std::string& id)
    {
        return dynamic_cast<Item*>(child(root, id));
    }
}

class EducationFacadeTest : public ::testing::Test
{
    protected:
        ParticipantRegistry participants; // declared first so it is destroyed last
        std::unique_ptr<EducationFacade> facade;
        std::string id;

        void SetUp()
        {
            facade.reset(new EducationFacade(&participants));
            id = facade->start("COS 214");
        }

        Container* assignment() const
        {
            return dynamic_cast<Container*>(facade->getTree(id)->getChildren().front());
        }

        void markAll()
        {
            const std::vector<education::DemoSubmission> rows = EducationSeed::demoSubmissions();
            for (unsigned i = 0; i < rows.size(); ++i)
            {
                ASSERT_EQ(CommandResult::Success, facade->startItem(id, rows[i].itemId));
                ASSERT_EQ(CommandResult::Success, facade->completeItem(id, rows[i].itemId, rows[i].mark));
            }
        }
};

TEST_F(EducationFacadeTest, ConstructorAddsTheEducationStaff)
{
    EXPECT_EQ(3u, participants.getByRole(EducationRoles::ta()).size());
    EXPECT_EQ(1u, participants.getByRole(EducationRoles::lecturer()).size());
    EXPECT_EQ(1u, participants.getByRole(EducationRoles::moderator()).size());
}

TEST_F(EducationFacadeTest, SecondFacadeOnTheSameRegistryDoesNotSeedAgain)
{
    EducationFacade another(&participants);

    EXPECT_EQ(3u, participants.getByRole(EducationRoles::ta()).size());
}

TEST_F(EducationFacadeTest, AssignmentStaysOpenForSubmissionsUntilMarkingStarts)
{
    EXPECT_EQ("Intake", assignment()->getPhaseName());

    EXPECT_EQ(CommandResult::Success, facade->submit(id, "Student New"));

    Item* added = item(facade->getTree(id), "SUB-1");
    ASSERT_NE(nullptr, added);
    EXPECT_EQ("Student New", added->getName());
    EXPECT_EQ("Available", added->getStateName());
}

TEST_F(EducationFacadeTest, StartingTheFirstSubmissionOpensTheAssignmentAndAssignsTAs)
{
    EXPECT_EQ(CommandResult::Success, facade->startItem(id, "SUB-A"));

    EXPECT_EQ("Active", assignment()->getPhaseName());
    EXPECT_EQ("InProgress", item(facade->getTree(id), "SUB-A")->getStateName());
    Item* other = item(facade->getTree(id), "SUB-B");
    EXPECT_EQ("Assigned", other->getStateName());
    ASSERT_NE(nullptr, other->getAssignee());
    EXPECT_EQ(EducationRoles::ta(), other->getAssignee()->getRole());
}

TEST_F(EducationFacadeTest, SubmitIsRefusedOnceMarkingHasStarted)
{
    facade->startItem(id, "SUB-A");

    EXPECT_EQ(CommandResult::InvalidTransition, facade->submit(id, "Late Student"));
}

TEST_F(EducationFacadeTest, LastMarkMovesTheAssignmentToReview)
{
    markAll();

    EXPECT_EQ("Review", assignment()->getPhaseName());
}

TEST_F(EducationFacadeTest, ApproveRejectsLowMarksAndDoesNotPublish)
{
    markAll();

    EXPECT_EQ(CommandResult::Success, facade->approveContainer(id, "A1"));

    EXPECT_EQ("Rejected", item(facade->getTree(id), "SUB-G")->getStateName());
    EXPECT_EQ("Rejected", item(facade->getTree(id), "SUB-H")->getStateName());
    EXPECT_EQ("Review", assignment()->getPhaseName());
}

TEST_F(EducationFacadeTest, LecturerOverridesPublishTheResults)
{
    markAll();
    facade->approveContainer(id, "A1");

    EXPECT_EQ(CommandResult::Success, facade->overrideItem(id, "SUB-G", 0, true));
    EXPECT_EQ(CommandResult::Success, facade->overrideItem(id, "SUB-H", 42, false));

    EXPECT_EQ("Publish", assignment()->getPhaseName());
    EXPECT_TRUE(historyHas(facade->getHistory(id), "[System] Marks released for all students. Emails sent and LMS synced."));
}

TEST_F(EducationFacadeTest, InvalidMarkIsRefused)
{
    facade->startItem(id, "SUB-A");

    EXPECT_EQ(CommandResult::InvalidTransition, facade->completeItem(id, "SUB-A", 101));
}

TEST_F(EducationFacadeTest, UnknownInstanceIsNotFound)
{
    EXPECT_EQ(CommandResult::NotFound, facade->startItem("nope", "SUB-A"));
    EXPECT_EQ(CommandResult::NotFound, facade->submit("nope", "Student"));
}

TEST_F(EducationFacadeTest, StrategyChangeReachesTheCoordinator)
{
    EXPECT_EQ("Workload", facade->getStrategyName(id));

    EXPECT_EQ(CommandResult::Success, facade->setStrategy(id, "RoundRobin"));
    EXPECT_EQ("RoundRobin", facade->getStrategyName(id));
    EXPECT_EQ(CommandResult::NotFound, facade->setStrategy(id, "Nonsense"));
}
