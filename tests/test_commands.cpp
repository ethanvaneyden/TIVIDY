#include <gtest/gtest.h>

#include "AdvancePhaseCommand.h"
#include "ApproveContainerCommand.h"
#include "AssignItemCommand.h"
#include "Container.h"
#include "InvalidTransition.h"
#include "Item.h"
#include "OverrideItemCommand.h"
#include "Participant.h"
#include "RecordResultCommand.h"
#include "RejectItemCommand.h"
#include "StartItemCommand.h"
#include "SystemLogCommand.h"

class CommandTest : public ::testing::Test
{
protected:
    Participant worker{"worker", "Worker", "TA"};
    Participant reviewer{"reviewer", "Reviewer", "Lecturer"};
    Item item{"item", "submission", "Submission"};
    Container container{"container", "assignment", "Assignment"};

    void SetUp() override
    {
        container.add(&item);
    }

    void completeItem(int value)
    {
        container.advancePhase();
        item.assign(&worker);
        item.start();
        item.complete(value);
        container.advancePhase();
    }
};

TEST_F(CommandTest, AssignItemCommandAssignsWorker)
{
    container.advancePhase();
    AssignItemCommand command(&item, &worker);

    command.execute();

    EXPECT_EQ(item.getStateName(), "Assigned");
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_EQ(worker.getPendingTasks(), 1);
    EXPECT_EQ(command.description(), "Assigned item to worker");
}

TEST_F(CommandTest, RecordResultCommandCompletesItem)
{
    container.advancePhase();
    item.assign(&worker);
    item.start();
    RecordResultCommand command(&item, 76);

    command.execute();

    EXPECT_EQ(item.getStateName(), "Completed");
    EXPECT_EQ(item.getResult(), 76);
    EXPECT_EQ(worker.getPendingTasks(), 0);
    EXPECT_EQ(command.description(), "Recorded result: 76");
}

TEST_F(CommandTest, StartItemCommandStartsAssignedItem)
{
    container.advancePhase();
    item.assign(&worker);
    StartItemCommand command(&item);

    command.execute();

    EXPECT_EQ(item.getStateName(), "InProgress");
    EXPECT_EQ(command.description(), "Started item item");
}

TEST_F(CommandTest, RejectItemCommandRejectsCompletedItem)
{
    completeItem(35);
    RejectItemCommand command(&item, "Below threshold");

    command.execute();

    EXPECT_EQ(item.getStateName(), "Rejected");
    EXPECT_EQ(item.getRejectionReason(), "Below threshold");
    EXPECT_EQ(command.description(), "Item rejected. Reason: Below threshold");
}

TEST_F(CommandTest, OverrideItemCommandResolvesRejectedItem)
{
    completeItem(35);
    item.reject("Below threshold");
    OverrideItemCommand command(&item, &reviewer, 45, "Corrected mark");

    command.execute();

    EXPECT_EQ(item.getStateName(), "Completed");
    EXPECT_EQ(item.getResult(), 45);
    EXPECT_EQ(item.getReviewer(), &reviewer);
    EXPECT_EQ(item.getReviewMethod(), "Personal");
}

TEST_F(CommandTest, AdvancePhaseCommandAdvancesContainer)
{
    AdvancePhaseCommand command(&container);

    command.execute();

    EXPECT_EQ(container.getPhaseName(), "Active");
}

TEST_F(CommandTest, ApproveContainerCommandPublishesReviewedContainer)
{
    completeItem(76);
    item.recordReview(&reviewer, "Batch");
    ApproveContainerCommand command(&container);

    command.execute();

    EXPECT_EQ(container.getPhaseName(), "Publish");
}

TEST_F(CommandTest, ApproveContainerCommandLeavesUnreviewedContainerInReview)
{
    completeItem(76);
    ApproveContainerCommand command(&container);

    EXPECT_NO_THROW(command.execute());
    EXPECT_EQ(container.getPhaseName(), "Review");
}

TEST_F(CommandTest, SystemLogCommandProvidesItsMessageAsDescription)
{
    SystemLogCommand command("Workflow event recorded");

    EXPECT_NO_THROW(command.execute());
    EXPECT_EQ(command.description(), "Workflow event recorded");
}

TEST(WorkCommandTest, NullReceiversHaveSafeDescriptions)
{
    EXPECT_EQ(AssignItemCommand(nullptr, nullptr).description(), "Invalid item assignment");
    EXPECT_EQ(RecordResultCommand(nullptr, 1).description(), "Invalid result recording");
    EXPECT_EQ(RejectItemCommand(nullptr, "reason").description(), "Invalid item rejection");
    EXPECT_EQ(OverrideItemCommand(nullptr, nullptr, 1, "reason").description(),
              "Invalid item override");
    EXPECT_EQ(AdvancePhaseCommand(nullptr).description(), "Invalid phase advancement");
    EXPECT_EQ(ApproveContainerCommand(nullptr).description(), "Invalid container approval");
}
