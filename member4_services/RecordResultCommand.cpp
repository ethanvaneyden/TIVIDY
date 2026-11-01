#include "include/RecordResultCommand.h"

RecordResultCommand::RecordResultCommand(Item* item, int newValue)
    : item(item), newValue(newValue) {
}

void RecordResultCommand::execute() {
    if (item != nullptr) {
        item->setResult(newValue);
    }
}

std::string RecordResultCommand::description() const {
    if (item == nullptr) {
        return "Invalid result recording";
    }
    return "Recorded result: " + std::to_string(newValue);
}