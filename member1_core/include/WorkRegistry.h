// WorkRegistry.h
#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "Component.h"

/**
 * @brief The single owner of all components in a WorkflowInstance.
 * 
 * Uses std::unique_ptr to guarantee memory safety. Returns non-owning 
 * raw pointers to the rest of the system.
 */
class WorkRegistry {
private:
    /// @brief The actual storage mapping ID to owned Component.
    std::unordered_map<std::string, std::unique_ptr<Component>> components;

public:
    WorkRegistry();
    ~WorkRegistry();

    /**
     * @brief Takes ownership of a component.
     * @param item The unique_ptr transferring ownership.
     * @return Non-owning raw pointer for tree wiring.
     * @throws std::invalid_argument If null or duplicate ID.
     */
    Component* add(std::unique_ptr<Component> item);

    /**
     * @brief Looks up a component.
     * @return Non-owning pointer, or nullptr if not found.
     */
    Component* get(const std::string& id) const;

    /**
     * @brief Recursively removes a component and all its children.
     * @param id The ID of the component to remove.
     */
    void remove(const std::string& id);

    bool contains(const std::string& id) const; ///< @return True if exists.
    size_t size() const;                        ///< @return Number of items.
    std::vector<std::string> getAllIds() const; ///< @return All IDs.
};