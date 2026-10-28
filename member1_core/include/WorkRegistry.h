#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "Component.h"

/**
 * @brief Owns and indexes workflow components by identifier.
 *
 * The registry takes ownership of components passed to add(). Components
 * returned from lookup and insertion are non-owning pointers. Removing a
 * component also removes its registered descendants and detaches it from its
 * parent when applicable.
 */
class WorkRegistry
{
private:
    std::unordered_map<std::string, std::unique_ptr<Component>> components;

public:
    /// @brief Constructs an empty component registry.
    WorkRegistry();

    /// @brief Destroys all components owned by the registry.
    ~WorkRegistry();

    /**
     * @brief Registers and takes ownership of a component.
     * @param item Component to register.
     * @return Non-owning pointer to the registered component.
     * @throws std::invalid_argument If item is null or its identifier is
     *         already registered.
     */
    Component *add(std::unique_ptr<Component> item);

    /**
     * @brief Looks up a component by identifier.
     * @param id Component identifier.
     * @return Non-owning pointer to the component, or nullptr if not found.
     */
    Component *get(const std::string &id) const;

    /**
     * @brief Removes a component and any registered descendants.
     * @param id Identifier of the component to remove.
     * @throws InvalidTransition If a child cannot be removed in its current
     *         container phase.
     *
     * Has no effect if the identifier is not registered. Components are
     * detached from their parents before destruction.
     */
    void remove(const std::string &id);

    /// @brief Checks whether an identifier is registered.
    bool contains(const std::string &id) const;

    /// @brief Returns the number of registered components.
    size_t size() const;

    /// @brief Returns all registered component identifiers.
    std::vector<std::string> getAllIds() const;
};
