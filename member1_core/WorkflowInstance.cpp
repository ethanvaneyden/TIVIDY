#include "WorkflowInstance.h"

#include "AssignmentStrategy.h"
#include "Component.h"
#include "Container.h"
#include "InvalidTransition.h"
#include "Item.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Root.h"
#include "WorkflowBuilder.h"
#include "WorkflowMediator.h"
#include "WorkRegistry.h"

#include <stdexcept>
#include <utility>

namespace
{

Component* findInTree(Component* node, const std::string& id)
{
    if (node == nullptr)
        return nullptr;
    if (node->getId() == id)
        return node;

    const std::vector<Component*> children = node->getChildren();
    for (std::vector<Component*>::const_iterator child = children.begin();
         child != children.end(); ++child)
    {
        Component* found = findInTree(*child, id);
        if (found != nullptr)
            return found;
    }
    return nullptr;
}

void validateRegisteredTree(const Component* node, const WorkRegistry& registry)
{
    if (node == nullptr || registry.get(node->getId()) != node)
        throw std::invalid_argument(
            "The work registry must own every node in the workflow tree");

    const std::vector<Component*> children = node->getChildren();
    for (std::vector<Component*>::const_iterator child = children.begin();
         child != children.end(); ++child)
    {
        validateRegisteredTree(*child, registry);
    }
}

}

WorkflowInstance::WorkflowInstance(
    std::string instanceId,
    std::string definitionType,
    std::unique_ptr<WorkRegistry> registry,
    Root* root,
    std::unique_ptr<WorkflowMediator> coordinator,
    std::unique_ptr<AssignmentStrategy> strategy,
    std::unique_ptr<WorkflowBuilder> builder,
    ParticipantRegistry* participants)
    : instanceId(std::move(instanceId)),
      definitionType(std::move(definitionType)),
      registry(std::move(registry)),
      coordinator(std::move(coordinator)),
      strategy(std::move(strategy)),
      builder(std::move(builder)),
      participants(participants),
      root(root)
{
    if (this->instanceId.empty())
        throw std::invalid_argument("Workflow instance id cannot be empty");
    if (this->definitionType.empty())
        throw std::invalid_argument("Workflow definition type cannot be empty");
    if (!this->registry)
        throw std::invalid_argument("Workflow instance requires a work registry");
    if (this->root == nullptr)
        throw std::invalid_argument("Workflow instance requires a root");
    validateRegisteredTree(this->root, *this->registry);
    if (!this->coordinator)
        throw std::invalid_argument("Workflow instance requires a mediator");

    this->root->setMediator(this->coordinator.get());
}

WorkflowInstance::~WorkflowInstance() = default;

const std::string& WorkflowInstance::getInstanceId() const
{
    return instanceId;
}

const std::string& WorkflowInstance::getDefinitionType() const
{
    return definitionType;
}

Root* WorkflowInstance::getRoot() const
{
    return root;
}

std::vector<std::string> WorkflowInstance::getHistory() const
{
    return history;
}

void WorkflowInstance::addItem(
    const std::string& parentContainerId,
    const std::string& itemId,
    const std::string& name,
    const std::string& subjectId)
{
    if (parentContainerId.empty() || itemId.empty() || name.empty())
        throw std::invalid_argument("Parent id, item id, and item name are required");
    if (findInTree(root, itemId) != nullptr || registry->get(itemId) != nullptr)
        throw std::invalid_argument("Duplicate workflow component id: " + itemId);

    Component* parentComponent = findInTree(root, parentContainerId);
    Container* parent = dynamic_cast<Container*>(parentComponent);
    if (parent == nullptr)
        throw std::out_of_range("Parent container not found: " + parentContainerId);

    std::unique_ptr<Item> ownedItem(
        new Item(itemId, "item", name, nullptr, subjectId));
    Item* item = ownedItem.get();
    registry->add(std::unique_ptr<Component>(ownedItem.release()));

    try
    {
        parent->add(item);
    }
    catch (...)
    {
        registry->remove(itemId);
        throw;
    }

    item->setMediator(coordinator.get());
    history.push_back("Added item " + itemId + " to container " + parentContainerId);
}

