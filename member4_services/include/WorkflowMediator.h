#ifndef WORKFLOWMEDIATOR_H
#define WORKFLOWMEDIATOR_H

/**
 * @file WorkflowMediator.h
 * @brief STUB — replaced by Member 4's full implementation.
*/

using namespace std;
#include <string>
#include "Colleague.h"
#include "WorkItem.h"

class WorkflowMediator
{
    public:
        WorkflowMediator();
        virtual ~WorkflowMediator();
        virtual void notify(Colleague* sender, string event);
        virtual void onSubmissionReceived(WorkItem* item);
        virtual void onMarkingCompleted(WorkItem* item);
        virtual void onModerationCompleted(WorkItem* item);
};

#endif