#ifndef TIVIDY_OBSERVER_H
#define TIVIDY_OBSERVER_H

#include "WorkflowEvent.h"

class Observer {
public:
    virtual void update(const WorkflowEvent& event) = 0;
    virtual ~Observer() = default;
};


#endif //TIVIDY_OBSERVER_H
