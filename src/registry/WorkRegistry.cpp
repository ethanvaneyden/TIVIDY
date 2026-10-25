#include "registry/WorkRegistry.h"
#include <stdexcept>

Component *WorkRegistry::add(std::unique_ptr<Component> item)
{
    if (!item)
    {
        throw std::invalid_argument("Cannot register null component");
    }
    std::string id = item->getId();
    if (components.count(id) > 0)
    {
        throw std::invalid_argument("Duplicate id inserted");
    }
    Component *raw = item.get();
    components.emplace(id, std::move(item));
    return raw;
}

Component *WorkRegistry::get(const std::string &id) const
{
    auto pair = components.find(id);
    if (pair == components.end())
    {
        return nullptr;
    }
    return pair->second.get();
}

void WorkRegistry::remove(const std::string &id)
{
    auto pair = components.find(id);
    if (pair == components.end())
        return;

    Component *component = pair->second.get();

    auto children = component->getChildren();
    for (auto *child : children)
    {
        remove(child->getId());
    }

    if (component->getParent())
    {
        component->getParent()->remove(id);
    }

    components.erase(id);
}

bool WorkRegistry::contains(const std::string &id) const
{
    return components.count(id) > 0;
}

size_t WorkRegistry::size() const
{
    return components.size();
}

std::vector<std::string> WorkRegistry::getAllIds() const
{
    std::vector<std::string> ids = {};
    for (auto &pair : components)
    {
        ids.push_back(pair.first);
    }
    return ids;
}
