// Component.cpp
#include "Component.h"
#include <stdexcept>
#include <utility>

Component::Component(std::string id, std::string type, std::string name, Component* parent)
    : id(std::move(id)), type(std::move(type)), name(std::move(name)), parent(parent) {
    if (this->id.empty() || this->type.empty())
        throw std::invalid_argument("Component ID and type must not be empty");
}

void Component::add(Component*) {
    throw std::logic_error("Cannot add children to a leaf component.");
}

void Component::remove(const std::string&) {
    throw std::logic_error("Cannot remove children from a leaf component.");
}

std::vector<Component*> Component::getChildren() const {
    return {};
}

const std::string& Component::getId() const { return id; }
const std::string& Component::getType() const { return type; }
const std::string& Component::getName() const { return name; }
Component* Component::getParent() const { return parent; }
void Component::setParent(Component* p) { parent = p; }

void Component::setMediator(WorkflowMediator* m) {
    Colleague::setMediator(m);
    for (auto* child : getChildren()) {
        if (child) child->setMediator(m);
    }
}