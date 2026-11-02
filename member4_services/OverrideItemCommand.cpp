#include "include/OverrideItemCommand.h"

#include "Item.h"
#include "Participant.h"
#include "WorkflowMediator.h"

OverrideItemCommand::OverrideItemCommand(Item* item, Participant* operatorParticipant, int newValue, const std::string& reason)
    : item(item),
      operatorParticipant(operatorParticipant),
      newValue(newValue),
      reason(reason)
{
}

void OverrideItemCommand::execute()
{
    if(item == nullptr || operatorParticipant == nullptr)
    {
        return;
    }

    /* 1. Apply the state change to the item. */
    item->resolveReview(operatorParticipant, newValue, reason);

    /* 2. Notify the mediator so it can log the override and check
     *    whether the container is now ready to publish. */
    WorkflowMediator* mediator = item->getMediator();
    if(mediator != nullptr)
    {
        mediator->onItemOverridden(item, operatorParticipant, newValue, reason);
    }
}

std::string OverrideItemCommand::description() const {
    if (item == nullptr || operatorParticipant == nullptr) {
        return "Invalid item override";
    }

    return "Result overridden by " + operatorParticipant->getName() + ". Reason: " + reason;
}