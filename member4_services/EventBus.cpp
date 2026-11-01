#include "include/EventBus.h"

void EventBus::subscribe(Observer* observer) {
    if (observer != nullptr) {
        subscribers.push_back(observer);
    }
}

void EventBus::publish(const WorkflowEvent& event) {
    for (Observer* observer : subscribers) {
        if (observer != nullptr) {
            observer->update(event);
        }
    }
}

std::size_t EventBus::observerCount() const {
    return subscribers.size();
}