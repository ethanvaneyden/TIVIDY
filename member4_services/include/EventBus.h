#ifndef TIVIDY_EVENTBUS_H
#define TIVIDY_EVENTBUS_H
#include <memory>
#include <vector>

using namespace std;
#include "Observer.h"

class EventBus {
private:
    vector<Observer*> subscribers;
public:
    void subscribe(Observer* observer);
    void publish(const WorkflowEvent& event);
    size_t observerCount() const;
};


#endif //TIVIDY_EVENTBUS_H
