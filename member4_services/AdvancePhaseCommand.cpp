#include "include/AdvancePhaseCommand.h"

AdvancePhaseCommand::AdvancePhaseCommand(Container* container)
    : container(container) {
}

void AdvancePhaseCommand::execute() {
    if (container != nullptr) {
        container->advancePhase();
    }
}

std::string AdvancePhaseCommand::description() const {
    if (container == nullptr) {
        return "Invalid phase advancement";
    }

    return "Advanced container to the next phase";
}