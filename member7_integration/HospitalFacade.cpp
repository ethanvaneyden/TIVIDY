/**
 * @file HospitalFacade.cpp
 * @brief Implementation of HospitalFacade.
 */
#include "HospitalFacade.h"

#include "WorkflowInstance.h"
#include "WorkRegistry.h"
#include "Root.h"
#include "Container.h"
#include "Item.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "InvalidTransition.h"

#include "HospitalAdmissionBuilder.h"
#include "HospitalAdmissionDirector.h"
#include "HospitalAdmissionCoordinator.h"
#include "HospitalPolicies.h"
#include "HospitalSeed.h"

#include "WorkflowMediator.h"
#include "AssignmentStrategy.h"
#include "WorkloadStrategy.h"
#include "RoundRobinStrategy.h"
#include "RandomStrategy.h"
#include "WorkflowBuilder.h"

#include "ActionHistory.h"
#include "OverrideItemCommand.h"
#include "WorkCommand.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    Participant* firstWithRole(ParticipantRegistry* registry,
                               const std::string& role)
    {
        if(registry == nullptr)
        {
            return nullptr;
        }
        auto candidates = registry->getByRole(role);
        return candidates.empty() ? nullptr : candidates.front();
    }

    CommandResult classify(std::function<void()> body)
    {
        try
        {
            body();
            return CommandResult::Success;
        }
        catch (const InvalidTransition&)
        {
            return CommandResult::InvalidTransition;
        }
        catch (const std::invalid_argument&)
        {
            return CommandResult::NotFound;
        }
        catch (const std::runtime_error&)
        {
            return CommandResult::InvalidTransition;
        }
    }
}

HospitalFacade::HospitalFacade(ParticipantRegistry* participants)
    : participants(participants)
{
    if(this->participants == nullptr)
    {
        throw std::invalid_argument("HospitalFacade: participant registry required");
    }

    if(this->participants->getByRole(hospital::HospitalRoles::chiefOfMedicine()).empty())
    {
        hospital::HospitalSeed::seedParticipants(*this->participants);
    }
}

HospitalFacade::~HospitalFacade() = default;

std::string HospitalFacade::scenarioName() const
{
    return "healthcare";
}

std::string HospitalFacade::start(const std::string& name)
{
    static int counter = 0;
    std::string instanceId = "hc_" + std::to_string(++counter);

    std::unique_ptr<WorkRegistry> registry(new WorkRegistry());
    hospital::HospitalAdmissionBuilder builder(*registry);

    hospital::HospitalAdmissionDirector director(builder);
    director.buildDemoWard();

    Root* root = builder.getRoot();
    if(root == nullptr)
    {
        throw std::runtime_error("HospitalFacade: builder produced no root");
    }

<<<<<<< HEAD
    hospital::HospitalAdmissionCoordinator* coordinatorPtr =
        new hospital::HospitalAdmissionCoordinator(
            *participants,
            std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy()));
    std::unique_ptr<WorkflowMediator> coordinator(coordinatorPtr);

    std::unique_ptr<AssignmentStrategy> strategy(new WorkloadStrategy());

    std::unique_ptr<WorkflowBuilder> runtimeBuilder(
        new hospital::HospitalAdmissionBuilder(*registry));

=======
    /* 3. Create the hospital mediator.*/
    HospitalWorkflowFactory factory;
    std::unique_ptr<AssignmentStrategy> strategy = factory.createStrategy();
    std::unique_ptr<WorkflowMediator> coordinator(
        new hospital::HospitalAdmissionCoordinator(*participants, std::move(strategy)));

    /* 4. Create a runtime builder (for addContainer/addItem during execution).*/
    std::unique_ptr<WorkflowBuilder> runtimeBuilder = factory.createBuilder();
    runtimeBuilder->setRegistry(*registry);

    /* 5. Assemble the instance.*/
>>>>>>> dev
    std::unique_ptr<WorkflowInstance> instance(new WorkflowInstance(
        instanceId,
        name,
        std::move(registry),
        root,
        std::move(coordinator),
        std::move(runtimeBuilder),
        participants));

    instances.push_back(std::move(instance));
    coordinators[instanceId] = coordinatorPtr;

    return instanceId;
}

std::vector<std::string> HospitalFacade::listInstances() const
{
    std::vector<std::string> ids;
    ids.reserve(instances.size());
    for (const auto& instance : instances)
    {
        ids.push_back(instance->getInstanceId());
    }
    return ids;
}

Root* HospitalFacade::getTree(const std::string& instanceId) const
{
    WorkflowInstance* instance = findInstance(instanceId);
    return instance == nullptr ? nullptr : instance->getRoot();
}

std::vector<std::string> HospitalFacade::getHistory(const std::string& instanceId) const
{
    WorkflowInstance* instance = findInstance(instanceId);
    return instance == nullptr
        ? std::vector<std::string>()
        : instance->getHistory();
}

