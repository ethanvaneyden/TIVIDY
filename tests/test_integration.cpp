#include <gtest/gtest.h>

#include "TividyWorkflow.h"
#include "JsonSerializer.h"
#include "EducationWorkflowFactory.h"
#include "EducationPolicies.h"
#include "ParticipantRegistry.h"
#include "Container.h"
#include "Root.h"
#include "Item.h"
#include "AssignmentStrategy.h"
#include "WorkflowBuilder.h"
#include "WorkflowInstance.h"

#include <algorithm>

// ---------------------------------------------------------------------------
// Facade smoke tests
// ---------------------------------------------------------------------------

TEST(TividyWorkflowTest, StartEducationReturnsNonEmptyId)
{
    TividyWorkflow wf;
    auto id = wf.startEducation("COS 214 Assignment 1");
    EXPECT_FALSE(id.empty());
}

TEST(TividyWorkflowTest, StartHealthcareReturnsNonEmptyId)
{
    TividyWorkflow wf;
    auto id = wf.startHealthcare("Admission 42");
    EXPECT_FALSE(id.empty());
}

TEST(TividyWorkflowTest, ListInstancesContainsStartedOnes)
{
    TividyWorkflow wf;
    const std::string first = wf.startEducation("E1");
    const std::string second = wf.startEducation("E2");
    auto ids = wf.listEducationInstances();
    ASSERT_EQ(ids.size(), 2u);
    EXPECT_NE(std::find(ids.begin(), ids.end(), first), ids.end());
    EXPECT_NE(std::find(ids.begin(), ids.end(), second), ids.end());
}

TEST(TividyWorkflowTest, OperationsOnMissingInstanceReturnNotFound)
{
    TividyWorkflow wf;
    EXPECT_EQ(wf.startItem("does-not-exist", "i1"), CommandResult::NotFound);
    EXPECT_EQ(wf.completeItem("does-not-exist", "i1", 10), CommandResult::NotFound);
}

// ---------------------------------------------------------------------------
// JSON serializer tests
// ---------------------------------------------------------------------------

TEST(JsonSerializerTest, GetStringExtractsField)
{
    std::string body = R"({"name":"Alice","age":30})";
    EXPECT_EQ(JsonSerializer::getString(body, "name", ""), "Alice");
    EXPECT_EQ(JsonSerializer::getString(body, "missing", "fallback"), "fallback");
}

TEST(JsonSerializerTest, GetIntExtractsField)
{
    std::string body = R"({"value":75})";
    EXPECT_EQ(JsonSerializer::getInt(body, "value", 0), 75);
    EXPECT_EQ(JsonSerializer::getInt(body, "missing", 42), 42);
}

TEST(JsonSerializerTest, SerializeHistoryProducesValidArray)
{
    std::vector<std::string> records = {"started", "marked Q1"};
    auto json = JsonSerializer::serializeHistory(records);
    EXPECT_EQ(json, "[\"started\",\"marked Q1\"]");
}

TEST(JsonSerializerTest, SerializeErrorProducesValidObject)
{
    auto json = JsonSerializer::serializeError("NotFound", "instance");
    EXPECT_EQ(json, "{\"error\":\"NotFound\",\"message\":\"instance\"}");
}

// ---------------------------------------------------------------------------
// End-to-end integration tests
// ---------------------------------------------------------------------------

namespace
{
    /* Walk the container for the item with the given id. */
    Item* findItem(Root* root, const std::string& itemId)
    {
        Container* container = dynamic_cast<Container*>(root->getChildren().front());
        if (container == nullptr) return nullptr;
        const std::vector<Component*> children = container->getChildren();
        for (unsigned i = 0; i < children.size(); ++i)
        {
            if (children[i]->getId() == itemId)
            {
                return dynamic_cast<Item*>(children[i]);
            }
        }
        return nullptr;
    }

    /* Complete every item in the container. Container then advances to Review. */
    bool completeAllItems(TividyWorkflow& workflow,
                          const std::string& instanceId,
                          Root* root)
    {
        Container* container = dynamic_cast<Container*>(root->getChildren().front());
        if (container == nullptr) return false;
        const std::vector<Component*> children = container->getChildren();
        for (unsigned i = 0; i < children.size(); ++i)
        {
            const std::string id = children[i]->getId();
            if (workflow.startItem(instanceId, id) != CommandResult::Success)
            {
                return false;
            }
            /* Use a mark in the TA band (50-70) so approval succeeds. */
            if (workflow.completeItem(instanceId, id, 60) != CommandResult::Success)
            {
                return false;
            }
        }
        return true;
    }
}

