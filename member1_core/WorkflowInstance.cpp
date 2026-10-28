/**
 * @file WorkflowInstance.cpp
 * @brief Shared workflow instance implementation.
*/

#include "WorkflowInstance.h"
#include "WorkRegistry.h"
#include "WorkflowMediator.h"  
#include "AssignmentStrategy.h"
#include "WorkflowBuilder.h"   
#include "Container.h"
#include "ParticipantRegistry.h"
#include "Root.h"
#include <utility>
#include <stdexcept>

WorkflowInstance::WorkflowInstance(
        std::string instanceId,
        std::string definitionType,
        std::unique_ptr<WorkRegistry> workRegistry,
        Root *root,
        std::shared_ptr<WorkflowMediator> coordinator,
        std::unique_ptr<AssignmentStrategy> strategy,
        std::unique_ptr<WorkflowBuilder> builder,
        ParticipantRegistry *participants)
    : instanceId(std::move(instanceId)),
      definitionType(std::move(definitionType)),
      workRegistry(std::move(workRegistry)),
      coordinator(std::move(coordinator)),
      strategy(std::move(strategy)),
      builder(std::move(builder)),
      participants(participants),
      root(root)
{
    if (this->root != nullptr && this->coordinator)
        this->root->setMediator(this->coordinator.get());
}

WorkflowInstance::~WorkflowInstance() = default;

const std::string &WorkflowInstance::getInstanceId() const
{
    return instanceId;
}

const std::string &WorkflowInstance::getDefinitionType() const
{
    return definitionType;
}

WorkflowMediator* WorkflowInstance::getCoordinator() const
{
    return coordinator.get();
}

Root *WorkflowInstance::getRoot() const
{
    return root;
}

std::vector<std::string> WorkflowInstance::getHistory() const
{
    return {};
}

void WorkflowInstance::addItem(const std::string &itemId,
                               const std::string &name,
                               const std::string &subjectId)
{
    (void)itemId;
    (void)name;
    (void)subjectId;
}

void WorkflowInstance::addContainer(const std::string &containerId,
                                    const std::string &name)
{
    (void)containerId;
    (void)name;
}

void WorkflowInstance::completeItem(const std::string &itemId, int value)
{
    (void)itemId;
    (void)value;
}

void WorkflowInstance::approveContainer(const std::string &containerId)
{
    (void)containerId;
}

bool WorkflowInstance::resolveRejected(Item &, bool, int, Participant *)
{
    throw std::logic_error("This workflow does not support rejected item resolution");
}

void WorkflowInstance::rejectContainer(const std::string &containerId,
                                       const std::string &reason)
{
    (void)containerId;
    (void)reason;
}