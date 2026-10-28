#pragma once
#include <string>
#include <vector>

class Item;
class Container;
class Participant;

/**
 * @brief The Mediator interface.
 * 
 * Colleagues (Items, Containers, AssignmentEngine, etc.) call these methods
 * to notify the Coordinator of events. 
 */
class WorkflowMediator {
public:
    virtual ~WorkflowMediator() = default;

    // ==========================================
    // TREE EVENTS (Called by Items)
    // ==========================================
    virtual void onItemAssigned(Item* item, Participant* assignee) = 0;
    virtual void onItemStarted(Item* item) = 0;
    virtual void onItemCompleted(Item* item, int value) = 0;
    virtual void onItemRejected(Item* item, const std::string& reason) = 0;
    
    // Operator override
    virtual void onItemOverridden(Item* item, Participant* reviewer, int newValue, const std::string& reason) = 0;
    
    // Phase changes
    virtual void onPhaseAdvanced(Container* container) = 0;

    // ==========================================
    // ASSIGNMENT & BATCH EVENTS
    // ==========================================
    virtual void onItemAdded(Item* item) = 0;
    virtual void onAssignmentFailed(Item* item, const std::string& reason) = 0;
    
    // Batch approvals
    virtual void onContainerApprovalRequested(Container* container, Participant* operatorParticipant) = 0;
    virtual void onContainerRejected(Container* container, const std::string& reason) = 0;
    
    // Called by the Chain of Responsibility
    virtual void onApprovalResolved(Item* item, bool approved) = 0;

    // ==========================================
    // UTILITY
    // ==========================================
    virtual std::vector<std::string> getHistoryAsStrings() const = 0;
};