#include "include/ActionHistory.h"
using namespace std;

void ActionHistory::execute(unique_ptr<WorkCommand> command) {
    if (command == nullptr) {
        return;
    }
    command->execute();
    history.push_back(std::move(command));
}

vector<string> ActionHistory::getHistoryAsStrings() const {
    vector<string> descriptions;
    for (const auto& command : history) {
        descriptions.push_back(command->description());
    }
    return descriptions;
}
