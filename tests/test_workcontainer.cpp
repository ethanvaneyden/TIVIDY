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