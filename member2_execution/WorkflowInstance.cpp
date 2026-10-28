/**
 * @file WorkflowInstance.cpp
 * @brief STUB — replaced by Member 2's full implementation.
*/

#include "WorkflowInstance.h"
#include "WorkRegistry.h"
#include "WorkflowMediator.h"  
#include "AssignmentStrategy.h"
#include "WorkflowBuilder.h"   
#include "Container.h"
#include "ParticipantRegistry.h"
#include <utility>

WorkflowInstance::WorkflowInstance(
        std::string instanceId,
        std::string definitionType,
        std::unique_ptr<WorkRegistry> workRegistry,
        Container *root,
        std::unique_ptr<WorkflowMediator> coordinator,
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

Container *WorkflowInstance::getRoot() const
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

void WorkflowInstance::rejectContainer(const std::string &containerId,
                                       const std::string &reason)
{
    (void)containerId;
    (void)reason;
}