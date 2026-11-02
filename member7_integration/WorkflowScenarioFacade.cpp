/**
 * @file WorkflowScenarioFacade.cpp
 * @brief Generic facade implementation shared by all scenarios.
 */
#include "WorkflowScenarioFacade.h"

#include "ActionHistory.h"
#include "BatchApproveCommand.h"
#include "Container.h"
#include "InvalidTransition.h"
#include "Item.h"
#include "OpenContainerCommand.h"
#include "OverrideItemCommand.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "RandomStrategy.h"
#include "RecordResultCommand.h"
#include "RoundRobinStrategy.h"
#include "Root.h"
#include "StartItemCommand.h"
#include "WorkRegistry.h"
#include "WorkflowBuilder.h"
#include "WorkflowDirector.h"
#include "WorkflowInstance.h"
#include "WorkflowMediator.h"
#include "WorkloadStrategy.h"

#include <algorithm>
#include <stdexcept>

namespace
{
    /// Thrown inside run() bodies when an id does not exist.
    class NotFoundError : public std::runtime_error
    {
    public:
        explicit NotFoundError(const std::string& what) : std::runtime_error(what) {}
    };

    /// Thrown when the request itself is not acceptable (e.g. mark out of range).
    class RejectedError : public std::runtime_error
    {
    public:
        explicit RejectedError(const std::string& what) : std::runtime_error(what) {}
    };

    std::string sanitize(const std::string& raw)
    {
        std::string out;
        for (std::string::size_type i = 0; i < raw.size() && out.size() < 60; ++i)
        {
            const unsigned char c = static_cast<unsigned char>(raw[i]);
            if (c < 32 || raw[i] == '/' || raw[i] == '\\' || raw[i] == '?' ||
                raw[i] == '#' || raw[i] == '%')
            {
                out += '-';
            }
            else
            {
                out += raw[i];
            }
        }
        const std::string::size_type first = out.find_first_not_of(' ');
        if (first == std::string::npos)
        {
            return "untitled";
        }
        const std::string::size_type last = out.find_last_not_of(' ');
        return out.substr(first, last - first + 1);
    }
}

WorkflowScenarioFacade::WorkflowScenarioFacade(
    ParticipantRegistry* participantRegistry,
    std::unique_ptr<WorkflowComponentFactory> componentFactory)
    : participants(participantRegistry),
      factory(std::move(componentFactory)),
      instanceCounter(0)
{
    if (participants == nullptr)
    {
        throw std::invalid_argument("Scenario facade: participant registry required");
    }
    if (!factory)
    {
        throw std::invalid_argument("Scenario facade: component factory required");
    }
    factory->seedParticipants(*participants);
}

WorkflowScenarioFacade::~WorkflowScenarioFacade() = default;

std::string WorkflowScenarioFacade::scenarioName() const
{
    return factory->scenarioName();
}

WorkflowInstance* WorkflowScenarioFacade::findInstance(const std::string& instanceId) const
{
    for (std::vector<std::unique_ptr<WorkflowInstance>>::const_iterator it = instances.begin();
         it != instances.end(); ++it)
    {
        if ((*it)->getInstanceId() == instanceId)
        {
            return it->get();
        }
    }
    return nullptr;
}

Participant* WorkflowScenarioFacade::operatorParticipant() const
{
    const std::vector<Participant*> candidates = participants->getByRole(factory->operatorRole());
    return candidates.empty() ? nullptr : candidates.front();
}

std::string WorkflowScenarioFacade::nextId(const std::string& instanceId, const std::string& prefix,
                                           const WorkflowInstance& instance)
{
    int& counter = idCounters[instanceId + "|" + prefix];
    std::string id;
    do
    {
        id = prefix + (prefix.size() == 1 ? "" : "-") + std::to_string(++counter);
    } while (instance.getItem(id) != nullptr || instance.getContainer(id) != nullptr);
    return id;
}

CommandResult WorkflowScenarioFacade::run(const std::string& instanceId,
                                          const std::function<void(WorkflowInstance&)>& body)
{
    message.clear();
    WorkflowInstance* instance = findInstance(instanceId);
    if (instance == nullptr)
    {
        message = "Instance not found: " + instanceId;
        return CommandResult::NotFound;
    }

    try
    {
        body(*instance);
        return CommandResult::Success;
    }
    catch (const NotFoundError& e)
    {
        message = e.what();
        return CommandResult::NotFound;
    }
    catch (const RejectedError& e)
    {
        message = e.what();
        return CommandResult::Rejected;
    }
    catch (const InvalidTransition& e)
    {
        message = e.what();
        return CommandResult::InvalidTransition;
    }
    catch (const std::invalid_argument& e)
    {
        message = e.what();
        return CommandResult::Rejected;
    }
    catch (const std::exception& e)
    {
        // logic_error / runtime_error from the engine: an operation that is
        // not legal in the current state.
        message = e.what();
        return CommandResult::InvalidTransition;
    }
}

