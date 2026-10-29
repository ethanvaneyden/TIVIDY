#include "DepthFirstIterator.h"

DepthFirstIterator::DepthFirstIterator(Component *root)
{
    this->root = root;
    this->curr = root;
}

Component *DepthFirstIterator::first()
{
    pending.clear();
    curr = root;
    return curr;
}
Component *DepthFirstIterator::next()
{
    if (!curr)
        return nullptr;

    const std::vector<Component*> children = curr->getChildren();
    for (auto child = children.rbegin(); child != children.rend(); ++child)
        pending.push_back(*child);

    if (pending.empty())
    {
        curr = nullptr;
        return nullptr;
    }

    curr = pending.back();
    pending.pop_back();
    return curr;
}
bool DepthFirstIterator::isDone()
{
    if (curr == nullptr)
        return true;

    return false;
}
Component *DepthFirstIterator::current()
{
    return curr;
}