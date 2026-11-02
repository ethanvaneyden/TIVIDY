#include "include/OverrideItemCommand.h"

#include "Item.h"
#include "Participant.h"

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

    /* Item::resolveReview() changes the state and notifies the Mediator
     * itself (onItemOverridden), which logs the override and publishes the
     * container when every item is settled. Notifying again here would log
     * the override twice. */
    item->resolveReview(operatorParticipant, newValue, reason);
}

std::string OverrideItemCommand::description() const {
    if (item == nullptr || operatorParticipant == nullptr) {
        return "Invalid item override";
    }

    return "Result of " + item->getId() + " overridden by " + operatorParticipant->getName() + ". Reason: " + reason;
}