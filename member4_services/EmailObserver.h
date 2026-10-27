#ifndef TIVIDY_EMAILOBSERVER_H
#define TIVIDY_EMAILOBSERVER_H
#include "Observer.h"


class EmailObserver : public Observer {
    public:
    void update(WorkflowEvent& event);
};


#endif //TIVIDY_EMAILOBSERVER_H
