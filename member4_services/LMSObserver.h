#ifndef TIVIDY_LMSOBSERVER_H
#define TIVIDY_LMSOBSERVER_H
#include "Observer.h"


class LMSObserver : public Observer {
    public:
    void update(WorkflowEvent& event);
};


#endif //TIVIDY_LMSOBSERVER_H
