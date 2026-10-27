#ifndef TIVIDY_OBSERVER_H
#define TIVIDY_OBSERVER_H

#include "WorkflowEvent.h"

class Observer {
public:
    void update(WorkflowEvent& event);
};


#endif //TIVIDY_OBSERVER_H
