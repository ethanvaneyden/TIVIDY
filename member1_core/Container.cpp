// Container.cpp
#include "Container.h"
#include "DepthFirstIterator.h"
#include "IntakePhase.h"
#include "InvalidTransition.h"
#include "Item.h"
#include "StateFilterIterator.h"
#include "WorkflowPhase.h" // Member 2
#include <algorithm>
#include <stdexcept>

Container::Container(std::string id, std::string type, std::string name)
    : Component(std::move(id), std::move(type), std::move(name)),
      phase(std::unique_ptr<WorkflowPhase>(new IntakePhase())) {}

Container::~Container()
{
    for (std::vector<Component*>::iterator child = children.begin();
         child != children.end(); ++child)
    {
        (*child)->setParent(nullptr);
    }
}

void Container::add(Component* c) {
    if (!c) throw std::invalid_argument("Cannot add a null component");
    if (getPhaseName() != "Intake")
        throw InvalidTransition("Items can only be added during the Intake phase");
    if (!dynamic_cast<Item*>(c))
        throw std::invalid_argument("A container can only hold items");
    if (c == this || c->getParent() != nullptr)
        throw std::invalid_argument("Component already belongs to a parent");
    for (Component* child : children) {
        if (child->getId() == c->getId())
            throw std::invalid_argument("Duplicate child ID: " + c->getId());
    }
    children.push_back(c);
    c->setParent(this);
}

void Container::remove(const std::string& id) {
    auto it = std::find_if(children.begin(), children.end(), [&id](Component* c){ return c->getId() == id; });
    if (it != children.end()) {
        if (getPhaseName() != "Intake")
            throw InvalidTransition("Items can only be removed during the Intake phase");
        (*it)->setParent(nullptr);
        children.erase(it);
    }
}

std::vector<Component*> Container::getChildren() const { return children; }

bool Container::isCompleted() const {
    if (children.empty()) return false;
    for (auto* c : children) if (!c->isCompleted()) return false;
    return true;
}

bool Container::isReadyToPublish() const {
    if (!isCompleted()) return false;
    for (const Component* child : children) {
        const Item* item = dynamic_cast<const Item*>(child);
        if (!item || !item->isReviewed()) return false;
    }
    return true;
}

void Container::setPhase(std::unique_ptr<WorkflowPhase> next)
{
    if (!next)
        throw std::invalid_argument("Container phase cannot be null");
    phase = std::move(next);
}
std::string Container::getPhaseName() const { return phase ? phase->name() : "Unknown"; }
void Container::advancePhase() {
    if (!phase)
        throw std::logic_error("Container has no workflow phase");

    std::unique_ptr<WorkflowPhase> next = phase->advance(*this);
    if (next)
        phase = std::move(next);
}
Iterator* Container::createIterator() { return new DepthFirstIterator(this); }

Iterator* Container::createIterator(const std::string& state) {
    return new StateFilterIterator(this, state);
}