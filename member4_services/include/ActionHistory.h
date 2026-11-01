#ifndef TIVIDY_ACTIONHISTORY_H
#define TIVIDY_ACTIONHISTORY_H

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "WorkCommand.h"
using namespace std;

class ActionHistory {
private:
    vector<unique_ptr<WorkCommand>> history;

public:
    void execute(unique_ptr<WorkCommand> command);
    vector<string> getHistoryAsStrings() const;
};

#endif // TIVIDY_ACTIONHISTORY_H
