/**
 * @file EducationFacade.cpp
 * @brief Implementation of EducationFacade.
 */

#include "EducationFacade.h"

#include "WorkflowInstance.h"
#include "WorkRegistry.h"
#include "Root.h"
#include "Container.h"
#include "Item.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "InvalidTransition.h"

#include "AssignmentMarkingBuilder.h"
#include "AssignmentMarkingCoordinator.h"
#include "AssignmentMarkingDirector.h"
#include "EducationSeed.h"
#include "EducationPolicies.h"

#include "WorkloadStrategy.h"
#include "RoundRobinStrategy.h"
#include "RandomStrategy.h"

#include "ActionHistory.h"
#include "OverrideItemCommand.h"
#include "WorkCommand.h"

#include <functional>
#include <memory>
#include <stdexcept>

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

EducationFacade::EducationFacade(ParticipantRegistry* participants)
    : participants(participants)
{
    if(this->participants == nullptr)
    {
        throw std::invalid_argument("EducationFacade: participant registry required");
    }

    if(this->participants->getByRole(education::EducationRoles::lecturer()).empty())
    {
        education::EducationSeed::seedParticipants(*this->participants);
    }
}

EducationFacade::~EducationFacade() = default;

std::string EducationFacade::scenarioName() const
{
    return "education";
}

std::string EducationFacade::start(const std::string& name)
{
    static int counter = 0;
    std::string instanceId = "edu_" + std::to_string(++counter);

    std::unique_ptr<WorkRegistry> registry(new WorkRegistry());
    education::AssignmentMarkingBuilder builder(*registry);
    education::AssignmentMarkingDirector director(builder);
    director.buildDemoWorkflow();

    Root* root = builder.getRoot();
    if(root == nullptr)
    {
        throw std::runtime_error("EducationFacade: builder produced no root");
    }

    education::AssignmentMarkingCoordinator* coordinatorPtr =
        new education::AssignmentMarkingCoordinator(
            *participants,
            std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy()));
    std::unique_ptr<WorkflowMediator> coordinator(coordinatorPtr);

    std::unique_ptr<AssignmentStrategy> strategy(new WorkloadStrategy());

    std::unique_ptr<WorkflowBuilder> runtimeBuilder(
        new education::AssignmentMarkingBuilder(*registry));

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
    coordinators[instanceId] = coordinatorPtr;

    return instanceId;
}

std::vector<std::string> EducationFacade::listInstances() const
{
    std::vector<std::string> ids;
    ids.reserve(instances.size());
    for (const auto& instance : instances)
    {
        ids.push_back(instance->getInstanceId());
    }
    return ids;
}

Root* EducationFacade::getTree(const std::string& instanceId) const
{
    WorkflowInstance* instance = findInstance(instanceId);
    return instance == nullptr ? nullptr : instance->getRoot();
}

std::vector<std::string> EducationFacade::getHistory(const std::string& instanceId) const
{
    WorkflowInstance* instance = findInstance(instanceId);
    return instance == nullptr
        ? std::vector<std::string>()
        : instance->getHistory();
}

CommandResult EducationFacade::submit(const std::string& instanceId,
                                      const std::string& containerName)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if(instance == nullptr)
    {
        return CommandResult::NotFound;
    }

    static int submissionCounter = 0;
    std::string itemId;
    do
    {
        itemId = "SUB-" + std::to_string(++submissionCounter);
    } while (instance->getItem(itemId) != nullptr);

    return classify([&]()
    {
        instance->addItem(education::AssignmentMarkingDirector::assignmentId(),
                          itemId, containerName, itemId);
    });
}

CommandResult EducationFacade::startItem(const std::string& instanceId,
                                          const std::string& itemId)
{
    WorkflowInstance* instance = findInstance(instanceId);
    education::AssignmentMarkingCoordinator* coordinator = findCoordinator(instanceId);
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

CommandResult EducationFacade::completeItem(const std::string& instanceId,
                                            const std::string& itemId,
                                            int value)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if(instance == nullptr)
    {
        return CommandResult::NotFound;
    }

    if(!education::EducationPolicies::isValidMark(value))
    {
        return CommandResult::InvalidTransition;
    }

    return classify([&]()
    {
        instance->completeItem(itemId, value);
    });
}

CommandResult EducationFacade::approveContainer(const std::string& instanceId,
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

CommandResult EducationFacade::overrideItem(const std::string& instanceId,
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

    if(!education::EducationPolicies::isValidMark(effectiveValue))
    {
        return CommandResult::InvalidTransition;
    }

    const std::string reason = accept ? "Accepted" : "Overridden";

    return classify([&]()
    {
        /* Command pattern in the demo path. The command notifies the
         * mediator, which logs the override and checks whether the
         * container can publish. */
        ActionHistory invoker;
        invoker.execute(std::unique_ptr<WorkCommand>(
            new OverrideItemCommand(item, op, effectiveValue, reason)));
    });
}

WorkflowInstance* EducationFacade::findInstance(const std::string& instanceId) const
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

education::AssignmentMarkingCoordinator* EducationFacade::findCoordinator(const std::string& instanceId) const
{
    std::map<std::string, education::AssignmentMarkingCoordinator*>::const_iterator found =
        coordinators.find(instanceId);
    return found == coordinators.end() ? nullptr : found->second;
}

Participant* EducationFacade::operatorParticipant() const
{
    return firstWithRole(participants, education::EducationRoles::lecturer());
}

void EducationFacade::openIfNeeded(WorkflowInstance* instance,
                                   education::AssignmentMarkingCoordinator* coordinator)
{
    Container* assignment = instance->getContainer(education::AssignmentMarkingDirector::assignmentId());
    if(assignment != nullptr &&
       assignment->getPhaseName() == "Intake" &&
       !assignment->getChildren().empty())
    {
        coordinator->openAssignment(*assignment);
    }
}

std::unique_ptr<AssignmentStrategy> EducationFacade::makeStrategy(const std::string& strategyName)
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

CommandResult EducationFacade::setStrategy(const std::string& instanceId,
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

std::string EducationFacade::getStrategyName(const std::string& instanceId) const
{
    WorkflowInstance* instance = findInstance(instanceId);
    return instance != nullptr ? instance->getStrategyName() : "";
}