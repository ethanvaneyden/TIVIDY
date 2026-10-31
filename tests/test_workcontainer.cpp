#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "Container.h"
#include "DepthFirstIterator.h"
#include "Item.h"
#include "InvalidTransition.h"
#include "Root.h"
#include "WorkRegistry.h"

TEST(RootTest, TracksContainersAndEnforcesSingleParent)
{
    Root root("root", "workflow", "Workflow");
    Root otherRoot("other-root", "workflow", "Other workflow");
    Container container("container", "group", "Container");

    root.add(&container);

    const std::vector<Component*> children = root.getChildren();
    ASSERT_EQ(children.size(), 1u);
    EXPECT_EQ(children.front(), &container);
    EXPECT_EQ(container.getParent(), &root);
    EXPECT_THROW(root.add(&container), std::invalid_argument);
    EXPECT_THROW(otherRoot.add(&container), std::invalid_argument);

    root.remove("container");
    EXPECT_EQ(container.getParent(), nullptr);
    EXPECT_TRUE(root.getChildren().empty());
}

TEST(RootTest, RejectsRemovingContainerOutsideIntake)
{
    Root root("root", "workflow", "Workflow");
    Container container("container", "group", "Container");
    Item item("item", "work", "Item");
    root.add(&container);
    container.add(&item);
    container.advancePhase();

    EXPECT_THROW(root.remove("container"), InvalidTransition);
    ASSERT_EQ(root.getChildren().size(), 1u);
    EXPECT_EQ(container.getParent(), &root);
}

TEST(RootTest, CreatesDepthFirstIteratorForWorkflowTree)
{
    Root root("root", "workflow", "Workflow");
    Container container("container", "group", "Container");
    Item item("item", "work", "Item");
    root.add(&container);
    container.add(&item);

    std::unique_ptr<Iterator> iterator(root.createIterator());
    std::vector<std::string> names;
    for (iterator->first(); !iterator->isDone(); iterator->next())
        names.push_back(iterator->current()->getName());

    EXPECT_EQ(names, (std::vector<std::string>{"Workflow", "Container", "Item"}));
}

TEST(ContainerTest, CreatesDepthFirstIteratorFromContainer)
{
    Container container("container", "group", "Container");
    Item first("first", "work", "First");
    Item second("second", "work", "Second");
    container.add(&first);
    container.add(&second);

    std::unique_ptr<Iterator> iterator(container.createIterator());
    std::vector<std::string> names;
    for (iterator->first(); !iterator->isDone(); iterator->next())
        names.push_back(iterator->current()->getName());

    EXPECT_EQ(names, (std::vector<std::string>{"Container", "First", "Second"}));
}

TEST(WorkRegistryTest, RemovesRegisteredTreeAndDetachesDescendants)
{
    WorkRegistry registry;
    Root* root = static_cast<Root*>(
        registry.add(std::unique_ptr<Component>(new Root("root", "workflow", "Workflow"))));
    Container* container = static_cast<Container*>(
        registry.add(std::unique_ptr<Component>(new Container("container", "group", "Container"))));
    Item* item = static_cast<Item*>(
        registry.add(std::unique_ptr<Component>(new Item("item", "work", "Item"))));

    root->add(container);
    container->add(item);

    registry.remove("root");

    EXPECT_EQ(registry.size(), 0u);
    EXPECT_FALSE(registry.contains("container"));
    EXPECT_FALSE(registry.contains("item"));
}

TEST(WorkRegistryTest, RejectsRemovalOutsideIntakeWithoutChangingTree)
{
    WorkRegistry registry;
    Container* container = static_cast<Container*>(
        registry.add(std::unique_ptr<Component>(new Container("container", "group", "Container"))));
    Item* item = static_cast<Item*>(
        registry.add(std::unique_ptr<Component>(new Item("item", "work", "Item"))));
    container->add(item);
    container->advancePhase();

    EXPECT_THROW(registry.remove("item"), InvalidTransition);
    EXPECT_TRUE(registry.contains("item"));
    ASSERT_EQ(container->getChildren().size(), 1u);
    EXPECT_EQ(container->getChildren().front(), item);
    EXPECT_EQ(item->getParent(), container);
}

TEST(WorkRegistryTest, RejectsRemovingContainerOutsideIntake)
{
    WorkRegistry registry;
    Container* container = static_cast<Container*>(
        registry.add(std::unique_ptr<Component>(new Container("container", "group", "Container"))));
    Item* item = static_cast<Item*>(
        registry.add(std::unique_ptr<Component>(new Item("item", "work", "Item"))));
    container->add(item);
    container->advancePhase();

    EXPECT_THROW(registry.remove("container"), InvalidTransition);
    EXPECT_TRUE(registry.contains("container"));
    EXPECT_TRUE(registry.contains("item"));
}
#include "Participant.h"

namespace {
class ContainerProgressionTest : public ::testing::Test {
protected:
    Participant worker{"worker", "Worker", "Worker"};
    Participant reviewer{"reviewer", "Reviewer", "Reviewer"};
    Item first{"first", "work", "First"};
    Item second{"second", "work", "Second"};
    Item extra{"extra", "work", "Extra"};
    Container container{"batch", "group", "Batch"};