void WorkflowInstance::startItem(const std::string& itemId)
{
    Component* component = findInTree(root, itemId);
    Item* item = dynamic_cast<Item*>(component);
    if (item == nullptr)
        throw std::out_of_range("Item not found: " + itemId);

    Container* parent = dynamic_cast<Container*>(item->getParent());
    if (parent == nullptr)
        throw std::logic_error("Item is not attached to a workflow container");

    if (parent->getPhaseName() == "Intake")
    {
        parent->advancePhase();
        history.push_back("Activated container " + parent->getId());
    }

    if (item->getAssignee() == nullptr)
    {
        if (!strategy || participants == nullptr)
            throw std::logic_error("Item assignment is not configured");

        std::vector<Participant*> pool;
        const std::vector<std::string> ids = participants->getAllIds();
        for (std::vector<std::string>::const_iterator id = ids.begin();
             id != ids.end(); ++id)
        {
            Participant* participant = participants->get(*id);
            if (participant != nullptr)
                pool.push_back(participant);
        }

        Participant* assignee = strategy->assign(pool);
        if (assignee == nullptr)
        {
            coordinator->onAssignmentFailed(item, "No worker is available");
            throw std::logic_error("Assignment strategy found no eligible worker");
        }
        item->assign(assignee);
        history.push_back("Assigned item " + itemId + " to " + assignee->getId());
    }

    item->start();
    history.push_back("Started item " + itemId);
}

void WorkflowInstance::completeItem(const std::string& itemId, int value)
{
    Component* component = findInTree(root, itemId);
    Item* item = dynamic_cast<Item*>(component);
    if (item == nullptr)
        throw std::out_of_range("Item not found: " + itemId);

    item->complete(value);
    history.push_back("Completed item " + itemId);
}

void WorkflowInstance::rejectItem(
    const std::string& itemId,
    const std::string& reason)
{
    Component* component = findInTree(root, itemId);
    Item* item = dynamic_cast<Item*>(component);
    if (item == nullptr)
        throw std::out_of_range("Item not found: " + itemId);

    item->reject(reason);
    history.push_back("Rejected item " + itemId + ": " + reason);
}

void WorkflowInstance::overrideItem(
    const std::string& itemId,
    Participant* operatorParticipant,
    int newValue,
    const std::string& reason)
{
    if (reason.empty())
        throw std::invalid_argument("An override reason is required");

    Component* component = findInTree(root, itemId);
    Item* item = dynamic_cast<Item*>(component);
    if (item == nullptr)
        throw std::out_of_range("Item not found: " + itemId);

    item->resolveReview(operatorParticipant, newValue, reason);
    history.push_back("Overrode item " + itemId + ": " + reason);
}

void WorkflowInstance::approveContainer(
    const std::string& containerId,
    Participant* operatorParticipant)
{
    (void)operatorParticipant;
    Component* component = findInTree(root, containerId);
    if (dynamic_cast<Container*>(component) == nullptr)
        throw std::out_of_range("Container not found: " + containerId);

    throw std::logic_error(
        "Batch approval is not available in the WorkflowMediator interface");
}

void WorkflowInstance::rejectContainer(
    const std::string& containerId,
    const std::string& reason)
{
    if (reason.empty())
        throw std::invalid_argument("A rejection reason is required");

    Component* component = findInTree(root, containerId);
    Container* container = dynamic_cast<Container*>(component);
    if (container == nullptr)
        throw std::out_of_range("Container not found: " + containerId);
    if (container->getPhaseName() != "Review")
        throw InvalidTransition("A container can only be rejected during Review");

    const std::vector<Component*> children = container->getChildren();
    if (children.empty())
        throw InvalidTransition("An empty container cannot be rejected");

    std::vector<Item*> items;
    for (std::vector<Component*>::const_iterator child = children.begin();
         child != children.end(); ++child)
    {
        Item* item = dynamic_cast<Item*>(*child);
        if (item == nullptr || item->getStateName() != "Completed")
            throw InvalidTransition("Only completed items can be rejected as a batch");
        items.push_back(item);
    }

    for (std::vector<Item*>::const_iterator item = items.begin();
         item != items.end(); ++item)
    {
        (*item)->reject(reason);
    }
    history.push_back("Rejected container " + containerId + ": " + reason);
}
