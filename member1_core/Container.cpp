// Container.cpp
#include "Container.h"
#include "DepthFirstIterator.h"
#include "IntakePhase.h"
#include "Item.h"
#include "StateFilterIterator.h"
#include "WorkflowPhase.h" // Member 2
#include <algorithm>
#include <stdexcept>

Container::Container(std::string id, std::string type, std::string name, Component* parent)
    : Component(std::move(id), std::move(type), std::move(name), parent),
      phase(std::unique_ptr<WorkflowPhase>(new IntakePhase())) {}

Container::~Container() = default;

void Container::add(Component* c) {
    if (!c) throw std::invalid_argument("Cannot add a null component");
    if (dynamic_cast<Container*>(c)) throw std::invalid_argument("Cannot nest containers");
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

void Container::setPhase(std::unique_ptr<WorkflowPhase> next) { phase = std::move(next); }
std::string Container::getPhaseName() const { return phase ? phase->name() : "Unknown"; }
void Container::advancePhase() {
    if (!phase) return;

    std::unique_ptr<WorkflowPhase> next = phase->advance(*this);
    if (next)
        phase = std::move(next);
}
Iterator* Container::createIterator() { return new DepthFirstIterator(this); }

Iterator* Container::createIterator(const std::string& state) {
    return new StateFilterIterator(this, state);
}