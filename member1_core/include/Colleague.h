#ifndef COLLEAGUE_H
#define COLLEAGUE_H
class WorkflowMediator;

class Colleague
{
protected:
    WorkflowMediator *mediator = nullptr;

public:
    Colleague() = default;
    virtual ~Colleague() = default;

    void setMediator(WorkflowMediator *m);
    WorkflowMediator *getMediator() const;
};

#endif