#include "include/RejectItemCommand.h"

RejectItemCommand::RejectItemCommand(
    Item* item,
    const std::string& reason
)
    : item(item), reason(reason) {
}

void RejectItemCommand::execute() {
    if (item != nullptr) {
        item->setRejectionReason(reason);
    }
}

std::string RejectItemCommand::description() const {
    if (item == nullptr) {
        return "Invalid item rejection";
    }

    return "Item rejected. Reason: " + reason;
}