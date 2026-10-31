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
#include "HospitalCoordinator.h"      
#include "HospitalPolicies.h"         
#include "HospitalSeed.h"             

#include "WorkflowMediator.h"
#include "AssignmentStrategy.h"
#include "WorkloadStrategy.h"
#include "WorkflowBuilder.h"

#include <functional>
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
}

HospitalFacade::~HospitalFacade() = default;

std::string HospitalFacade::scenarioName() const
{
    return "healthcare";
}

std::string HospitalFacade::start(const std::string& name)
{
    /* Static counter for instance IDs — matches EducationFacade.*/
    static int counter = 0;
    std::string instanceId = "hc_" + std::to_string(++counter);

    /* 1. Build the tree into a fresh WorkRegistry.*/
    std::unique_ptr<WorkRegistry> registry(new WorkRegistry());
    hospital::HospitalAdmissionBuilder builder(*registry);

    /* 2. Build the demo workflow via the scenario Director.*/
    hospital::HospitalAdmissionDirector director(builder);
    director.buildDemoWorkflow();

    Root* root = builder.getRoot();
    if(root == nullptr)
    {
        throw std::runtime_error("HospitalFacade: builder produced no root");
    }

    /* 3. Create the hospital mediator.*/
    std::unique_ptr<WorkflowMediator> coordinator(
        new hospital::HospitalCoordinator(*participants));

    /* 4. Create the assignment strategy.*/
    std::unique_ptr<AssignmentStrategy> strategy(new WorkloadStrategy());

    /* 5. Create a runtime builder (for addContainer/addItem during execution).*/
    std::unique_ptr<WorkflowBuilder> runtimeBuilder(
        new hospital::HospitalAdmissionBuilder(*registry));

    /* 6. Assemble the instance.*/
    std::unique_ptr<WorkflowInstance> instance(new WorkflowInstance(
        instanceId,
        name,
        std::move(registry),
        root,
        std::move(coordinator),
        std::move(strategy),
        std::move(runtimeBuilder),
        participants));

    instances.push_back(std::move(instance));

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
    if(instance == nullptr)
    {
        return CommandResult::NotFound;
    }
    return classify([&]()
    {
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

    /* The operator for container approval is the Chief of Medicine.*/
    Participant* op = firstWithRole(participants, "Chief");
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

    Participant* op = firstWithRole(participants, "Chief");
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

    return classify([&]()
    {
        instance->overrideItem(itemId, op, effectiveValue,
                               accept ? "Accepted" : "Overridden");
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

Participant* HospitalFacade::operatorParticipant() const
{
    return firstWithRole(participants, "Chief");
}