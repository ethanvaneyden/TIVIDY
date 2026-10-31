/**
 * @file WorkflowInstance.cpp
 * @brief Implementation of the WorkflowInstance class.
 * @author Member 1 (Core Infrastructure)
 * @date October 2026
 */

#include "WorkflowInstance.h"
#include "WorkRegistry.h"
#include "Root.h"
#include "Container.h"
#include "Item.h"
#include "Component.h"
#include "Iterator.h"
#include "WorkflowMediator.h"
#include "WorkflowBuilder.h"
#include "AssignmentStrategy.h"
#include "ParticipantRegistry.h"
#include <memory>
#include <stdexcept>
#include <utility>

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
    if (!this->root) throw std::invalid_argument("Root component cannot be null");
    if (!this->registry) throw std::invalid_argument("Registry cannot be null");
    if (!this->coordinator) throw std::invalid_argument("Coordinator cannot be null");
    if (!this->strategy) throw std::invalid_argument("Assignment strategy cannot be null");
    if (!this->builder) throw std::invalid_argument("Workflow builder cannot be null");
    if (!this->participants) throw std::invalid_argument("Participant registry cannot be null");
    if (this->registry->get(this->root->getId()) != this->root)
        throw std::invalid_argument("Root must be owned by the WorkRegistry");

    this->root->setMediator(this->coordinator.get());
}

WorkflowInstance::~WorkflowInstance() = default;

const std::string& WorkflowInstance::getInstanceId() const { 
    return instanceId; 
}

const std::string& WorkflowInstance::getDefinitionType() const { 
    return definitionType; 
}

Root* WorkflowInstance::getRoot() const { 
    return root; 
}

std::vector<std::string> WorkflowInstance::getHistory() const {
    return coordinator->getHistoryAsStrings();
}

std::vector<Item*> WorkflowInstance::getItems() const
{
    std::vector<Item*> items;
    std::unique_ptr<Iterator> iterator(root->createIterator());
    for (iterator->first(); !iterator->isDone(); iterator->next())
    {
        Item* item = dynamic_cast<Item*>(iterator->current());
        if (item != nullptr)
            items.push_back(item);
    }
    return items;
}

std::vector<Item*> WorkflowInstance::getItemsByState(const std::string& state) const
{
    std::vector<Item*> items;
    std::unique_ptr<Iterator> iterator(root->createIterator(state));
    for (iterator->first(); !iterator->isDone(); iterator->next())
    {
        Item* item = dynamic_cast<Item*>(iterator->current());
        if (item != nullptr)
            items.push_back(item);
    }
    return items;
}

Container* WorkflowInstance::addContainer(
    const std::string& containerId,
    const std::string& name)
{
    if (!builder) throw std::runtime_error("No builder configured");
    builder->buildContainer(containerId, name);
    Container* container = getContainer(containerId);
    if (!container)
        throw std::logic_error("WorkflowBuilder did not register the created container");
    container->setMediator(coordinator.get());
    if (container->getParent() == nullptr)
    {
        try
        {
            root->add(container);
        }
        catch (...)
        {
            registry->remove(containerId);
            throw;
        }
    }
    else if (container->getParent() != root)
    {
        registry->remove(containerId);
        throw std::logic_error("WorkflowBuilder attached the container to the wrong root");
    }
    return container;
}

Item* WorkflowInstance::getItem(const std::string& itemId) const {
    return dynamic_cast<Item*>(registry->get(itemId));
}

Container* WorkflowInstance::getContainer(const std::string& containerId) const {
    return dynamic_cast<Container*>(registry->get(containerId));
}

void WorkflowInstance::addItem(
    const std::string& parentContainerId,
    const std::string& itemId,
    const std::string& name,
    const std::string& subjectId)
{
    if (!builder) throw std::runtime_error("No builder configured for dynamic items");

    Component* c = registry->get(parentContainerId);
    Container* parent = dynamic_cast<Container*>(c);
    if (!parent) throw std::runtime_error("Parent container not found: " + parentContainerId);

    builder->buildItem(itemId, name);
    Item* item = getItem(itemId);
    if (!item)
        throw std::logic_error("WorkflowBuilder did not register the created item");
    item->setMediator(coordinator.get());
    if (item->getParent() == nullptr) {
        try
        {
            parent->add(item);
        }
        catch (...)
        {
            registry->remove(itemId);
            throw;
        }
    }
    else if (item->getParent() != parent)
    {
        registry->remove(itemId);
        throw std::logic_error("WorkflowBuilder attached the item to the wrong container");
    }
    coordinator->onItemAdded(item);
}

void WorkflowInstance::startItem(const std::string& itemId) {
    Component* c = registry->get(itemId);
    Item* item = dynamic_cast<Item*>(c);
    if (!item) throw std::runtime_error("Item not found: " + itemId);

    // State machine handles validity (throws if not Assigned) and notifies Mediator
    item->start();
}

void WorkflowInstance::completeItem(const std::string& itemId, int value) {
    Component* c = registry->get(itemId);
    Item* item = dynamic_cast<Item*>(c);
    if (!item) throw std::runtime_error("Item not found: " + itemId);

    // State machine handles validity and notifies Mediator
    item->complete(value);
}

void WorkflowInstance::rejectItem(const std::string& itemId, const std::string& reason) {
    Component* c = registry->get(itemId);
    Item* item = dynamic_cast<Item*>(c);
    if (!item) throw std::runtime_error("Item not found: " + itemId);

    item->reject(reason);
}

void WorkflowInstance::overrideItem(
    const std::string& itemId,
    Participant* operatorParticipant,
    int newValue,
    const std::string& reason)
{
    Component* c = registry->get(itemId);
    Item* item = dynamic_cast<Item*>(c);
    if (!item) throw std::runtime_error("Item not found: " + itemId);

    // The Operator (Lecturer/Chief) overrides a Rejected item
    item->resolveReview(operatorParticipant, newValue, reason);
}

void WorkflowInstance::approveContainer(const std::string& containerId, Participant* operatorParticipant) {
    Component* c = registry->get(containerId);
    Container* container = dynamic_cast<Container*>(c);
    if (!container) throw std::runtime_error("Container not found: " + containerId);

    // Delegate the batch approval to the Mediator (runs the Chain of Responsibility)
    coordinator->onContainerApprovalRequested(container, operatorParticipant);
}

void WorkflowInstance::rejectContainer(const std::string& containerId, const std::string& reason) {
    Component* c = registry->get(containerId);
    Container* container = dynamic_cast<Container*>(c);
    if (!container) throw std::runtime_error("Container not found: " + containerId);

    // Delegate to Mediator to handle the rejection and rework logic
    coordinator->onContainerRejected(container, reason);
}