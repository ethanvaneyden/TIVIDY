#include "include/AssignItemCommand.h"

AssignItemCommand::AssignItemCommand(Item* item, Participant* newAssignee) : item(item), newAssignee(newAssignee) {
}

void AssignItemCommand::execute() {
    if (item != nullptr && newAssignee != nullptr) {
        item->setAssignee(newAssignee);
    }
}

std::string AssignItemCommand::description() const {
    if (item == nullptr || newAssignee == nullptr) {
        return "Invalid item assignment";
    }

    return "Assigned " + item->getId() + " to " + newAssignee->getId();
}