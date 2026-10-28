#include "Container.h"
#include "Item.h"
#include "DepthFirstIterator.h"
#include "IntakePhase.h"
#include "InvalidTransition.h"
#include "LifecycleEvents.h"
#include "WorkflowMediator.h"

#include <stdexcept>
#include <utility>

Container::Container(std::string id, std::string type, std::string name, Component* parent)
    : Component(id, type, name, parent), phase(new IntakePhase())
{
}

Container::~Container() = default;

void Container::add(Component* c)
{
    if (!c) throw std::invalid_argument("A child item is required");
    if (getPhaseName() != "Intake")
        throw InvalidTransition("Items can only be added during Intake");
    if (!dynamic_cast<Item*>(c))
        throw std::invalid_argument("The workflow structure is Root -> Container -> Item");
    if (c->getParent() && c->getParent() != this)
        throw std::invalid_argument("This item already belongs to another container");
    for (const Component* child : children)
        if (child == c || child->getId() == c->getId())
            throw std::invalid_argument("Duplicate item in container");
    children.push_back(c);
    c->setParent(this);
}

void Container::remove(const std::string& id)
{
    if (getPhaseName() != "Intake")
        throw InvalidTransition("Items can only be removed during Intake");
    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->getId() == id)
        {
            (*it)->setParent(nullptr);
            children.erase(it);
            return;
        }
    }
}

std::vector<Component*> Container::getChildren() const { return children; }

Iterator* Container::createIterator()
{
    return new DepthFirstIterator(this);
}

bool Container::isCompleted() const
{
    if (children.empty()) return false;
    for (const Component* child : children)
        if (!child->isCompleted()) return false;
    return true;
}

void Container::setPhase(std::unique_ptr<WorkflowPhase> next)
{
    if (!next) throw std::invalid_argument("A container phase cannot be null");
    phase = std::move(next);
}

void Container::advancePhase()
{
    // Keep the old phase alive while advance installs its successor.
    std::unique_ptr<WorkflowPhase> current = std::move(phase);
    try
    {
        current->advance(*this);
    }
    catch (...)
    {
        if (!phase) phase = std::move(current);
        throw;
    }
    if (!phase) phase = std::move(current);
    WorkflowMediator* target = getMediator();
    if (!target && parent) target = parent->getMediator();
    if (target) target->notify(this, LifecycleEvents::PhaseChanged);
}

std::string Container::getPhaseName() const { return phase->name(); }

bool Container::isReadyToPublish() const
{
    if (!isCompleted()) return false;
    for (const Component* child : children)
    {
        const Item* item = dynamic_cast<const Item*>(child);
        if (!item || !item->isReviewed()) return false;
    }
    return true;
}

bool Container::tryPublish()
{
    if (getPhaseName() != "Review" || !isReadyToPublish()) return false;
    advancePhase();
    return true;
}