std::string WorkflowScenarioFacade::start(const std::string& name)
{
    const std::string display = sanitize(name);
    const std::string instanceId =
        factory->instancePrefix() + "_" + std::to_string(instanceCounter) + "_" + display;

    std::unique_ptr<WorkRegistry> registry(new WorkRegistry());

    // Builder + Director: assemble the demo tree for this scenario.
    std::unique_ptr<WorkflowBuilder> treeBuilder = factory->createBuilder();
    treeBuilder->setRegistry(*registry);
    std::unique_ptr<WorkflowDirector> director = factory->createDirector(*treeBuilder);
    Root* root = nullptr;
    try
    {
        root = director->buildDemoTree(*treeBuilder, display);
    }
    catch (const std::invalid_argument&)
    {
        // The name clashed with a generated id; build again with a suffix.
        registry.reset(new WorkRegistry());
        treeBuilder = factory->createBuilder();
        treeBuilder->setRegistry(*registry);
        director = factory->createDirector(*treeBuilder);
        root = director->buildDemoTree(*treeBuilder, display + " workspace");
    }
    if (root == nullptr)
    {
        throw std::runtime_error("Scenario facade: builder produced no root");
    }

    // A second builder stays with the instance for runtime additions.
    std::unique_ptr<WorkflowBuilder> runtimeBuilder = factory->createBuilder();
    runtimeBuilder->setRegistry(*registry);

    std::unique_ptr<WorkflowMediator> coordinator =
        factory->createCoordinator(*participants, factory->createStrategy());

    std::unique_ptr<WorkflowInstance> instance(new WorkflowInstance(
        instanceId,
        factory->scenarioName(),
        std::move(registry),
        root,
        std::move(coordinator),
        factory->createStrategy(),
        std::move(runtimeBuilder),
        participants));

    instances.push_back(std::move(instance));
    ++instanceCounter;
    return instanceId;
}

std::vector<std::string> WorkflowScenarioFacade::listInstances() const
{
    std::vector<std::string> ids;
    ids.reserve(instances.size());
    for (std::vector<std::unique_ptr<WorkflowInstance>>::const_iterator it = instances.begin();
         it != instances.end(); ++it)
    {
        ids.push_back((*it)->getInstanceId());
    }
    return ids;
}

Root* WorkflowScenarioFacade::getTree(const std::string& instanceId) const
{
    WorkflowInstance* instance = findInstance(instanceId);
    return instance == nullptr ? nullptr : instance->getRoot();
}

std::vector<std::string> WorkflowScenarioFacade::getHistory(const std::string& instanceId) const
{
    WorkflowInstance* instance = findInstance(instanceId);
    return instance == nullptr ? std::vector<std::string>() : instance->getHistory();
}

CommandResult WorkflowScenarioFacade::submit(const std::string& instanceId,
                                             const std::string& containerName)
{
    return legacySubmit(instanceId, containerName);
}

CommandResult WorkflowScenarioFacade::legacySubmit(const std::string& instanceId,
                                                   const std::string& name)
{
    return addContainer(instanceId, name);
}

CommandResult WorkflowScenarioFacade::addContainer(const std::string& instanceId,
                                                   const std::string& name)
{
    return run(instanceId, [&](WorkflowInstance& instance)
    {
        if (name.empty())
        {
            throw RejectedError("A name is required");
        }
        const std::string id = nextId(instanceId, factory->containerPrefix(), instance);
        instance.addContainer(id, name);
        instance.getActionHistory()->log("Added " + id + " (" + name + ")");
    });
}

CommandResult WorkflowScenarioFacade::addItem(const std::string& instanceId,
                                              const std::string& containerId,
                                              const std::string& name,
                                              const std::map<std::string, std::string>& attributes)
{
    return run(instanceId, [&](WorkflowInstance& instance)
    {
        if (name.empty())
        {
            throw RejectedError("A name is required");
        }
        Container* container = instance.getContainer(containerId);
        if (container == nullptr)
        {
            throw NotFoundError("Container not found: " + containerId);
        }
        const std::string id = nextId(instanceId, factory->itemPrefix(), instance);
        instance.addItem(containerId, id, name, id, attributes);
    });
}

CommandResult WorkflowScenarioFacade::openContainer(const std::string& instanceId,
                                                    const std::string& containerId)
{
    return run(instanceId, [&](WorkflowInstance& instance)
    {
        Container* container = instance.getContainer(containerId);
        if (container == nullptr)
        {
            throw NotFoundError("Container not found: " + containerId);
        }
        instance.getActionHistory()->execute(
            std::unique_ptr<WorkCommand>(new OpenContainerCommand(container)));
    });
}