CommandResult HospitalFacade::submit(const std::string& instanceId,
                                      const std::string& containerName)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if(instance == nullptr)
    {
        return CommandResult::NotFound;
    }

    static int counter = 0;
    std::string containerId = "PAT-" + std::to_string(++counter);

    return classify([&]()
    {
        instance->addContainer(containerId, containerName);
    });
}

CommandResult HospitalFacade::startItem(const std::string& instanceId,
                                        const std::string& itemId)
{
    WorkflowInstance* instance = findInstance(instanceId);
    hospital::HospitalAdmissionCoordinator* coordinator = findCoordinator(instanceId);
    if(instance == nullptr || coordinator == nullptr)
    {
        return CommandResult::NotFound;
    }

    return classify([&]()
    {
        openIfNeeded(instance, coordinator);
        instance->startItem(itemId);
    });
}

CommandResult HospitalFacade::completeItem(const std::string& instanceId,
                                            const std::string& itemId,
                                            int value)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if(instance == nullptr)
    {
        return CommandResult::NotFound;
    }

    if(!hospital::HospitalPolicies::isValidSeverity(value))
    {
        return CommandResult::InvalidTransition;
    }

    return classify([&]()
    {
        instance->completeItem(itemId, value);
    });
}

CommandResult HospitalFacade::approveContainer(const std::string& instanceId,
                                                const std::string& containerId)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if(instance == nullptr)
    {
        return CommandResult::NotFound;
    }

    Participant* op = operatorParticipant();
    if(op == nullptr)
    {
        return CommandResult::NotFound;
    }

    return classify([&]()
    {
        instance->approveContainer(containerId, op);
    });
}

CommandResult HospitalFacade::overrideItem(const std::string& instanceId,
                                            const std::string& itemId,
                                            int value,
                                            bool accept)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if(instance == nullptr)
    {
        return CommandResult::NotFound;
    }

    Participant* op = operatorParticipant();
    if(op == nullptr)
    {
        return CommandResult::NotFound;
    }

    Item* item = instance->getItem(itemId);
    if(item == nullptr)
    {
        return CommandResult::NotFound;
    }
    int effectiveValue = accept ? item->getResult() : value;

    if(!hospital::HospitalPolicies::isValidSeverity(effectiveValue))
    {
        return CommandResult::InvalidTransition;
    }

    const std::string reason = accept ? "Accepted" : "Overridden";

    return classify([&]()
    {
        ActionHistory invoker;
        invoker.execute(std::unique_ptr<WorkCommand>(
            new OverrideItemCommand(item, op, effectiveValue, reason)));
    });
}

WorkflowInstance* HospitalFacade::findInstance(const std::string& instanceId) const
{
    for (const auto& instance : instances)
    {
        if(instance->getInstanceId() == instanceId)
        {
            return instance.get();
        }
    }
    return nullptr;
}

hospital::HospitalAdmissionCoordinator* HospitalFacade::findCoordinator(const std::string& instanceId) const
{
    std::map<std::string, hospital::HospitalAdmissionCoordinator*>::const_iterator found =
        coordinators.find(instanceId);
    return found == coordinators.end() ? nullptr : found->second;
}

Participant* HospitalFacade::operatorParticipant() const
{
    return firstWithRole(participants, hospital::HospitalRoles::chiefOfMedicine());
}

void HospitalFacade::openIfNeeded(WorkflowInstance* instance,
                                  hospital::HospitalAdmissionCoordinator* coordinator)
{
    Container* ward = instance->getContainer(hospital::HospitalAdmissionDirector::wardId());
    if(ward != nullptr &&
       ward->getPhaseName() == "Intake" &&
       !ward->getChildren().empty())
    {
        coordinator->openWard(*ward);
    }
}

std::unique_ptr<AssignmentStrategy> HospitalFacade::makeStrategy(const std::string& strategyName)
{
    if (strategyName == "Workload")
    {
        return std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy());
    }
    if (strategyName == "RoundRobin")
    {
        return std::unique_ptr<AssignmentStrategy>(new RoundRobinStrategy());
    }
    if (strategyName == "Random")
    {
        return std::unique_ptr<AssignmentStrategy>(new RandomStrategy());
    }
    return std::unique_ptr<AssignmentStrategy>();
}

CommandResult HospitalFacade::setStrategy(const std::string& instanceId,
                                           const std::string& strategyName)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if (instance == nullptr)
    {
        return CommandResult::NotFound;
    }

    std::unique_ptr<AssignmentStrategy> s = makeStrategy(strategyName);
    if (!s)
    {
        return CommandResult::NotFound;
    }

    instance->setStrategy(std::move(s));
    return CommandResult::Success;
}

std::string HospitalFacade::getStrategyName(const std::string& instanceId) const
{
    WorkflowInstance* instance = findInstance(instanceId);
    return instance != nullptr ? instance->getStrategyName() : "";
}