// Container.h
#pragma once

#include "Component.h"
#include <vector>
#include <memory>

class WorkflowPhase;

/**
 * @brief Holds work items and manages the 4-phase lifecycle.
 */
class Container : public Component {
private:
    std::vector<Component*> children;          ///< Non-owning items.
    std::unique_ptr<WorkflowPhase> phase;      ///< Owned phase state.

public:
    /**
     * @brief Creates a container with an initial intake phase.
     * @param id Unique component ID.
     * @param type Scenario-specific type.
     * @param name Display name.
     * @param parent Optional non-owning parent pointer.
     * @throws std::invalid_argument If ID or type is empty.
     */
    Container(std::string id, std::string type, std::string name, Component* parent = nullptr);

    /**
     * @brief Destroys the container and its owned phase object.
     */
    ~Container() override;

    /**
     * @brief Adds an item to this container.
     * @param c Non-owning pointer to the child.
     * @throws std::invalid_argument If child is a Container (no deep nesting).
     * @throws std::invalid_argument If null, already parented, or duplicate ID.
     */
    void add(Component* c) override;

    /**
     * @brief Detaches a direct child by ID.
     * @param id Child ID.
     */
    void remove(const std::string& id) override;

    /**
     * @brief Returns direct non-owning children.
     * @return Child pointers.
     */
    std::vector<Component*> getChildren() const override;

    /**
     * @brief Checks whether every child is complete.
     * @return True when the non-empty container's children are complete.
     */
    bool isCompleted() const override;

    /**
     * @brief Checks whether the container is eligible for publishing.
     * @return True when every child is complete and reviewed.
     */
    bool isReadyToPublish() const;

    /**
     * @brief Creates a depth-first iterator rooted at this container.
     * @return Caller-owned iterator, or nullptr if empty.
     */
    Iterator* createIterator() override;

    /**
     * @brief Creates an iterator filtered by item state.
     * @param state State name to match.
     * @return Caller-owned iterator.
     */
    Iterator* createIterator(const std::string& state) override;

    /**
     * @brief Advances to the next workflow phase.
     */
    void advancePhase();

    /**
     * @brief Returns the current phase name.
     * @return Phase name.
     */
    std::string getPhaseName() const;

    /**
     * @brief Replaces the owned phase.
     * @param next Owning pointer to the next phase.
     * @throws std::invalid_argument If next is null.
     */
    void setPhase(std::unique_ptr<WorkflowPhase> next);
};