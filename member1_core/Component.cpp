#include "Component.h"
#include "WorkflowMediator.h"
#include <stdexcept>

Component::Component(std::string id, std::string type, std::string name, Component *parent) : id(id), type(type), name(name), parent(parent)
{

}

void Component::add(Component *c)
{
    throw std::logic_error{"Unable to add a component to a leaf"};
}

void Component::remove(const std::string &id)
{
    throw std::logic_error{"Unable to remove a component from leaf"};
}

std::vector<Component *> Component::getChildren() const
{
    return {};
}

const std::string &Component::getId() const
{
    return id;
}

const std::string &Component::getType() const
{
    return type;
}

const std::string &Component::getName() const
{
    return name;
}

Component *Component::getParent() const
{
    return parent;
}

void Component::setParent(Component *p)
{
    parent = p;
}

void Component::setMediator(WorkflowMediator *workflowMediator)
{
    Colleague::setMediator(workflowMediator);
    const std::vector<Component*> children = getChildren();
    for (std::vector<Component*>::const_iterator child = children.begin();
         child != children.end(); ++child)
    {
        if (*child != nullptr)
            (*child)->setMediator(workflowMediator);
    }
}
