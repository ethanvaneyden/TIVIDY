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

    /* Shared helper: translate exceptions into CommandResult. */
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
}

EducationFacade::~EducationFacade() = default;

std::string EducationFacade::scenarioName() const
{
    return "education";
}

std::string EducationFacade::start(const std::string& name)
{
    /* Static counter for instance IDs — matches earlier behavior. */
    static int counter = 0;
    std::string instanceId = "edu_" + std::to_string(++counter);

    /* 1. Build the tree into a fresh WorkRegistry using the education builder. */
    std::unique_ptr<WorkRegistry> registry(new WorkRegistry());
    education::AssignmentMarkingBuilder builder(*registry);

    /* 2. Build the demo workflow (module + assignment + sample submissions). */
    education::AssignmentMarkingDirector director(builder);
    director.buildDemoWorkflow();

    Root* root = builder.getRoot();
    if(root == nullptr)
    {
        throw std::runtime_error("EducationFacade: builder produced no root");
    }

    /* 3. Create the education mediator. */
    std::unique_ptr<WorkflowMediator> coordinator(
        new education::AssignmentMarkingCoordinator(*participants));

    /* 4. Create the assignment strategy. */
    std::unique_ptr<AssignmentStrategy> strategy(new WorkloadStrategy());

    /* 5. Create a builder to hand to the instance (a second one - the
     *    instance's builder is used for later addContainer/addItem calls
     *    during runtime, not tree construction). */
    std::unique_ptr<WorkflowBuilder> runtimeBuilder(
        new education::AssignmentMarkingBuilder(*registry));

    /* 6. Assemble the instance. */
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

    /* Generate a unique container ID from a counter on the instance.
     * The education tree in the demo already contains one container
     * (ASSIGN-1) built by the director; new submissions extend it. */
    static int submissionCounter = 0;
    std::string containerId =
        "SUB-" + std::to_string(++submissionCounter);

    return classify([&]()
    {
        instance->addContainer(containerId, containerName);
    });
}

CommandResult EducationFacade::startItem(const std::string& instanceId,
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

CommandResult EducationFacade::completeItem(const std::string& instanceId,
                                            const std::string& itemId,
                                            int value)
{
    WorkflowInstance* instance = findInstance(instanceId);
    if(instance == nullptr)
    {
        return CommandResult::NotFound;
    }

    /* Range check before the engine sees the value. */
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

    /* The operator who runs the batch must be a lecturer. */
    Participant* op = firstWithRole(participants, "Lecturer");
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

    /* The operator for an override must be a lecturer. */
    Participant* op = firstWithRole(participants, "Lecturer");
    if(op == nullptr)
    {
        return CommandResult::NotFound;
    }

    /* accept == true means "keep the item's existing result" — pass it
     * through as the new value. accept == false means "use the value I
     * typed in" — the caller already put it in `value`. */
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

    return classify([&]()
    {
        instance->overrideItem(itemId, op, effectiveValue,
                               accept ? "Accepted" : "Overridden");
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

Participant* EducationFacade::operatorParticipant() const
{
    return firstWithRole(participants, "Lecturer");
}