TEST(IntegrationTest, EducationHappyPath_CompleteAndApprove)
{
    TividyWorkflow workflow;
    const std::string instanceId = workflow.startEducation("Happy Path Test");
    ASSERT_FALSE(instanceId.empty());

    Root* root = workflow.getTree(instanceId);
    ASSERT_NE(root, nullptr);
    ASSERT_FALSE(root->getChildren().empty());
    Container* container = dynamic_cast<Container*>(root->getChildren()[0]);
    ASSERT_NE(container, nullptr);

    /* Complete every item so the container advances to Review. */
    ASSERT_TRUE(completeAllItems(workflow, instanceId, root));
    EXPECT_EQ(container->getPhaseName(), "Review");

    /* Approve the container: chain runs per-item, all marks are in the TA
     * band, so every item should become reviewed. */
    ASSERT_EQ(CommandResult::Success,
              workflow.approveContainer(instanceId, container->getId()));

    /* Every item in the container should be Completed and reviewed. */
    const std::vector<Component*> children = container->getChildren();
    for (unsigned i = 0; i < children.size(); ++i)
    {
        Item* item = dynamic_cast<Item*>(children[i]);
        ASSERT_NE(item, nullptr);
        EXPECT_EQ(item->getStateName(), "Completed");
        EXPECT_TRUE(item->isReviewed());
        ASSERT_NE(item->getReviewer(), nullptr);
    }
}

TEST(IntegrationTest, EducationRejectedPath_OverrideCompletes)
{
    TividyWorkflow workflow;
    const std::string instanceId = workflow.startEducation("Rejected Path Test");
    ASSERT_FALSE(instanceId.empty());

    Root* root = workflow.getTree(instanceId);
    ASSERT_NE(root, nullptr);
    Container* container = dynamic_cast<Container*>(root->getChildren()[0]);
    ASSERT_NE(container, nullptr);

    /* Find the first item, start and complete it with a failing mark. */
    const std::string itemId = container->getChildren().front()->getId();
    ASSERT_EQ(CommandResult::Success, workflow.startItem(instanceId, itemId));
    ASSERT_EQ(CommandResult::Success, workflow.completeItem(instanceId, itemId, 30));

    /* Complete all other items with passing marks so the container
     * advances to Review. */
    const std::vector<Component*> children = container->getChildren();
    for (unsigned i = 1; i < children.size(); ++i)
    {
        const std::string id = children[i]->getId();
        ASSERT_EQ(CommandResult::Success, workflow.startItem(instanceId, id));
        ASSERT_EQ(CommandResult::Success, workflow.completeItem(instanceId, id, 60));
    }
    EXPECT_EQ(container->getPhaseName(), "Review");

    /* Run the batch approval. The 30-mark item falls off the chain and
     * becomes Rejected; the 60-mark items are approved. */
    ASSERT_EQ(CommandResult::Success,
              workflow.approveContainer(instanceId, container->getId()));

    Item* rejected = findItem(root, itemId);
    ASSERT_NE(rejected, nullptr);
    ASSERT_EQ(rejected->getStateName(), "Rejected");
    EXPECT_FALSE(rejected->isReviewed());

    /* Lecturer overrides the rejection. This resolves the item and
     * records the reviewer. */
    ASSERT_EQ(CommandResult::Success,
              workflow.overrideItem(instanceId, itemId, 30, true));

    rejected = findItem(root, itemId);
    ASSERT_NE(rejected, nullptr);
    EXPECT_EQ(rejected->getStateName(), "Completed");
    EXPECT_TRUE(rejected->isReviewed());

    /* Container should now be ready to publish. */
    EXPECT_EQ(container->getPhaseName(), "Publish");
}

TEST(IntegrationTest, TwoInstancesAreIsolated)
{
    TividyWorkflow workflow;
    const std::string id1 = workflow.startEducation("Isolation A");
    const std::string id2 = workflow.startEducation("Isolation B");
    ASSERT_FALSE(id1.empty());
    ASSERT_FALSE(id2.empty());
    ASSERT_NE(id1, id2);

    Root* root1 = workflow.getTree(id1);
    Root* root2 = workflow.getTree(id2);
    ASSERT_NE(root1, nullptr);
    ASSERT_NE(root2, nullptr);

    Container* c1 = dynamic_cast<Container*>(root1->getChildren()[0]);
    Container* c2 = dynamic_cast<Container*>(root2->getChildren()[0]);
    ASSERT_NE(c1, nullptr);
    ASSERT_NE(c2, nullptr);

    /* Both start with the same demo director content; verify items are
     * distinct objects. */
    Item* item1 = dynamic_cast<Item*>(c1->getChildren()[0]);
    Item* item2 = dynamic_cast<Item*>(c2->getChildren()[0]);
    ASSERT_NE(item1, nullptr);
    ASSERT_NE(item2, nullptr);
    ASSERT_NE(item1, item2);

    /* Mutating item1 must not affect item2. */
    const std::string originalState2 = item2->getStateName();
    ASSERT_EQ(CommandResult::Success,
              workflow.startItem(id1, item1->getId()));
    EXPECT_EQ(item1->getStateName(), "InProgress");
    EXPECT_EQ(item2->getStateName(), originalState2);
}

TEST(IntegrationTest, JsonSerializerProducesValidJson)
{
    TividyWorkflow workflow;
    const std::string instanceId = workflow.startEducation("JSON Test");
    Root* root = workflow.getTree(instanceId);
    ASSERT_NE(root, nullptr);

    const std::string json = JsonSerializer::serializeTree(root);
    ASSERT_FALSE(json.empty());

    const std::string rootId = JsonSerializer::getString(json, "id", "");
    ASSERT_FALSE(rootId.empty());
}