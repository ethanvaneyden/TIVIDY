#include "include/ApproveContainerCommand.h"

ApproveContainerCommand::ApproveContainerCommand(Container* container)
    : container(container) {
}

void ApproveContainerCommand::execute() {
    if (container != nullptr && container->isReadyToPublish()) {
        container->advancePhase();
    }
}

std::string ApproveContainerCommand::description() const {
    if (container == nullptr) {
        return "Invalid container approval";
    }

    return "Container approved for batch processing";
}