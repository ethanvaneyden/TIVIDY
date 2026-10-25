#include "composite/Item.h"

Item::Item(std::string id, std::string type, std::string name, Component *parent) : Component(id, type, name, parent)
{
    // TODO set state to available when interface defined
    assignee = nullptr;
    reviewer = nullptr;
}

Iterator *Item::createIterator()
{
    // TODO Implement once iterator interface is defined
    return nullptr;
}

bool Item::isCompleted() const
{
    // TODO Implement once state interface is defined
    return false;
}

// TODO Implement state delegation methods

Participant *Item::getAssignee() const
{
    return assignee;
}

void Item::setAssignee(Participant *p)
{
    assignee = p;
}

Participant *Item::getReviewer() const
{
    return reviewer;
}

const std::string &Item::getReviewMethod() const
{
    return reviewMethod;
}

void Item::recordReview(Participant *p, const std::string &method)
{
    reviewer = p;
    reviewMethod = method;
}
