// Root.cpp
#include "Root.h"
#include "Container.h"
#include "Iterator.h"
#include <stdexcept>
#include <algorithm>

Root::Root(std::string id, std::string type, std::string name, Component* parent)
    : Component(std::move(id), std::move(type), std::move(name), parent) {}

void Root::add(Component* c) {
    if (!c) return;
    if (!dynamic_cast<Container*>(c)) throw std::invalid_argument("Root can only hold Containers");
    c->setParent(this);
    children.push_back(c);
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
    return nullptr; // Member 3 will implement this
}