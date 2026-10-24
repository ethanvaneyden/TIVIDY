#ifndef WORKFLOWMEDIATOR_H
#define WORKFLOWMEDIATOR_H
class WorkflowMediator;

class Colleague {
protected:
    WorkflowMediator* mediator;

public:
    Colleague();
    virtual ~Colleague();

    void setMediator(WorkflowMediator* m);
    WorkflowMediator* getMediator() const;
};

#endif