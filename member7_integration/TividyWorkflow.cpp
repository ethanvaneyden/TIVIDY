/**
 * @file TividyWorkflow.cpp
 * @brief Implementation of the top-level TIVIDY facade.
 */
#include "TividyWorkflow.h"

#include "EducationFacade.h"
#include "HospitalFacade.h"
#include "ParticipantRegistry.h"

TividyWorkflow::TividyWorkflow()
{
    // 1. Create the shared participant registry.
    participants = std::unique_ptr<ParticipantRegistry>(new ParticipantRegistry());

    // 2. Create the scenario-specific facades.
    educationFacade = std::unique_ptr<ScenarioFacade>(new education::EducationFacade(*participants));
    healthcareFacade = std::unique_ptr<ScenarioFacade>(new hospital::HospitalFacade(*participants));

    // 3. Register the facades with the instance index (initially empty).
    // This is a placeholder; actual registration happens when instances are created.
}

TividyWorkflow::~TividyWorkflow() = default;

ScenarioFacade* TividyWorkflow::findFacade(const std::string& instanceId) const
{
    const auto it = instanceIndex.find(instanceId);
    return it != instanceIndex.end() ? it->second : nullptr;
}

std::string TividyWorkflow::startEducation(const std::string& name)
{
    const std::string id = educationFacade->start(name);
    if(!id.empty())
    {
        instanceIndex[id] = educationFacade.get();
    }
    return id;
}

std::string TividyWorkflow::startHealthcare(const std::string& name)
{
    const std::string id = healthcareFacade->start(name);
    if(!id.empty())
    {
        instanceIndex[id] = healthcareFacade.get();
    }
    return id;
}

std::vector<std::string> TividyWorkflow::listEducationInstances() const
{
    return educationFacade->listInstances();
}

std::vector<std::string> TividyWorkflow::listHealthcareInstances() const
{
    return healthcareFacade->listInstances();
}

Root* TividyWorkflow::getTree(const std::string& instanceId) const
{
    ScenarioFacade* facade = findFacade(instanceId);
    return facade != nullptr ? facade->getTree(instanceId) : nullptr;
}

std::vector<std::string> TividyWorkflow::getHistory(const std::string& instanceId) const
{
    ScenarioFacade* facade = findFacade(instanceId);
    return facade != nullptr ? facade->getHistory(instanceId) : std::vector<std::string>();
}

CommandResult TividyWorkflow::submit(const std::string& instanceId,
                                    const std::string& containerName)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    return facade->submit(instanceId, containerName);
}

CommandResult TividyWorkflow::startItem(const std::string& instanceId,
                                        const std::string& itemId)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    return facade->startItem(instanceId, itemId);
}

CommandResult TividyWorkflow::completeItem(const std::string& instanceId,
                                            const std::string& itemId, int value)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    return facade->completeItem(instanceId, itemId, value);
}

CommandResult TividyWorkflow::approveContainer(const std::string& instanceId,
                                                const std::string& containerId)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    return facade->approveContainer(instanceId, containerId);
}

CommandResult TividyWorkflow::overrideItem(const std::string& instanceId,
                                            const std::string& itemId, int value, bool accept)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    return facade->overrideItem(instanceId, itemId, value, accept);
}

CommandResult TividyWorkflow::admitPatient(const std::string& instanceId,
                                            const std::string& containerName)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    // admitPatient is semantically the same as submit for the hospital facade.
    return facade->submit(instanceId, containerName);
}