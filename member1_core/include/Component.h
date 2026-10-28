#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <vector>
#include "Colleague.h"

class Iterator;

/**
 * @brief Abstract base for components in the workflow tree.
 *
 * A component has an identifier, type, name, optional non-owning parent, and
 * optional workflow mediator inherited from Colleague. Leaf implementations
 * provide their own iterator and completion behavior.
 */
class Component : public Colleague
{
protected:
    std::string id;
    std::string type;
    std::string name;

    /// @brief Non owning reference
    Component *parent;

public:
    /**
     * @brief Constructs a component.
     * @param id Component identifier.
     * @param type Component type.
     * @param name Display name.
     * @param parent Optional non-owning parent component.
     */
    Component(std::string id, std::string type, std::string name, Component *parent = nullptr);

    /// @brief Destroys the component.
    virtual ~Component() = default;

    /**
     * @brief Adds a child component.
     * @param c Child to add.
     * @throws std::logic_error The base implementation rejects adding children
     *         to a leaf component.
     */
    virtual void add(Component *c);

    /**
     * @brief Removes a child component by identifier.
     * @param id Identifier of the child to remove.
     * @throws std::logic_error The base implementation rejects removing
     *         children from a leaf component.
     */
    virtual void remove(const std::string &id);

    /// @brief Returns the component's children.
    /// @return An empty vector in the base implementation.
    virtual std::vector<Component *> getChildren() const;

    /// @brief Creates an iterator for traversing this component.
    virtual Iterator *createIterator() = 0;

    /// @brief Checks whether the component's work is complete.
    virtual bool isCompleted() const = 0;

    /// @brief Returns the component identifier.
    const std::string &getId() const;

    /// @brief Returns the component type.
    const std::string &getType() const;

    /// @brief Returns the component display name.
    const std::string &getName() const;

    /// @brief Returns the component state name, if the component has a state.
    virtual std::string getStateName() const { return ""; }

    /// @brief Returns the non-owning parent component, or nullptr if detached.
    Component *getParent() const;

    /// @brief Sets the non-owning parent component.
    /// @param p Parent component, or nullptr to detach.
    void setParent(Component *p);
};

#endif