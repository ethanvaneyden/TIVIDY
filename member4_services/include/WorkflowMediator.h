#ifndef WORKFLOWMEDIATOR_H
#define WORKFLOWMEDIATOR_H

#include <string>

class Colleague;

class WorkflowMediator {
public:
    WorkflowMediator() = default;
    virtual ~WorkflowMediator() = default;
    virtual void notify(Colleague* sender, const std::string& event) = 0;
};

#endif
