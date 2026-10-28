// Root.h
#pragma once


#include "Component.h"
#include <vector>

/**
 * @brief The Root container (e.g., Module or Hospital).
 * 
 * Acts as the structural entry point. Has NO phase, only holds Containers.
 */
class Root : public Component {
private:
    std::vector<Component*> children; ///< Non-owning references.

public:
    /**
     * @brief Creates a root component.
     * @param id Unique component ID.
     * @param type Scenario-specific type.
     * @param name Display name.
     * @param parent Optional parent pointer; normally null.
     * @throws std::invalid_argument If ID or type is empty.
     */
    Root(std::string id, std::string type, std::string name, Component* parent = nullptr);

    /**
     * @brief Destroys the root without deleting registry-owned descendants.
     */
    ~Root() override = default;

    /**
     * @brief Adds a container to the root.
     * @param c Non-owning pointer to the child.
     * @throws std::invalid_argument If child is not a Container.
     * @throws std::invalid_argument If null, already parented, or duplicate ID.
     */
    void add(Component* c) override;
    
    /**
     * @brief Detaches a direct child by ID.
     * @param id Child ID.
     */
    void remove(const std::string& id) override;

    /**
     * @brief Returns direct non-owning child pointers.
     * @return Child components.
     */
    std::vector<Component*> getChildren() const override;

    /**
     * @brief Checks whether every child container is complete.
     * @return True when the non-empty root's containers are complete.
     */
    bool isCompleted() const override;

    /**
     * @brief Creates a depth-first iterator rooted at this component.
     * @return Caller-owned iterator.
     */
    Iterator* createIterator() override;

    /**
     * @brief Creates a state-filtered iterator.
     * @param state State name to match.
     * @return Caller-owned iterator.
     */
    Iterator* createIterator(const std::string& state) override;
};