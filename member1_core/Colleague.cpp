#include "Colleague.h"
#include "WorkflowMediator.h"

void Colleague::setMediator(WorkflowMediator *m)
{
    mediator = m;
}

WorkflowMediator *Colleague::getMediator() const
{
    return mediator;
}
