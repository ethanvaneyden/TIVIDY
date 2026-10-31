#include "include//SystemLogCommand.h"

SystemLogCommand::SystemLogCommand(const std::string& message)
    : message(message) {
}

void SystemLogCommand::execute() {
    // Logging command does not perform any additional action.
}

std::string SystemLogCommand::description() const {
    return message;
}