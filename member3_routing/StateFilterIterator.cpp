#include "StateFilterIterator.h"

StateFilterIterator::StateFilterIterator(Component *root, const std::string &targetstate) : DepthFirstIterator(root), state(targetstate) {}

Component *StateFilterIterator::FindTarget()
{
    while (current() && current()->getStateName() != state)
        DepthFirstIterator::next();
    return current();
}
Component *StateFilterIterator::first()
{
    DepthFirstIterator::first();
    return FindTarget();
}
Component *StateFilterIterator::next()
{
    DepthFirstIterator::next();
    return FindTarget();
}