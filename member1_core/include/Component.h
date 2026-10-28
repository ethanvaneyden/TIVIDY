// Component.h
#pragma once

#include <string>
#include <vector>
#include "Colleague.h"

class Iterator;

/**
 * @brief Abstract base for all nodes in the workflow tree.
 * 
 * Component role in the Composite pattern. Inherits from Colleague 
 * to receive Mediator events.
 */
class Component : public Colleague {
protected:
    std::string id;             ///< Unique identifier.
    std::string type;           ///< Type (e.g., "Submission", "Assignment").
    std::string name;           ///< Display name.
    Component* parent;          ///< Non-owning parent reference.

public:
    /**
     * @brief Constructs a component.
     */
    Component(std::string id, std::string type, std::string name, Component* parent = nullptr);
    
    /// @brief Virtual destructor.
    virtual ~Component() = default;

    /**
     * @brief Adds a child component.
     * @throws std::logic_error Leaves cannot have children.
     */
    virtual void add(Component* c);

    /**
     * @brief Removes a child component.
     * @throws std::logic_error Leaves cannot have children.
     */
    virtual void remove(const std::string& id);

    /// @brief Returns child components (empty for leaves).
    virtual std::vector<Component*> getChildren() const;

    /// @brief Creates an iterator for this component.
    virtual Iterator* createIterator() = 0;

    /// @brief Checks if the component's work is done.
    virtual bool isCompleted() const = 0;

    const std::string& getId() const;   ///< @return Component ID.
    const std::string& getType() const; ///< @return Component type.
    const std::string& getName() const; ///< @return Component display name.
    
    /// @brief Returns the state name (empty for non-items).
    virtual std::string getStateName() const { return ""; }
    
    Component* getParent() const;       ///< @return Non-owning parent.
    void setParent(Component* p);       ///< @param p New parent.

    /**
     * @brief Sets the mediator for this component and all its children.
     * @param mediator Non-owning pointer to the mediator.
     */
    void setMediator(WorkflowMediator* mediator);
};