    void SetUp() override {
        container.add(&first);
        container.add(&second);
    }
    void complete(Item& item) {
        item.assign(&worker);
        item.start();
        item.complete(75);
    }
    void enterReview() {
        container.advancePhase();
        complete(first);
        complete(second);
        container.advancePhase();
    }
    void expectMembershipLocked() {
        const auto children = container.getChildren();
        EXPECT_THROW(container.add(&extra), InvalidTransition);
        EXPECT_THROW(container.remove(first.getId()), InvalidTransition);
        EXPECT_EQ(container.getChildren(), children);
        EXPECT_EQ(first.getParent(), &container);
        EXPECT_EQ(extra.getParent(), nullptr);
    }
};
}

TEST(ContainerProgression, EmptyContainerCannotActivate) {
    Container container("empty", "group", "Empty");
    EXPECT_FALSE(container.isCompleted());
    EXPECT_FALSE(container.isReadyToPublish());
    EXPECT_THROW(container.advancePhase(), InvalidTransition);
    EXPECT_EQ(container.getPhaseName(), "Intake");
}

TEST_F(ContainerProgressionTest, IntakeAllowsDetachAndReattach) {
    container.remove(first.getId());
    EXPECT_EQ(first.getParent(), nullptr);
    ASSERT_EQ(container.getChildren().size(), 1u);
    container.add(&first);
    EXPECT_EQ(first.getParent(), &container);
    EXPECT_EQ(container.getChildren().size(), 2u);
    container.advancePhase();
    EXPECT_EQ(container.getPhaseName(), "Active");
}

TEST_F(ContainerProgressionTest, RemovingLastItemRestoresEmptyActivationGate) {
    container.remove(first.getId());
    container.remove(second.getId());
    EXPECT_THROW(container.advancePhase(), InvalidTransition);
    EXPECT_EQ(container.getPhaseName(), "Intake");
    container.add(&first);
    EXPECT_NO_THROW(container.advancePhase());
    EXPECT_EQ(container.getPhaseName(), "Active");
}

TEST_F(ContainerProgressionTest, EveryItemMustFinishBeforeReview) {
    container.advancePhase();
    EXPECT_THROW(container.advancePhase(), InvalidTransition);
    complete(first);
    second.assign(&worker);
    second.start();
    EXPECT_FALSE(container.isCompleted());
    EXPECT_THROW(container.advancePhase(), InvalidTransition);
    EXPECT_EQ(container.getPhaseName(), "Active");
    EXPECT_EQ(first.getStateName(), "Completed");
    EXPECT_EQ(second.getStateName(), "InProgress");
    second.complete(80);
    EXPECT_TRUE(container.isCompleted());
    EXPECT_NO_THROW(container.advancePhase());
    EXPECT_EQ(container.getPhaseName(), "Review");
}

TEST_F(ContainerProgressionTest, ActiveLocksMembership) {
    container.advancePhase();
    expectMembershipLocked();
}

TEST_F(ContainerProgressionTest, CompletionAloneDoesNotPermitPublication) {
    enterReview();
    EXPECT_TRUE(container.isCompleted());
    EXPECT_FALSE(container.isReadyToPublish());
    EXPECT_THROW(container.advancePhase(), InvalidTransition);
    first.recordReview(&reviewer, "Batch");
    EXPECT_FALSE(container.isReadyToPublish());
    EXPECT_THROW(container.advancePhase(), InvalidTransition);
    EXPECT_EQ(container.getPhaseName(), "Review");
    EXPECT_TRUE(first.isReviewed());
    EXPECT_FALSE(second.isReviewed());
    second.recordReview(&reviewer, "Batch");
    EXPECT_TRUE(container.isReadyToPublish());
    container.advancePhase();
    EXPECT_EQ(container.getPhaseName(), "Publish");
}

TEST_F(ContainerProgressionTest, ReviewLocksMembership) {
    enterReview();
    expectMembershipLocked();
}

TEST_F(ContainerProgressionTest, RejectedItemBlocksPublicationUntilPersonallyResolved) {
    enterReview();
    first.recordReview(&reviewer, "Batch");
    second.reject("Needs correction");
    EXPECT_FALSE(container.isReadyToPublish());
    EXPECT_THROW(container.advancePhase(), InvalidTransition);
    EXPECT_EQ(container.getPhaseName(), "Review");
    EXPECT_EQ(second.getStateName(), "Rejected");
    second.resolveReview(&reviewer, 82);
    EXPECT_TRUE(container.isReadyToPublish());
    container.advancePhase();
    EXPECT_EQ(container.getPhaseName(), "Publish");
    EXPECT_EQ(second.getResult(), 82);
    EXPECT_TRUE(first.isReviewed());
}

TEST_F(ContainerProgressionTest, PublishIsFinalAndMembershipRemainsLocked) {
    enterReview();
    first.recordReview(&reviewer, "Batch");
    second.recordReview(&reviewer, "Batch");
    container.advancePhase();
    EXPECT_NO_THROW(container.advancePhase());
    EXPECT_NO_THROW(container.advancePhase());
    EXPECT_EQ(container.getPhaseName(), "Publish");
    EXPECT_TRUE(container.isReadyToPublish());
    expectMembershipLocked();
    EXPECT_EQ(first.getResult(), 75);
    EXPECT_EQ(second.getResult(), 75);
}
