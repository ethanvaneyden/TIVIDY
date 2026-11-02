#include "ActionHistory.h"

#include "SystemLogCommand.h"

#include <algorithm>
#include <exception>

void ActionHistory::execute(std::unique_ptr<WorkCommand> command) {
    if (command == nullptr) {
        return;
    }

    WorkCommand* raw = command.get();
    const std::string intent = raw->description();
    history.push_back(std::move(command));

    try {
        raw->execute();
    } catch (const std::exception& error) {
        // Remove the failed command by identity: events logged while it ran
        // may already sit behind it.
        for (std::vector<std::unique_ptr<WorkCommand>>::iterator it = history.begin();
             it != history.end(); ++it) {
            if (it->get() == raw) {
                history.erase(it);
                break;
            }
        }
        log("Denied: " + intent + " (" + error.what() + ")");
        throw;
    } catch (...) {
        for (std::vector<std::unique_ptr<WorkCommand>>::iterator it = history.begin();
             it != history.end(); ++it) {
            if (it->get() == raw) {
                history.erase(it);
                break;
            }
        }
        log("Denied: " + intent);
        throw;
    }
}

void ActionHistory::log(const std::string& message) {
    std::unique_ptr<WorkCommand> entry(new SystemLogCommand(message));
    entry->execute();
    history.push_back(std::move(entry));
}

std::vector<std::string> ActionHistory::getHistoryAsStrings() const {
    std::vector<std::string> descriptions;
    descriptions.reserve(history.size());
    for (const std::unique_ptr<WorkCommand>& command : history) {
        descriptions.push_back(command->description());
    }
    return descriptions;
}

std::size_t ActionHistory::size() const {
    return history.size();
}
