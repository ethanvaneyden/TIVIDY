#include "include/EventBus.h"

#include <utility>

void EventBus::subscribe(unique_ptr<Observer> observer) {
    if (observer) {
        subscribers.push_back(std::move(observer));
    }
}

void EventBus::publish(const WorkflowEvent& event) {
    for (const unique_ptr<Observer>& observer : subscribers) {
        observer->update(event);
    }
}

std::size_t EventBus::observerCount() const {
    return subscribers.size();
}