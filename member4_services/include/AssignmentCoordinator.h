#pragma once

#include <memory>
#include <string>
#include <vector>

#include "WorkflowMediator.h"
#include "ActionHistory.h"
#include "EventBus.h"

#include "../../member1_core/include/WorkRegistry.h"
#include "../../member1_core/include/ParticipantRegistry.h"
#include "../../member3_routing/include/AssignmentEngine.h"

class Item;
class Container;
class Participant;
class AssignmentStrategy;

class AssignmentCoordinator : public WorkflowMediator
{
private:
    std::unique_ptr<AssignmentEngine> assignmentEngine;
    std::unique_ptr<ActionHistory> history;
    std::unique_ptr<EventBus> eventBus;

    WorkRegistry* registry;
    ParticipantRegistry* participantRegistry;

public:
    AssignmentCoordinator(
        WorkRegistry* registry,
        ParticipantRegistry* participantRegistry
    );

    ~AssignmentCoordinator() override;

    // Item events
    void onItemAssigned(
        Item* item,
        Participant* assignee
    ) override;

    void onItemStarted(Item* item) override;

    void onItemCompleted(
        Item* item,
        int value
    ) override;

    void onItemRejected(
        Item* item,
        const std::string& reason
    ) override;

    void onItemOverridden(
        Item* item,
        Participant* reviewer,
        int newValue,
        const std::string& reason
    ) override;

    // Phase events
    void onPhaseAdvanced(Container* container) override;

    // Assignment and batch events
    void onItemAdded(Item* item) override;

    void onAssignmentFailed(
        Item* item,
        const std::string& reason
    ) override;

    void onContainerApprovalRequested(
        Container* container,
        Participant* operatorParticipant
    ) override;

    void onContainerRejected(
        Container* container,
        const std::string& reason
    ) override;

    void onApprovalResolved(
        Item* item,
        bool approved
    ) override;

    // Coordinator operations
    void onSubmissionReceived(Item* item);
    void onMarkingCompleted(Item* item);
    void onModerationCompleted(Item* item);

    void setAssignmentStrategy(
        std::unique_ptr<AssignmentStrategy> strategy
    );

    std::vector<std::string> getHistoryAsStrings() const override;

    // Item operations
    void recordResult(Item* item, int value);

    void rejectItem(
        Item* item,
        const std::string& reason
    );

    void overrideItem(
        Item* item,
        Participant* reviewer,
        int newValue,
        const std::string& reason
    );

    // Container operations
    void advanceContainerPhase(Container* container);

    void approveContainer(Container* container);
};
