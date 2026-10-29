#ifndef TIVIDY_COLLEAGUE_H
#define TIVIDY_COLLEAGUE_H

#include <string>

class WorkflowMediator;

class Colleague {
protected:
    WorkflowMediator* mediator = nullptr;
public:
    virtual ~Colleague() = default;
    void setMediator(WorkflowMediator* mediator);
    virtual void notify(const std::string& event) = 0;
};

#endif