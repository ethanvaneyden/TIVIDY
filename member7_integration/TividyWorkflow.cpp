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
    educationFacade = std::unique_ptr<ScenarioFacade>(new EducationFacade(participants.get()));
    healthcareFacade = std::unique_ptr<ScenarioFacade>(new HospitalFacade(participants.get()));

    // 3. Register the facades with the instance index (initially empty).
    // This is a placeholder; actual registration happens when instances are created.
}

TividyWorkflow::~TividyWorkflow() = default;

ScenarioFacade* TividyWorkflow::findFacade(const std::string& instanceId) const
{
    const auto it = instanceIndex.find(instanceId);
    lastFacade = it != instanceIndex.end() ? it->second : nullptr;
    if(lastFacade == nullptr)
    {
        missingInstance = instanceId;
    }
    return lastFacade;
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
    const CommandResult result = facade->submit(instanceId, containerName);
    return result;
}

CommandResult TividyWorkflow::startItem(const std::string& instanceId,
                                        const std::string& itemId)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    const CommandResult result = facade->startItem(instanceId, itemId);
    return result;
}

CommandResult TividyWorkflow::completeItem(const std::string& instanceId,
                                            const std::string& itemId, int value)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    const CommandResult result = facade->completeItem(instanceId, itemId, value);
    return result;
}

CommandResult TividyWorkflow::approveContainer(const std::string& instanceId,
                                                const std::string& containerId)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    const CommandResult result = facade->approveContainer(instanceId, containerId);
    return result;
}

CommandResult TividyWorkflow::overrideItem(const std::string& instanceId,
                                            const std::string& itemId, int value, bool accept)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    const CommandResult result = facade->overrideItem(instanceId, itemId, value, accept);
    return result;
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

CommandResult TividyWorkflow::setStrategy(const std::string& instanceId,
                                          const std::string& strategyName)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if (facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    const CommandResult result = facade->setStrategy(instanceId, strategyName);
    return result;
}

std::string TividyWorkflow::getStrategyName(const std::string& instanceId) const
{
    ScenarioFacade* facade = findFacade(instanceId);
    return facade != nullptr ? facade->getStrategyName(instanceId) : "";
}

CommandResult TividyWorkflow::addContainer(const std::string& instanceId, const std::string& name)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    const CommandResult result = facade->addContainer(instanceId, name);
    return result;
}

CommandResult TividyWorkflow::addItem(const std::string& instanceId, const std::string& containerId,
                                      const std::string& name,
                                      const std::map<std::string, std::string>& attributes)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    const CommandResult result = facade->addItem(instanceId, containerId, name, attributes);
    return result;
}

CommandResult TividyWorkflow::openContainer(const std::string& instanceId, const std::string& containerId)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    const CommandResult result = facade->openContainer(instanceId, containerId);
    return result;
}

CommandResult TividyWorkflow::removeInstance(const std::string& instanceId)
{
    ScenarioFacade* facade = findFacade(instanceId);
    if(facade == nullptr)
    {
        return CommandResult::NotFound;
    }
    const CommandResult result = facade->removeInstance(instanceId);
    if(result == CommandResult::Success)
    {
        instanceIndex.erase(instanceId);
    }
    return result;
}

std::string TividyWorkflow::lastMessage() const
{
    if(lastFacade == nullptr)
    {
        return missingInstance.empty() ? std::string() : "Instance not found: " + missingInstance;
    }
    return lastFacade->lastMessage();
}

bool TividyWorkflow::hasScenario(const std::string& scenario) const
{
    return scenario == "education" || scenario == "healthcare";
}

bool TividyWorkflow::ownsInstance(const std::string& scenario, const std::string& instanceId) const
{
    const ScenarioFacade* facade = findFacade(instanceId);
    return facade != nullptr && facade->scenarioName() == scenario;
}
