#include <gtest/gtest.h>

#include <limits>

#include "Container.h"
#include "Item.h"
#include "Participant.h"

TEST(ItemWorkloadTest, WorkloadRemainsPendingUntilWorkIsCompleted)
{
    Participant worker("worker-1", "Alex", "Worker");
    Item item("item-1", "work", "Record a result");
    Container container("container-1", "batch", "Work batch");
    container.add(&item);
    container.advancePhase();

    EXPECT_EQ(worker.getPendingTasks(), 0);

    item.assign(&worker);
    EXPECT_EQ(worker.getPendingTasks(), 1);

    item.start();
    EXPECT_EQ(worker.getPendingTasks(), 1);

    item.complete(72);
    EXPECT_EQ(worker.getPendingTasks(), 0);
    EXPECT_EQ(item.getAssignee(), &worker);
}

TEST(ItemWorkloadTest, CompletingAnItemReleasesOnlyItsOwnPendingTask)
{
    // Participants outlive items; the container detaches both before they die.
    Participant worker("worker-1", "Alex", "Worker");
    Item first("item-1", "work", "First task");
    Item second("item-2", "work", "Second task");
    Container container("container-1", "batch", "Work batch");
    container.add(&first);
    container.add(&second);
    container.advancePhase();

    first.assign(&worker);
    second.assign(&worker);
    ASSERT_EQ(worker.getPendingTasks(), 2);

    first.start();
    first.complete(72);
    EXPECT_EQ(worker.getPendingTasks(), 1);
    EXPECT_EQ(second.getStateName(), "Assigned");
    EXPECT_FALSE(second.hasResult());
    EXPECT_EQ(second.getAssignee(), &worker);

    second.start();
    second.complete(48);
    EXPECT_EQ(worker.getPendingTasks(), 0);
    EXPECT_TRUE(first.isCompleted());
    EXPECT_TRUE(second.isCompleted());
    EXPECT_EQ(first.getResult(), 72);
    EXPECT_EQ(second.getResult(), 48);
}

namespace
{
class ItemResultTest : public ::testing::TestWithParam<int>
{
protected:
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

TEST_P(ItemResultTest, CompletionPreservesTheSuppliedIntegerResult)
{
    // Scenario policies validate marks or severity; the generic item stores int.
    item.assign(&worker);
    item.start();
    item.complete(GetParam());

    ASSERT_TRUE(item.hasResult());
    EXPECT_EQ(item.getResult(), GetParam());
    EXPECT_EQ(item.getStateName(), "Completed");
    EXPECT_TRUE(item.isCompleted());
    EXPECT_FALSE(item.isReviewed());
    EXPECT_EQ(worker.getPendingTasks(), 0);
}

INSTANTIATE_TEST_SUITE_P(
    GenericResults,
    ItemResultTest,
    ::testing::Values(std::numeric_limits<int>::min(), -1, 0, 72,
                      std::numeric_limits<int>::max()));
