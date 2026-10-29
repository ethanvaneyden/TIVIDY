#ifndef TIVIDY_ASSIGNMENTCOORDINATOR_H
#define TIVIDY_ASSIGNMENTCOORDINATOR_H

#include <memory>
#include <string>

#include "WorkflowMediator.h"
#include "ActionHistory.h"
#include "EventBus.h"

#include "../AssignmentEngine.h"
#include "../ApprovalHandler.h"
#include "../WorkRegistry.h"
#include "../AssignmentStrategy.h"

class AssignmentCoordinator : public WorkflowMediator {
private:
    std::unique_ptr<AssignmentEngine> assignmentEngine;
    std::unique_ptr<ActionHistory> history;
    std::unique_ptr<ApprovalHandler> approvalChain;
    std::unique_ptr<EventBus> eventBus;
    WorkRegistry* registry = nullptr;
    AssignmentStrategy* assignmentStrategy = nullptr;
public:
    AssignmentCoordinator();
    ~AssignmentCoordinator() override;
    void notify(Colleague* sender, const std::string& event) override;
    void onSubmissionReceived(WorkItem* item);
    void onMarkingCompleted(WorkItem* item);
    void onModerationCompleted(WorkItem* item);
};

#endif // TIVIDY_ASSIGNMENTCOORDINATOR_H