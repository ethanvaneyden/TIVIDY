#pragma once

#include <string>
#include <vector>
#include <memory>

class Item;
class Container;
class Participant;
class AssignmentStrategy;
class WorkCommand;

class WorkflowMediator
{
public:
    virtual ~WorkflowMediator() = default;

    // Item events
    virtual void onItemAssigned(
        Item* item,
        Participant* assignee
    ) = 0;

    virtual void onItemStarted(Item* item) = 0;

    virtual void onItemCompleted(
        Item* item,
        int value
    ) = 0;

    virtual void onItemRejected(
        Item* item,
        const std::string& reason
    ) = 0;

    // Operator override
    virtual void onItemOverridden(
        Item* item,
        Participant* reviewer,
        int newValue,
        const std::string& reason
    ) = 0;

    // Phase changes
    virtual void onPhaseAdvanced(Container* container) = 0;

    // Assignment and batch events
    virtual void onItemAdded(Item* item) = 0;

    virtual void onAssignmentFailed(
        Item* item,
        const std::string& reason
    ) = 0;

    virtual void onContainerApprovalRequested(
        Container* container,
        Participant* operatorParticipant
    ) = 0;

    virtual void onContainerRejected(
        Container* container,
        const std::string& reason
    ) = 0;

    // Chain of Responsibility callback
    virtual void onApprovalResolved(
        Item* item,
        bool approved
    ) = 0;

    // Utility
    virtual std::vector<std::string>
    getHistoryAsStrings() const = 0;

    virtual void setAssignmentStrategy(
        std::unique_ptr<AssignmentStrategy> strategy
    ) = 0;

    virtual std::string getAssignmentStrategyName() const = 0;

    virtual void executeCommand(std::unique_ptr<WorkCommand> command) = 0;
};
