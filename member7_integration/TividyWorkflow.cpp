/**
 * @file TividyWorkflow.cpp
 * @brief Implementation of the TividyWorkflow facade.
*/

#include "TividyWorkflow.h"
#include "Item.h"
#include "Participant.h"
#include "WorkflowInstance.h"
#include "WorkRegistry.h"
#include "WorkflowMediator.h"
#include "AssignmentStrategy.h"
#include "WorkflowBuilder.h"
#include "Container.h"
#include "ParticipantRegistry.h"
#include "EducationWorkflowFactory.h"

#include <stdexcept>

/* All methods below are scaffolds. They compile once Members 1 and 2
 * land their implementations. Each is marked with the dependency it
 * expects.*/

TividyWorkflow::TividyWorkflow()
    : participants(std::unique_ptr<ParticipantRegistry>(new ParticipantRegistry()))
{
}

TividyWorkflow::~TividyWorkflow() = default;

WorkflowInstance* TividyWorkflow::findInstance(const std::string& instanceId)
{
    for(auto& inst : educationInstances)
    {
        if(inst->getInstanceId() == instanceId)
        {
            return inst.get();
        }
    }
    for(auto& inst : healthcareInstances)
    {
        if(inst->getInstanceId() == instanceId)
        {
            return inst.get();
        }
    }
    return nullptr;
}

const WorkflowInstance* TividyWorkflow::findInstance(const std::string& instanceId) const
{
    for(const auto& inst : educationInstances)
    {
        if(inst->getInstanceId() == instanceId)
        {
            return inst.get();
        }
    }
    for(const auto& inst : healthcareInstances)
    {
        if(inst->getInstanceId() == instanceId)
        {
            return inst.get();
        }
    }
    return nullptr;
}

std::string TividyWorkflow::startEducation(const std::string& name)
{
    static int counter = 0;
    std::string id = "edu_" + std::to_string(counter++) + "_" + name;
    EducationWorkflowFactory factory;
    auto inst = factory.createInstance(id, nullptr, participants.get());
    educationInstances.push_back(std::move(inst));
    return id;
}

std::string TividyWorkflow::startHealthcare(const std::string& name)
{
    static int counter = 0;
    std::string id = "hc_" + std::to_string(counter++) + "_" + name;
    auto inst = std::unique_ptr<WorkflowInstance>(
        new WorkflowInstance(
            id, "healthcare",
            nullptr, nullptr, nullptr, nullptr, nullptr,
            participants.get()));
    healthcareInstances.push_back(std::move(inst));
    return id;
}

std::vector<std::string> TividyWorkflow::listEducationInstances() const
{
    std::vector<std::string> ids;
    for(const auto& inst : educationInstances)
    {
        ids.push_back(inst->getInstanceId());
    }
    return ids;
}

std::vector<std::string> TividyWorkflow::listHealthcareInstances() const
{
    std::vector<std::string> ids;
    for(const auto& inst : healthcareInstances)
    {
        ids.push_back(inst->getInstanceId());
    }
    return ids;
}

Root* TividyWorkflow::getTree(const std::string& instanceId) const
{
    const auto* inst = findInstance(instanceId);
    if(inst == nullptr)
    {
        return nullptr;
    }
    return inst->getRoot();
}

std::vector<std::string> TividyWorkflow::getHistory(const std::string& instanceId) const
{
    const auto* inst = findInstance(instanceId);
    if(inst == nullptr)
    {
        return {};
    }
    return inst->getHistory();
}

CommandResult TividyWorkflow::submitEducation(const std::string& instanceId,
                                              const std::string& containerName)
{
    auto* inst = findInstance(instanceId);
    if(inst == nullptr)
    {
        return CommandResult::NotFound;
    }
    /* [TBD — Member 2] call inst->addContainer(...) with a scenario-specific
     * container ID. Until then, this is a no-op success.*/
    return CommandResult::Success;
}

CommandResult TividyWorkflow::startItem(const std::string& instanceId,
                                        const std::string& itemId)
{
    auto* inst = findInstance(instanceId);
    if(inst == nullptr)
    {
        return CommandResult::NotFound;
    }
    /* [TBD — Member 2] lookup the item by id, call Item::start(), catch
     * InvalidTransition and translate.*/
    return CommandResult::Success;
}

CommandResult TividyWorkflow::completeItem(const std::string& instanceId,
                                           const std::string& itemId,
                                           int value)
{
    auto* inst = findInstance(instanceId);
    if(inst == nullptr)
    {
        return CommandResult::NotFound;
    }
    /* [TBD — Member 2] inst->completeItem(itemId, value);*/
    return CommandResult::Success;
}

CommandResult TividyWorkflow::approveContainer(const std::string& instanceId,
                                               const std::string& containerId)
{
    auto* inst = findInstance(instanceId);
    if(inst == nullptr)
    {
        return CommandResult::NotFound;
    }
    /* [TBD — Member 2 + Member 4] inst->approveContainer(containerId) which
     * runs the chain.*/
    return CommandResult::Success;
}

CommandResult TividyWorkflow::overrideItem(const std::string& instanceId,
                                           const std::string& itemId,
                                           int value, bool accept)
{
    auto* inst = findInstance(instanceId);
    if(inst == nullptr)
    {
        return CommandResult::NotFound;
    }
    /* [TBD — Member 2 + Member 4] resolve a Rejected item.*/
    return CommandResult::Success;
}

CommandResult TividyWorkflow::admitPatient(const std::string& instanceId,
                                           const std::string& containerName)
{
    auto* inst = findInstance(instanceId);
    if(inst == nullptr)
    {
        return CommandResult::NotFound;
    }
    /* [TBD — Member 2] same as submitEducation, for healthcare.*/
    return CommandResult::Success;
}