#include "BatchApproveCommand.h"

#include "InvalidTransition.h"
#include "WorkflowMediator.h"

BatchApproveCommand::BatchApproveCommand(Container *container, Participant *operatorParticipant)
    : container(container), operatorParticipant(operatorParticipant)
{
}

void BatchApproveCommand::execute()
{
    if (container == nullptr)
    {
        return;
    }
    if (container->getPhaseName() != "Review")
    {
        throw InvalidTransition("Batch approval requires the Review phase but " +
                                container->getId() + " is in " + container->getPhaseName());
    }
    WorkflowMediator *mediator = container->getMediator();
    if (mediator == nullptr)
    {
        throw InvalidTransition("Container is not attached to a workflow");
    }
    mediator->onContainerApprovalRequested(container, operatorParticipant);
}

std::string BatchApproveCommand::description() const
{
    if (container == nullptr)
    {
        return "Invalid batch approval";
    }
    const std::string who = operatorParticipant == nullptr ? "<nobody>" : operatorParticipant->getName();
    return "Batch approval of " + container->getId() + " requested by " + who;
}
