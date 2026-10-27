#ifndef TIVIDY_EVENTBUS_H
#define TIVIDY_EVENTBUS_H
#include <memory>
#include <vector>

using namespace std;
#include "Observer.h"

class EventBus {
private:
    vector<unique_ptr<Observer>> subscribers;
public:
    void subscribe(unique_ptr<Observer> observer);
    void publish(WorkflowEvent event);
    size_t observerCount();
};


#endif //TIVIDY_EVENTBUS_H
