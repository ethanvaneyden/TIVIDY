/**
 * @file WorkflowMediator.h
 * @brief Mediator interface between workflow components (GoF Mediator).
 *
 * Items, containers and handlers never talk to each other directly. They
 * report what happened to the mediator and the concrete mediator
 * (AssignmentMarkingCoordinator / HospitalAdmissionCoordinator) decides what
 * must happen next: assign workers, advance phases, run the approval chain,
 * publish events and keep the audit history.
 *
 * Participants:
 *  - Mediator:           WorkflowMediator
 *  - ConcreteMediators:  education::AssignmentMarkingCoordinator,
 *                        hospital::HospitalAdmissionCoordinator
 *  - Colleagues:         Component (Root, Container, Item), AssignmentEngine
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

class Item;
class Container;
class Participant;
class AssignmentStrategy;
class ActionHistory;

class WorkflowMediator
{
public:
    virtual ~WorkflowMediator() = default;

    // Item events
    virtual void onItemAssigned(Item* item, Participant* assignee) = 0;
    virtual void onItemStarted(Item* item) = 0;
    virtual void onItemCompleted(Item* item, int value) = 0;
    virtual void onItemRejected(Item* item, const std::string& reason) = 0;

    // Operator override
    virtual void onItemOverridden(
        Item* item,
        Participant* reviewer,
        int newValue,
        const std::string& reason) = 0;

    // Phase changes
    virtual void onPhaseAdvanced(Container* container) = 0;

    // Assignment and batch events
    virtual void onItemAdded(Item* item) = 0;
    virtual void onAssignmentFailed(Item* item, const std::string& reason) = 0;
    virtual void onContainerApprovalRequested(
        Container* container,
        Participant* operatorParticipant) = 0;
    virtual void onContainerRejected(Container* container, const std::string& reason) = 0;

    // Chain of Responsibility callback
    virtual void onApprovalResolved(Item* item, bool approved) = 0;

    // Utility
    virtual std::vector<std::string> getHistoryAsStrings() const = 0;

    // ------------------------------------------------------------------
    // Optional capabilities. Concrete mediators override these; the
    // defaults keep simple test mediators working without changes.
    // ------------------------------------------------------------------

    /**
     * @brief Opens a container: Intake -> Active and assigns a worker to every item.
     * @return Number of items that received a worker.
     * @throws InvalidTransition By default, and when the container cannot be opened.
     */
    virtual int onContainerOpenRequested(Container* container);

    /**
     * @brief Swaps the assignment strategy used for future assignments.
     * @param strategy On success ownership is taken and the pointer is reset.
     * @return true if this mediator uses strategies; false (strategy untouched) otherwise.
     */
    virtual bool setAssignmentStrategy(std::unique_ptr<AssignmentStrategy>& strategy);

    /** @brief Name of the active assignment strategy, or "None". */
    virtual std::string assignmentStrategyName() const;

    /**
     * @brief The Command invoker that records this workflow's audit trail.
     * @return Non-owning pointer, or nullptr when the mediator keeps no command history.
     */
    virtual ActionHistory* getActionHistory();
};
