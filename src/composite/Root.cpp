#include "composite/Root.h"

Root::Root(std::string id, std::string type, std::string name, Component *parent) : Component(id, type, name, parent)
{
    children = {};
}

void Root::add(Component *c)
{
    if (!c)
    {
        return;
    }
    c->setParent(this);
    children.push_back(c);
}

void Root::remove(const std::string &id)
{
    for (auto it = children.begin(); it != children.end(); it++)
    {
        if ((*it)->getId() == id)
        {
            (*it)->setParent(nullptr);
            children.erase(it);
            return;
        }
    }
}

std::vector<Component *> Root::getChildren() const
{
    return std::vector<Component *>();
}

Iterator *Root::createIterator()
{
    // TODO implement when iterator interface is defined
    return nullptr;
}
