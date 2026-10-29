#ifndef TIVIDY_ACTIONHISTORY_H
#define TIVIDY_ACTIONHISTORY_H
#include <memory>
#include <vector>

#include "WorkCommand.h"

using namespace std;
#include <vector>

class ActionHistory {
private:
    vector<unique_ptr<WorkCommand>> history;
    vector<unique_ptr<WorkCommand>> undone;
public:
    void execute(WorkCommand* command);
    void undoLast();
    void redoLast();
    List<WorkCommand*> getHistory();
};


#endif //TIVIDY_ACTIONHISTORY_H
