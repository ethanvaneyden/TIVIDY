#include "OpenContainerCommand.h"

#include "InvalidTransition.h"
#include "WorkflowMediator.h"

OpenContainerCommand::OpenContainerCommand(Container *container)
    : container(container), assigned(0)
{
}

void OpenContainerCommand::execute()
{
    if (container == nullptr)
    {
        return;
    }
    WorkflowMediator *mediator = container->getMediator();
    if (mediator == nullptr)
    {
        throw InvalidTransition("Container is not attached to a workflow");
    }
    assigned = mediator->onContainerOpenRequested(container);
}

std::string OpenContainerCommand::description() const
{
    if (container == nullptr)
    {
        return "Invalid container opening";
    }
    return "Opened " + container->getId() + " for work";
}

int OpenContainerCommand::assignedCount() const
{
    return assigned;
}
