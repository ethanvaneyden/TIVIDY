#pragma once
#include <string>

// Forward declarations to avoid circular dependencies
class Item;
class Container;
class Participant;

/**
 * @brief The Mediator interface.
 *
 * Colleagues (Items, Containers, AssignmentEngine, etc.) call these methods
 * to notify the Coordinator of events. The Coordinator then decides
 * what to do next (e.g., record history, trigger the chain, advance phases).
 */
class WorkflowMediator {
public:
    virtual ~WorkflowMediator() = default;

    // --- Tree Events ---

    /** @brief Called by an Item when it transitions to Completed. */
    virtual void onItemCompleted(Item* item, int value) = 0;

    /** @brief Called by an Item when a worker rejects it. */
    virtual void onItemRejected(Item* item, const std::string& reason) = 0;

    /** @brief Called by an Item when an Operator overrides a rejected state. */
    virtual void onItemOverridden(Item* item, const std::string& reason) = 0;

    /** @brief Called by a Container when its phase changes (e.g., Active -> Review). */
    virtual void onPhaseAdvanced(Container* container) = 0;

    // --- Chain of Responsibility Events ---

    /** @brief Called by an ApprovalHandler when a batch approval is resolved. */
    virtual void onApprovalResolved(Item* item, bool approved) = 0;

    // --- Assignment Events (Optional) ---

    /** @brief Called by the AssignmentEngine if no worker is available. */
    virtual void onAssignmentFailed(Item* item, const std::string& reason) = 0;
};
