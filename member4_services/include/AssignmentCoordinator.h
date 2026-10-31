#ifndef TIVIDY_ASSIGNMENTCOORDINATOR_H
#define TIVIDY_ASSIGNMENTCOORDINATOR_H

#include <memory>
#include <string>
#include <vector>

#include "WorkflowMediator.h"
#include "ActionHistory.h"
#include "EventBus.h"

#include "../member1_core/include/Item.h"

#include "../member1_core/include/WorkRegistry.h"
#include "../member1_core/include/ParticipantRegistry.h"
#include "../member3_routing/include/AssignmentEngine.h"
#include "../member3_routing/include/AssignmentStrategy.h"

class Item;
class Colleague;

class AssignmentCoordinator : public WorkflowMediator {
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
    void notify(
        Colleague* sender,
        const std::string& event
    ) override;
    void onSubmissionReceived(Item* item);
    void onMarkingCompleted(Item* item);
    void onModerationCompleted(Item* item);
    void setAssignmentStrategy(
        std::unique_ptr<AssignmentStrategy> strategy
    );
    std::vector<std::string> getHistory() const;
};

#endif // TIVIDY_ASSIGNMENTCOORDINATOR_H