CommandResult WorkflowScenarioFacade::removeInstance(const std::string& instanceId)
{
    message.clear();
    for (std::vector<std::unique_ptr<WorkflowInstance>>::iterator it = instances.begin();
         it != instances.end(); ++it)
    {
        if ((*it)->getInstanceId() == instanceId)
        {
            instances.erase(it);
            return CommandResult::Success;
        }
    }
    message = "Instance not found: " + instanceId;
    return CommandResult::NotFound;
}

void WorkflowScenarioFacade::openContainerOf(WorkflowInstance& instance, Item& item)
{
    Container* container = dynamic_cast<Container*>(item.getParent());
    if (container != nullptr && container->getPhaseName() == "Intake" &&
        !container->getChildren().empty())
    {
        instance.getActionHistory()->execute(
            std::unique_ptr<WorkCommand>(new OpenContainerCommand(container)));
    }
}

CommandResult WorkflowScenarioFacade::startItem(const std::string& instanceId,
                                                const std::string& itemId)
{
    return run(instanceId, [&](WorkflowInstance& instance)
    {
        Item* item = instance.getItem(itemId);
        if (item == nullptr)
        {
            throw NotFoundError("Item not found: " + itemId);
        }
        openContainerOf(instance, *item); // first start opens the container
        instance.getActionHistory()->execute(
            std::unique_ptr<WorkCommand>(new StartItemCommand(item)));
    });
}

CommandResult WorkflowScenarioFacade::completeItem(const std::string& instanceId,
                                                   const std::string& itemId,
                                                   int value)
{
    return run(instanceId, [&](WorkflowInstance& instance)
    {
        Item* item = instance.getItem(itemId);
        if (item == nullptr)
        {
            throw NotFoundError("Item not found: " + itemId);
        }
        if (!factory->isEnterableValue(value))
        {
            throw RejectedError("Value " + std::to_string(value) + " is outside the allowed range");
        }
        instance.getActionHistory()->execute(
            std::unique_ptr<WorkCommand>(new RecordResultCommand(item, value)));
    });
}

CommandResult WorkflowScenarioFacade::approveContainer(const std::string& instanceId,
                                                       const std::string& containerId)
{
    Participant* op = operatorParticipant();
    return run(instanceId, [&](WorkflowInstance& instance)
    {
        Container* container = instance.getContainer(containerId);
        if (container == nullptr)
        {
            throw NotFoundError("Container not found: " + containerId);
        }
        if (op == nullptr)
        {
            throw NotFoundError("No " + factory->operatorRole() + " is registered");
        }
        instance.getActionHistory()->execute(
            std::unique_ptr<WorkCommand>(new BatchApproveCommand(container, op)));
    });
}

CommandResult WorkflowScenarioFacade::overrideItem(const std::string& instanceId,
                                                   const std::string& itemId,
                                                   int value,
                                                   bool accept)
{
    Participant* op = operatorParticipant();
    return run(instanceId, [&](WorkflowInstance& instance)
    {
        Item* item = instance.getItem(itemId);
        if (item == nullptr)
        {
            throw NotFoundError("Item not found: " + itemId);
        }
        if (op == nullptr)
        {
            throw NotFoundError("No " + factory->operatorRole() + " is registered");
        }
        if (!item->isRejected())
        {
            throw InvalidTransition("Only rejected items can be overridden (" + itemId +
                                    " is " + item->getStateName() + ")");
        }
        const int effective = accept ? item->getResult() : value;
        if (!factory->isSettleableValue(effective))
        {
            throw RejectedError("Value " + std::to_string(effective) +
                                " cannot settle a rejected item");
        }
        const std::string reason = accept ? "Accepted" : "Overridden";
        instance.getActionHistory()->execute(
            std::unique_ptr<WorkCommand>(new OverrideItemCommand(item, op, effective, reason)));
    });
}

std::unique_ptr<AssignmentStrategy> WorkflowScenarioFacade::makeStrategy(const std::string& strategyName)
{
    if (strategyName == "Workload")
        return std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy());
    if (strategyName == "RoundRobin")
        return std::unique_ptr<AssignmentStrategy>(new RoundRobinStrategy());
    if (strategyName == "Random")
        return std::unique_ptr<AssignmentStrategy>(new RandomStrategy());
    return std::unique_ptr<AssignmentStrategy>();
}

CommandResult WorkflowScenarioFacade::setStrategy(const std::string& instanceId,
                                                  const std::string& strategyName)
{
    return run(instanceId, [&](WorkflowInstance& instance)
    {
        std::unique_ptr<AssignmentStrategy> strategy = makeStrategy(strategyName);
        if (!strategy)
        {
            throw NotFoundError("Unknown strategy: " + strategyName);
        }
        instance.setStrategy(std::move(strategy));
    });
}

std::string WorkflowScenarioFacade::getStrategyName(const std::string& instanceId) const
{
    WorkflowInstance* instance = findInstance(instanceId);
    return instance != nullptr ? instance->getStrategyName() : "";
}

std::string WorkflowScenarioFacade::lastMessage() const
{
    return message;
}
