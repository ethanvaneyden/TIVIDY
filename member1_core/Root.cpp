// Root.cpp
#include "Root.h"
#include "Container.h"
#include "DepthFirstIterator.h"
#include "Iterator.h"
#include "StateFilterIterator.h"
#include <stdexcept>
#include <algorithm>

Root::Root(std::string id, std::string type, std::string name, Component* parent)
    : Component(std::move(id), std::move(type), std::move(name), parent) {}

void Root::add(Component* c) {
    if (!c) throw std::invalid_argument("Cannot add a null component");
    if (!dynamic_cast<Container*>(c)) throw std::invalid_argument("Root can only hold Containers");
    if (c == this || c->getParent() != nullptr)
        throw std::invalid_argument("Component already belongs to a parent");
    for (Component* child : children) {
        if (child->getId() == c->getId())
            throw std::invalid_argument("Duplicate child ID: " + c->getId());
    }
    children.push_back(c);
    c->setParent(this);
}

void Root::remove(const std::string& id) {
    auto it = std::find_if(children.begin(), children.end(), [&id](Component* c){ return c->getId() == id; });
    if (it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
    }
}

std::vector<Component*> Root::getChildren() const { return children; }

bool Root::isCompleted() const {
    if (children.empty()) return false;
    for (auto* c : children) if (!c->isCompleted()) return false;
    return true;
}

Iterator* Root::createIterator() {
    return new DepthFirstIterator(this);
}

Iterator* Root::createIterator(const std::string& state) {
    return new StateFilterIterator(this, state);
}