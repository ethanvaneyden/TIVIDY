#include <gtest/gtest.h>

#include "Container.h"
#include "Item.h"
#include "Participant.h"

namespace
{
class ItemLifecycleTest : public ::testing::Test
{
protected:
    // Destroy the container before the item, and the item before its worker.
    Participant worker{"worker-1", "Alex", "Worker"};
    Item item{"item-1", "work", "Record a result"};
    Container container{"container-1", "batch", "Work batch"};

    void SetUp() override
    {
        container.add(&item);
        container.advancePhase();
    }
};
}

TEST_F(ItemLifecycleTest, NewItemIsAvailableWithoutExecutionOrReviewData)
{
    EXPECT_EQ(item.getStateName(), "Available");
    EXPECT_EQ(item.getParent(), &container);
    EXPECT_EQ(item.getAssignee(), nullptr);
    EXPECT_FALSE(item.hasResult());
    EXPECT_FALSE(item.isCompleted());
    EXPECT_FALSE(item.isReviewed());
    EXPECT_EQ(item.getReviewer(), nullptr);
    EXPECT_TRUE(item.getReviewMethod().empty());
    EXPECT_TRUE(item.getRejectionReason().empty());
    EXPECT_EQ(worker.getPendingTasks(), 0);
}

TEST_F(ItemLifecycleTest, AssignmentRecordsWorkerAndEntersAssignedState)
{
    item.assign(&worker);

    EXPECT_EQ(item.getStateName(), "Assigned");
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_FALSE(item.hasResult());
    EXPECT_FALSE(item.isCompleted());
    EXPECT_FALSE(item.isReviewed());
    EXPECT_EQ(worker.getPendingTasks(), 1);
}

TEST_F(ItemLifecycleTest, StartingAssignedWorkEntersInProgressWithoutAResult)
{
    item.assign(&worker);
    item.start();

    EXPECT_EQ(item.getStateName(), "InProgress");
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_FALSE(item.hasResult());
    EXPECT_FALSE(item.isCompleted());
    EXPECT_FALSE(item.isReviewed());
    EXPECT_EQ(worker.getPendingTasks(), 1);
}

TEST_F(ItemLifecycleTest, CompletingWorkRecordsResultWithoutApprovingIt)
{
    item.assign(&worker);
    item.start();
    item.complete(72);

    EXPECT_EQ(item.getStateName(), "Completed");
    EXPECT_EQ(item.getAssignee(), &worker);
    ASSERT_TRUE(item.hasResult());
    EXPECT_EQ(item.getResult(), 72);
    EXPECT_TRUE(item.isCompleted());
    EXPECT_FALSE(item.isReviewed());
    EXPECT_EQ(item.getReviewer(), nullptr);
    EXPECT_TRUE(item.getReviewMethod().empty());
    EXPECT_TRUE(item.getRejectionReason().empty());
    EXPECT_EQ(worker.getPendingTasks(), 0);
    EXPECT_EQ(container.getPhaseName(), "Active");
}
