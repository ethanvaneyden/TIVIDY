#include "composite/Container.h"

Container::Container(std::string id, std::string type, std::string name, Component *parent) : Component(id, type, name, parent)
{
    // TODO Intialise phase
    children = {};
}

void Container::add(Component *c)
{
    if (c)
    {
        children.push_back(c);
    }
}

void Container::remove(const std::string &id)
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

std::vector<Component *> Container::getChildren() const
{
    return children;
}

Iterator *Container::createIterator()
{
    // TODO Implement once iterator interface is defined
    return nullptr;
}

bool Container::isCompleted() const
{
    for(auto it = children.begin(); it != children.end(); it++) {

        if((*it)->isCompleted() == false) {
            return false;
        }
    }
    return true;
}


std::string Container::getPhaseName() const
{
    // TODO Implement once phase interface is defined
    return std::string();
}
