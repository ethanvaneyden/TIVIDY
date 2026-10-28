#include "Root.h"
#include "Container.h"
#include "DepthFirstIterator.h"
#include <stdexcept>

Root::Root(std::string id, std::string type, std::string name, Component *parent)
    : Component(id, type, name, parent)
{
}

void Root::add(Component *c)
{
    if (!c)
        throw std::invalid_argument("A child container is required");
    if (!dynamic_cast<Container*>(c))
        throw std::invalid_argument("The workflow structure is Root -> Container -> Item");
    if (c->getParent() && c->getParent() != this)
        throw std::invalid_argument("This container already belongs to another root");
    for (const Component* child : children)
        if (child == c || child->getId() == c->getId())
            throw std::invalid_argument("Duplicate container in root");
    children.push_back(c);
    c->setParent(this);
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
    return children;
}

bool Root::isCompleted() const
{
    if (children.empty())
        return false;
    for (const Component *child : children)
        if (!child->isCompleted())
            return false;
    return true;
}

Iterator *Root::createIterator()
{
    return new DepthFirstIterator(this);
}
