// WorkRegistry.cpp
#include "WorkRegistry.h"
#include <stdexcept>

WorkRegistry::WorkRegistry() = default;
WorkRegistry::~WorkRegistry() = default;

Component* WorkRegistry::add(std::unique_ptr<Component> item) {
    if (!item) throw std::invalid_argument("Cannot register null component");
    std::string id = item->getId();
    if (components.count(id)) throw std::invalid_argument("Duplicate ID: " + id);
    
    Component* raw = item.get();
    components.emplace(id, std::move(item));
    return raw;
}

Component* WorkRegistry::get(const std::string& id) const {
    auto it = components.find(id);
    return it != components.end() ? it->second.get() : nullptr;
}

void WorkRegistry::remove(const std::string& id) {
    auto it = components.find(id);
    if (it == components.end()) return;

    // Detach from parent
    if (it->second->getParent()) {
        it->second->getParent()->remove(id);
    }

    // Recursively remove children
    auto children = it->second->getChildren();
    for (auto* child : children) {
        remove(child->getId());
    }

    components.erase(it);
}

bool WorkRegistry::contains(const std::string& id) const {
    return components.count(id) > 0;
}

size_t WorkRegistry::size() const { return components.size(); }

std::vector<std::string> WorkRegistry::getAllIds() const {
    std::vector<std::string> ids;
    for (const auto& pair : components) ids.push_back(pair.first);
    return ids;
}