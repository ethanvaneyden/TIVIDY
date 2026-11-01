// Component.h
#pragma once

#include <string>
#include <vector>
#include "Colleague.h"

class Iterator;
class Container;
class Root;

/**
 * @brief Abstract base for all nodes in the workflow tree.
 * 
 * Component role in the Composite pattern. Inherits from Colleague 
 * to receive Mediator events.
 */
class Component : public Colleague {
private:
    std::string id;             ///< Unique identifier.
    std::string type;           ///< Type (e.g., "Submission", "Assignment").
    std::string name;           ///< Display name.
    Component* parent;          ///< Non-owning parent reference.

    friend class Container;
    friend class Root;
    void setParent(Component* p);

public:
    /**
     * @brief Constructs a component.
     * @param id Unique component ID.
     * @param type Scenario-specific component type.
     * @param name Display name.
     * @throws std::invalid_argument If the ID or type is empty.
     */
    Component(std::string id, std::string type, std::string name);
    
    /**
     * @brief Destroys the component without deleting its parent or children.
     */
    virtual ~Component() = default;

    /**
     * @brief Adds a child component.
     * @param c Child pointer.
     * @throws std::logic_error Leaves cannot have children.
     */
    virtual void add(Component* c);

    /**
     * @brief Removes a child component.
     * @param id ID of the child to remove.
     * @throws std::logic_error Leaves cannot have children.
     */
    virtual void remove(const std::string& id);

    /**
     * @brief Returns non-owning child pointers.
     * @return Child components, or an empty vector for a leaf.
     */
    virtual std::vector<Component*> getChildren() const;

    /**
     * @brief Creates an iterator for this component.
     * @return Caller-owned iterator pointer.
     */
    virtual Iterator* createIterator() = 0;

    /**
     * @brief Creates an iterator filtered by item state.
     * @param state State name to match.
     * @return Caller-owned iterator pointer.
     */
    virtual Iterator* createIterator(const std::string& state) = 0;

    /**
     * @brief Checks whether this component is complete.
     * @return True when complete.
     */
    virtual bool isCompleted() const = 0;

    /**
     * @brief Returns the component ID.
     * @return ID reference.
     */
    const std::string& getId() const;

    /**
     * @brief Returns the scenario-specific type.
     * @return Type reference.
     */
    const std::string& getType() const;

    /**
     * @brief Returns the display name.
     * @return Name reference.
     */
    const std::string& getName() const;
    
    /**
     * @brief Returns the lifecycle state name.
     * @return State name, or an empty string for non-items.
     */
    virtual std::string getStateName() const { return ""; }
    
    /**
     * @brief Returns the non-owning parent pointer.
     * @return Parent, or nullptr for a root component.
     */
    Component* getParent() const;

    /**
     * @brief Sets the mediator for this component and all its children.
     * @param mediator Non-owning pointer to the mediator.
     */
    void setMediator(WorkflowMediator* mediator);
};