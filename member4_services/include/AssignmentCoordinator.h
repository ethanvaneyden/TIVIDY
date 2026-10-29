#ifndef TIVIDY_ASSIGNMENTCOORDINATOR_H
#define TIVIDY_ASSIGNMENTCOORDINATOR_H

#include "ActionHistory.h"
#include "Colleague.h"
#include "EventBus.h"

using namespace std;
#include <memory>
#include "WorkFlow"

class AssignmentCoordinator : public WorkflowMediator{
private:
    unique_ptr<AssignmentEngine> assignmentEngine;
    unique_ptr<ActionHistory> history;
    unique_ptr<AprovalHandler> approvalChain;
    unique_ptr<EventBus> eventBus;
    WorkRegistry* registry;
    AssignmentStrategy* assignmentStrategy;
public:
    AssignmentCoordinator();//////////////////////
    void notify(Colleague* sender, string event) override;
    void onSubmissionReceived(WorkItem* item);
    void onMarkingCompleted(WorkItem* item);
    void onModerationCompleted(WorkItem* item);
};


#endif //TIVIDY_ASSIGNMENTCOORDINATOR_H
