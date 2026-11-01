#include "include/OverrideItemCommand.h"

OverrideItemCommand::OverrideItemCommand(Item* item, Participant* operatorParticipant, int newValue, const std::string& reason) : item(item), operatorParticipant(operatorParticipant), newValue(newValue), reason(reason) {
}

void OverrideItemCommand::execute() {
    if (item != nullptr && operatorParticipant != nullptr) {
        item->setResult(newValue);
        item->setReviewer(operatorParticipant, "Personal");
    }
}

std::string OverrideItemCommand::description() const {
    if (item == nullptr || operatorParticipant == nullptr) {
        return "Invalid item override";
    }

    return "Result overridden by " + operatorParticipant->getName() + ". Reason: " + reason;
}