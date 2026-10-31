#include <gtest/gtest.h>

#include <limits>

#include "Container.h"
#include "InvalidTransition.h"
#include "Item.h"
#include "Participant.h"
#include "ParticipantRegistry.h"

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

TEST_P(ItemResultTest, PersonalReviewPreservesTheSuppliedIntegerCorrection)
{
    item.assign(&worker);
    item.start();
    item.complete(72);
    container.advancePhase();
    item.reject("Personal correction required");

    item.resolveReview(&worker, GetParam());

    ASSERT_TRUE(item.hasResult());
    EXPECT_EQ(item.getResult(), GetParam());
    EXPECT_EQ(item.getStateName(), "Completed");
    EXPECT_TRUE(item.isCompleted());
    EXPECT_TRUE(item.isReviewed());
    EXPECT_EQ(item.getReviewer(), &worker);
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_EQ(item.getReviewMethod(), "Personal");
    EXPECT_EQ(item.getRejectionReason(), "Personal correction required");
    EXPECT_EQ(worker.getPendingTasks(), 0);
}

INSTANTIATE_TEST_SUITE_P(
    GenericResults,
    ItemResultTest,
    ::testing::Values(std::numeric_limits<int>::min(), -1, 0, 72,
                      std::numeric_limits<int>::max()));

TEST(ItemPhaseValidationTest, WorkCannotBeginDuringIntake)
{
    Participant worker("worker-1", "Alex", "Worker");
    Item item("item-1", "work", "Record a result");
    Container container("container-1", "batch", "Work batch");
    container.add(&item);

    EXPECT_THROW(item.assign(&worker), InvalidTransition);
    EXPECT_THROW(item.start(), InvalidTransition);
    EXPECT_THROW(item.complete(99), InvalidTransition);
    EXPECT_EQ(container.getPhaseName(), "Intake");
    EXPECT_EQ(item.getStateName(), "Available");
    EXPECT_EQ(item.getParent(), &container);
    EXPECT_EQ(item.getAssignee(), nullptr);
    EXPECT_FALSE(item.hasResult());
    EXPECT_FALSE(item.isCompleted());
    EXPECT_FALSE(item.isReviewed());
    EXPECT_EQ(worker.getPendingTasks(), 0);
}

TEST(ItemPhaseValidationTest, DetachedItemRejectsLifecycleActions)
{
    Participant worker("worker-1", "Alex", "Worker");
    Item item("item-1", "work", "Record a result");

    EXPECT_THROW(item.assign(&worker), InvalidTransition);
    EXPECT_THROW(item.start(), InvalidTransition);
    EXPECT_THROW(item.complete(99), InvalidTransition);
    EXPECT_THROW(item.reject("Personal review required"), InvalidTransition);
    EXPECT_THROW(item.recordReview(&worker, "Batch"), InvalidTransition);
    EXPECT_THROW(item.resolveReview(&worker, 99), InvalidTransition);
    EXPECT_EQ(item.getParent(), nullptr);
    EXPECT_EQ(item.getStateName(), "Available");
    EXPECT_EQ(item.getAssignee(), nullptr);
    EXPECT_EQ(item.getReviewer(), nullptr);
    EXPECT_FALSE(item.hasResult());
    EXPECT_FALSE(item.isCompleted());
    EXPECT_FALSE(item.isReviewed());
    EXPECT_TRUE(item.getReviewMethod().empty());
    EXPECT_TRUE(item.getRejectionReason().empty());
    EXPECT_EQ(worker.getPendingTasks(), 0);
}

TEST(ItemWorkloadTest, ReviewAndRecoveryDoNotConsumeAnotherItemsPendingTask)
{
    Participant worker("worker-1", "Alex", "Worker");
    Participant reviewer("reviewer-1", "Blair", "Reviewer");
    Item result("result", "work", "Completed work");
    Item pending("pending", "work", "Other active work");
    Container reviewBatch("review-batch", "batch", "Review batch");
    Container activeBatch("active-batch", "batch", "Active batch");
    reviewBatch.add(&result);
    activeBatch.add(&pending);
    reviewBatch.advancePhase();
    activeBatch.advancePhase();
    result.assign(&worker);
    pending.assign(&worker);
    result.start();
    result.complete(72);
    reviewBatch.advancePhase();
    ASSERT_EQ(worker.getPendingTasks(), 1);

    result.recordReview(&reviewer, "Batch");
    EXPECT_EQ(worker.getPendingTasks(), 1);
    result.reject("Personal correction required");
    EXPECT_EQ(worker.getPendingTasks(), 1);
    result.resolveReview(&reviewer, 85);
    EXPECT_EQ(worker.getPendingTasks(), 1);
    EXPECT_EQ(reviewer.getPendingTasks(), 0);
    EXPECT_EQ(result.getAssignee(), &worker);
    EXPECT_TRUE(result.isReviewed());
    EXPECT_EQ(result.getResult(), 85);
    EXPECT_EQ(pending.getStateName(), "Assigned");
    EXPECT_EQ(pending.getAssignee(), &worker);
    EXPECT_FALSE(pending.hasResult());
    EXPECT_FALSE(pending.isReviewed());

    pending.start();
    pending.complete(48);
    EXPECT_EQ(worker.getPendingTasks(), 0);
}

TEST(ItemReviewLifetimeTest, SelfReviewAndRejectionRetainAssigneeUntilItemDestruction)
{
    ParticipantRegistry registry;
    Participant* worker = registry.add("Alex", "Worker");
    Participant* reviewer = registry.add("Blair", "Reviewer");
    {
        Item item("item-1", "work", "Record a result");
        Container container("container-1", "batch", "Work batch");
        container.add(&item);
        container.advancePhase();
        item.assign(worker);
        item.start();
        item.complete(72);
        container.advancePhase();
        item.recordReview(reviewer, "Batch");
        EXPECT_THROW(registry.remove("Reviewer1"), std::logic_error);

        item.reject("A personal review is required");
        EXPECT_EQ(item.getReviewer(), nullptr);
        EXPECT_TRUE(item.getReviewMethod().empty());
        // Once its approval is cleared, the old reviewer is no longer retained.
        EXPECT_NO_THROW(registry.remove("Reviewer1"));
        EXPECT_FALSE(registry.contains("Reviewer1"));
        reviewer = registry.add("Casey", "Reviewer");
        item.resolveReview(worker, 80);

        item.reject("A separate reviewer is required");
        EXPECT_EQ(item.getReviewer(), nullptr);
        EXPECT_TRUE(item.getReviewMethod().empty());
        EXPECT_EQ(item.getAssignee(), worker);
        EXPECT_THROW(registry.remove("Worker1"), std::logic_error);

        item.resolveReview(reviewer, 85);
        EXPECT_THROW(registry.remove("Worker1"), std::logic_error);
        EXPECT_THROW(registry.remove("Reviewer2"), std::logic_error);
        EXPECT_TRUE(item.isReviewed());
        EXPECT_EQ(worker->getPendingTasks(), 0);
        EXPECT_EQ(reviewer->getPendingTasks(), 0);
    }

    EXPECT_NO_THROW(registry.remove("Worker1"));
    EXPECT_NO_THROW(registry.remove("Reviewer2"));
    EXPECT_EQ(registry.size(), 0u);
}
