#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "Container.h"
#include "InvalidTransition.h"
#include "Item.h"
#include "Participant.h"

namespace
{
class ItemLifecycleTest : public ::testing::Test
{
protected:
    // Destroy the container before the item, and the item before its worker.
    Participant worker{"worker-1", "Alex", "Worker"};
    Participant other{"worker-2", "Blair", "Worker"};
    Item item{"item-1", "work", "Record a result"};
    Container container{"container-1", "batch", "Work batch"};

    void SetUp() override
    {
        container.add(&item);
        container.advancePhase();
    }

    // A rejected action must preserve all public lifecycle data, not just throw.
    template <typename Exception, typename Operation>
    void expectRejected(Operation operation)
    {
        const std::string state = item.getStateName();
        const std::string phase = container.getPhaseName();
        Component* parent = item.getParent();
        Participant* assignee = item.getAssignee();
        Participant* reviewer = item.getReviewer();
        const std::string reviewMethod = item.getReviewMethod();
        const std::string reason = item.getRejectionReason();
        const bool hasResult = item.hasResult();
        const int result = hasResult ? item.getResult() : 0;
        const bool completed = item.isCompleted();
        const bool reviewed = item.isReviewed();
        const int workerTasks = worker.getPendingTasks();
        const int otherTasks = other.getPendingTasks();

        EXPECT_THROW(operation(), Exception);

        EXPECT_EQ(item.getStateName(), state);
        EXPECT_EQ(container.getPhaseName(), phase);
        EXPECT_EQ(item.getParent(), parent);
        EXPECT_EQ(item.getAssignee(), assignee);
        EXPECT_EQ(item.getReviewer(), reviewer);
        EXPECT_EQ(item.getReviewMethod(), reviewMethod);
        EXPECT_EQ(item.getRejectionReason(), reason);
        EXPECT_EQ(item.hasResult(), hasResult);
        if (hasResult && item.hasResult()) EXPECT_EQ(item.getResult(), result);
        EXPECT_EQ(item.isCompleted(), completed);
        EXPECT_EQ(item.isReviewed(), reviewed);
        EXPECT_EQ(worker.getPendingTasks(), workerTasks);
        EXPECT_EQ(other.getPendingTasks(), otherTasks);
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

TEST_F(ItemLifecycleTest, AvailableItemRejectsStartingOrCompletingWork)
{
    expectRejected<std::logic_error>([&] { item.start(); });
    expectRejected<InvalidTransition>([&] { item.complete(99); });
}

TEST_F(ItemLifecycleTest, AssignedItemRejectsCompletionAndReassignment)
{
    item.assign(&worker);

    expectRejected<InvalidTransition>([&] { item.complete(99); });
    expectRejected<InvalidTransition>([&] { item.assign(&other); });
    expectRejected<InvalidTransition>([&] { item.assign(&worker); });
}

TEST_F(ItemLifecycleTest, InProgressItemRejectsRestartAndReassignment)
{
    item.assign(&worker);
    item.start();

    expectRejected<InvalidTransition>([&] { item.start(); });
    expectRejected<InvalidTransition>([&] { item.assign(&other); });
    expectRejected<InvalidTransition>([&] { item.assign(&worker); });
}

TEST_F(ItemLifecycleTest, CompletedItemRejectsWorkerChanges)
{
    item.assign(&worker);
    item.start();
    item.complete(72);

    expectRejected<InvalidTransition>([&] { item.complete(99); });
    expectRejected<InvalidTransition>([&] { item.start(); });
    expectRejected<InvalidTransition>([&] { item.assign(&other); });
}

TEST_F(ItemLifecycleTest, RejectedItemRejectsWorkerActionsAndRepeatedRejection)
{
    item.assign(&worker);
    item.start();
    item.complete(72);
    container.advancePhase();
    item.reject("Personal review required");

    expectRejected<InvalidTransition>([&] { item.assign(&other); });
    expectRejected<InvalidTransition>([&] { item.start(); });
    expectRejected<InvalidTransition>([&] { item.complete(99); });
    expectRejected<InvalidTransition>([&] { item.reject("Replacement reason"); });
    expectRejected<InvalidTransition>([&] { item.recordReview(&other, "Batch"); });
}

TEST_F(ItemLifecycleTest, NullAssigneeDoesNotChangeAvailableItem)
{
    expectRejected<std::invalid_argument>([&] { item.assign(nullptr); });
}

TEST_F(ItemLifecycleTest, MissingResultCannotBeRead)
{
    expectRejected<std::logic_error>([&] { item.getResult(); });
}

TEST_F(ItemLifecycleTest, EmptyRejectionReasonDoesNotChangeCompletedResult)
{
    item.assign(&worker);
    item.start();
    item.complete(72);
    container.advancePhase();

    expectRejected<std::invalid_argument>([&] { item.reject(""); });
}

TEST_F(ItemLifecycleTest, NullReviewerDoesNotApproveCompletedItem)
{
    item.assign(&worker);
    item.start();
    item.complete(72);
    container.advancePhase();

    expectRejected<std::invalid_argument>([&] { item.recordReview(nullptr, "Batch"); });
}

TEST_F(ItemLifecycleTest, EmptyReviewMethodDoesNotReplaceExistingReview)
{
    item.assign(&worker);
    item.start();
    item.complete(72);
    container.advancePhase();
    item.recordReview(&other, "Batch");

    expectRejected<std::invalid_argument>([&] { item.recordReview(&worker, ""); });
}

TEST_F(ItemLifecycleTest, NullReviewerCannotResolveRejectedResult)
{
    item.assign(&worker);
    item.start();
    item.complete(72);
    container.advancePhase();
    item.reject("Personal review required");

    expectRejected<std::invalid_argument>([&] { item.resolveReview(nullptr, 99); });
}

TEST_F(ItemLifecycleTest, PersonalResolutionRequiresRejectedState)
{
    item.assign(&worker);
    item.start();
    item.complete(72);
    container.advancePhase();
    item.recordReview(&other, "Batch");

    expectRejected<InvalidTransition>([&] { item.resolveReview(&worker, 99); });
}

TEST_F(ItemLifecycleTest, ReviewActionsCannotRunDuringActive)
{
    item.assign(&worker);
    item.start();
    item.complete(72);

    expectRejected<InvalidTransition>([&] { item.reject("Personal review required"); });
    expectRejected<InvalidTransition>([&] { item.recordReview(&other, "Batch"); });
}

TEST_F(ItemLifecycleTest, PublishedItemRejectsReviewChanges)
{
    item.assign(&worker);
    item.start();
    item.complete(72);
    container.advancePhase();
    item.recordReview(&other, "Batch");
    container.advancePhase();

    expectRejected<InvalidTransition>([&] { item.reject("Replacement reason"); });
    expectRejected<InvalidTransition>([&] { item.recordReview(&worker, "Personal"); });
    expectRejected<InvalidTransition>([&] { item.resolveReview(&worker, 99); });
}

namespace
{
class ItemReviewTest : public ItemLifecycleTest
{
protected:
    void SetUp() override
    {
        ItemLifecycleTest::SetUp();
        item.assign(&worker);
        item.start();
        item.complete(72);
        container.advancePhase();
    }
};
}

TEST_F(ItemReviewTest, BatchReviewApprovesWithoutChangingCompletedWork)
{
    item.recordReview(&other, "Batch");

    EXPECT_EQ(item.getStateName(), "Completed");
    EXPECT_TRUE(item.isCompleted());
    EXPECT_TRUE(item.isReviewed());
    EXPECT_EQ(item.getReviewer(), &other);
    EXPECT_EQ(item.getReviewMethod(), "Batch");
    ASSERT_TRUE(item.hasResult());
    EXPECT_EQ(item.getResult(), 72);
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_TRUE(item.getRejectionReason().empty());
    EXPECT_EQ(worker.getPendingTasks(), 0);
    EXPECT_EQ(other.getPendingTasks(), 0);
}

TEST_F(ItemReviewTest, RejectionRetainsResultAndAssigneeForPersonalReview)
{
    item.reject("Outside automatic approval band");

    EXPECT_EQ(item.getStateName(), "Rejected");
    EXPECT_FALSE(item.isCompleted());
    EXPECT_FALSE(item.isReviewed());
    ASSERT_TRUE(item.hasResult());
    EXPECT_EQ(item.getResult(), 72);
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_EQ(item.getRejectionReason(), "Outside automatic approval band");
    EXPECT_EQ(item.getReviewer(), nullptr);
    EXPECT_TRUE(item.getReviewMethod().empty());
    EXPECT_EQ(worker.getPendingTasks(), 0);
}

TEST_F(ItemReviewTest, RejectionClearsPreviousApproval)
{
    item.recordReview(&other, "Batch");
    ASSERT_TRUE(item.isReviewed());

    item.reject("Approval needs reconsideration");

    EXPECT_EQ(item.getStateName(), "Rejected");
    EXPECT_FALSE(item.isReviewed());
    EXPECT_EQ(item.getReviewer(), nullptr);
    EXPECT_TRUE(item.getReviewMethod().empty());
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_EQ(item.getResult(), 72);
    EXPECT_EQ(item.getRejectionReason(), "Approval needs reconsideration");
}

TEST_F(ItemReviewTest, PersonalReviewCanAcceptTheExistingResult)
{
    item.reject("Needs a personal decision");
    item.resolveReview(&other, item.getResult());

    EXPECT_EQ(item.getStateName(), "Completed");
    EXPECT_TRUE(item.isCompleted());
    EXPECT_TRUE(item.isReviewed());
    ASSERT_TRUE(item.hasResult());
    EXPECT_EQ(item.getResult(), 72);
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_EQ(item.getReviewer(), &other);
    EXPECT_EQ(item.getReviewMethod(), "Personal");
    EXPECT_EQ(item.getRejectionReason(), "Needs a personal decision");
    EXPECT_EQ(worker.getPendingTasks(), 0);
    EXPECT_EQ(other.getPendingTasks(), 0);
}

TEST_F(ItemReviewTest, PersonalReviewCanCorrectTheRejectedResult)
{
    item.reject("Result needs correction");
    item.resolveReview(&other, 85, "Corrected after checking the work");

    EXPECT_EQ(item.getStateName(), "Completed");
    EXPECT_TRUE(item.isCompleted());
    EXPECT_TRUE(item.isReviewed());
    ASSERT_TRUE(item.hasResult());
    EXPECT_EQ(item.getResult(), 85);
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_EQ(item.getReviewer(), &other);
    EXPECT_EQ(item.getReviewMethod(), "Personal");
    // Resolution preserves the rejection reason for later inspection.
    EXPECT_EQ(item.getRejectionReason(), "Result needs correction");
    EXPECT_EQ(worker.getPendingTasks(), 0);
    EXPECT_EQ(other.getPendingTasks(), 0);
}

TEST_F(ItemReviewTest, RepeatedReviewCyclesReplaceApprovalAndKeepLatestRejection)
{
    item.reject("First review required");
    item.resolveReview(&other, 80);
    ASSERT_TRUE(item.isReviewed());

    item.reject("Second review required");
    EXPECT_FALSE(item.isReviewed());
    EXPECT_EQ(item.getReviewer(), nullptr);
    EXPECT_TRUE(item.getReviewMethod().empty());
    EXPECT_EQ(item.getResult(), 80);

    item.resolveReview(&worker, 81);
    EXPECT_EQ(item.getStateName(), "Completed");
    EXPECT_TRUE(item.isReviewed());
    EXPECT_EQ(item.getResult(), 81);
    EXPECT_EQ(item.getReviewer(), &worker);
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_EQ(item.getReviewMethod(), "Personal");
    EXPECT_EQ(item.getRejectionReason(), "Second review required");
    EXPECT_EQ(worker.getPendingTasks(), 0);
}

TEST_F(ItemReviewTest, ReplacingABatchReviewRecordsTheLatestReviewer)
{
    item.recordReview(&other, "Batch");
    ASSERT_EQ(item.getReviewer(), &other);

    item.recordReview(&worker, "Batch");

    EXPECT_TRUE(item.isReviewed());
    EXPECT_EQ(item.getReviewer(), &worker);
    EXPECT_EQ(item.getReviewMethod(), "Batch");
    EXPECT_EQ(item.getAssignee(), &worker);
    EXPECT_EQ(item.getResult(), 72);
    EXPECT_EQ(worker.getPendingTasks(), 0);
    EXPECT_EQ(other.getPendingTasks(), 0);
}

#include "WorkflowMediator.h"
#include <functional>
#include <vector>

namespace {
class RecordingLifecycleMediator : public WorkflowMediator {
public:
    std::vector<std::string> events;
    std::function<void(const std::string&, Item*)> inspect;
    Participant* participant = nullptr;
    int value = 0;
    std::string reason;
    void record(const std::string& event, Item* item) {
        events.push_back(event);
        if (inspect) inspect(event, item);
    }
    void onItemAssigned(Item* i, Participant* p) override { participant = p; record("assigned", i); }
    void onItemStarted(Item* i) override { record("started", i); }
    void onItemCompleted(Item* i, int v) override { value = v; record("completed", i); }
    void onItemRejected(Item* i, const std::string& r) override { reason = r; record("rejected", i); }
    void onItemOverridden(Item* i, Participant* p, int v, const std::string& r) override {
        participant = p; value = v; reason = r; record("overridden", i);
    }
    void onPhaseAdvanced(Container*) override { events.push_back("phase"); }
    void onItemAdded(Item*) override { events.push_back("added"); }
    void onAssignmentFailed(Item*, const std::string&) override { events.push_back("assignment-failed"); }
    void onContainerApprovalRequested(Container*, Participant*) override { events.push_back("approval-requested"); }
    void onContainerRejected(Container*, const std::string&) override { events.push_back("container-rejected"); }
    void onApprovalResolved(Item*, bool) override { events.push_back("approval-resolved"); }
    std::vector<std::string> getHistoryAsStrings() const override { return events; }
};

class LifecycleNotificationTest : public ::testing::Test {
protected:
    RecordingLifecycleMediator mediator;
    Participant worker{"worker", "Worker", "Worker"};
    Participant reviewer{"reviewer", "Reviewer", "Reviewer"};
    Item item{"item", "work", "Item"};
    Container container{"container", "batch", "Batch"};
    void SetUp() override {
        container.add(&item);
        container.advancePhase();
        item.setMediator(&mediator);
    }
    void enterReview() {
        item.assign(&worker);
        item.start();
        item.complete(60);
        container.advancePhase();
    }
};
}

TEST_F(LifecycleNotificationTest, ReportsExactArgumentsInOrderAfterStateAndDataChange) {
    mediator.inspect = [&](const std::string& event, Item* actual) {
        EXPECT_EQ(actual, &item);
        if (event == "assigned") {
            EXPECT_EQ(item.getStateName(), "Assigned");
            EXPECT_EQ(item.getAssignee(), &worker);
            EXPECT_EQ(mediator.participant, &worker);
            EXPECT_EQ(worker.getPendingTasks(), 1);
        } else if (event == "started") {
            EXPECT_EQ(item.getStateName(), "InProgress");
        } else if (event == "completed") {
            EXPECT_EQ(item.getStateName(), "Completed");
            EXPECT_EQ(item.getResult(), 60);
            EXPECT_EQ(mediator.value, 60);
            EXPECT_EQ(worker.getPendingTasks(), 0);
        } else if (event == "rejected") {
            EXPECT_EQ(item.getStateName(), "Rejected");
            EXPECT_EQ(mediator.reason, "Check result");
            EXPECT_EQ(item.getRejectionReason(), "Check result");
            EXPECT_EQ(item.getReviewer(), nullptr);
            EXPECT_TRUE(item.getReviewMethod().empty());
        } else if (event == "overridden") {
            EXPECT_EQ(item.getStateName(), "Completed");
            EXPECT_EQ(item.getReviewer(), &reviewer);
            EXPECT_EQ(item.getReviewMethod(), "Personal");
            EXPECT_EQ(item.getResult(), 82);
            EXPECT_EQ(mediator.participant, &reviewer);
            EXPECT_EQ(mediator.value, 82);
            EXPECT_EQ(mediator.reason, "Corrected by operator");
        }
    };
    enterReview();
    item.recordReview(&reviewer, "Batch");
    item.reject("Check result");
    item.resolveReview(&reviewer, 82, "Corrected by operator");
    EXPECT_EQ(mediator.events, (std::vector<std::string>{
        "assigned", "started", "completed", "rejected", "overridden"}));
}

TEST_F(LifecycleNotificationTest, InvalidActionsDoNotSendSuccessCallbacks) {
    EXPECT_THROW(item.assign(nullptr), std::invalid_argument);
    EXPECT_THROW(item.complete(5), InvalidTransition);
    EXPECT_TRUE(mediator.events.empty());
    enterReview();
    mediator.events.clear();
    EXPECT_THROW(item.assign(&worker), InvalidTransition);
    EXPECT_THROW(item.start(), InvalidTransition);
    EXPECT_THROW(item.complete(5), InvalidTransition);
    EXPECT_THROW(item.reject(""), std::invalid_argument);
    EXPECT_THROW(item.resolveReview(&reviewer, 5), InvalidTransition);
    EXPECT_THROW(item.resolveReview(nullptr, 5), std::invalid_argument);
    EXPECT_TRUE(mediator.events.empty());
    item.reject("Check");
    mediator.events.clear();
    EXPECT_THROW(item.reject("Again"), InvalidTransition);
    EXPECT_TRUE(mediator.events.empty());
}

TEST_F(LifecycleNotificationTest, DetachedMediatorReceivesNoFurtherNotifications) {
    item.assign(&worker);
    item.setMediator(nullptr);
    item.start();
    item.complete(60);
    container.advancePhase();
    item.reject("Check");
    item.resolveReview(&reviewer, 82);
    EXPECT_EQ(mediator.events, (std::vector<std::string>{"assigned"}));
    EXPECT_TRUE(item.isReviewed());
}

TEST_F(LifecycleNotificationTest, CallbackExceptionsPropagateAfterTransitionWithoutRollbackOrDuplicateEvents) {
    mediator.inspect = [](const std::string&, Item*) { throw std::runtime_error("callback failure"); };
    EXPECT_THROW(item.assign(&worker), std::runtime_error);
    EXPECT_EQ(item.getStateName(), "Assigned");
    EXPECT_EQ(worker.getPendingTasks(), 1);
    EXPECT_THROW(item.assign(&worker), InvalidTransition);
    EXPECT_THROW(item.start(), std::runtime_error);
    EXPECT_EQ(item.getStateName(), "InProgress");
    EXPECT_THROW(item.complete(60), std::runtime_error);
    EXPECT_EQ(item.getResult(), 60);
    EXPECT_EQ(worker.getPendingTasks(), 0);
    container.advancePhase();
    item.recordReview(&reviewer, "Batch");
    EXPECT_THROW(item.reject("Check"), std::runtime_error);
    EXPECT_EQ(item.getStateName(), "Rejected");
    EXPECT_EQ(item.getReviewer(), nullptr);
    EXPECT_THROW(item.resolveReview(&reviewer, 82, "Corrected"), std::runtime_error);
    EXPECT_TRUE(item.isReviewed());
    EXPECT_EQ(item.getResult(), 82);
    EXPECT_EQ(item.getReviewer(), &reviewer);
    EXPECT_EQ(mediator.events, (std::vector<std::string>{
        "assigned", "started", "completed", "rejected", "overridden"}));
}
