#include "DepthFirstIterator.h"

DepthFirstIterator::DepthFirstIterator(Component *root)
{
    this->root = root;
    this->curr = root;
}

Component *DepthFirstIterator::first()
{
    curr = root;
    return curr;
}
Component *DepthFirstIterator::next()
{
    if (!curr)
        return nullptr;

    
    std::vector<Component *> kids = curr->getChildren();
    if (!kids.empty())
    {
        curr = kids.front();
        return curr;
    }

    Component *node = curr;
    while (node != root && node->getParent())
    {
        Component *parent = node->getParent();
        std::vector<Component *> sibs = parent->getChildren();
        for (size_t i = 0; i + 1 < sibs.size(); ++i)
        {
            if (sibs[i] == node)
            {
                curr = sibs[i + 1];
                return curr;
            }
        }
        node = parent;
    }

    curr = nullptr;
    return nullptr;
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