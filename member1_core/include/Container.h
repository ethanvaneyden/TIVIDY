#ifndef CONTAINER_H
#define CONTAINER_H
#include "Component.h"
#include <vector>
#include <memory>

#include "WorkflowPhase.h"

/**
 * @brief A workflow component that groups and manages work items.
 *
 * A container holds non-owning references to its child components and tracks
 * the workflow phase shared by those children. Items may be added or removed
 * only during the Intake phase. The phase sequence is Intake, Active, Review,
 * and Publish.
 */
class Container : public Component {
private:
    std::vector<Component*> children;
    std::unique_ptr<WorkflowPhase> phase;

    /// @brief Replaces the current phase.
    /// @param next The phase to install; must not be null.
    void setPhase(std::unique_ptr<WorkflowPhase> next);
    friend class IntakePhase;
    friend class ActivePhase;
    friend class ReviewPhase;
    friend class PublishPhase;

public:
    /**
     * @brief Constructs a container in the Intake phase.
     * @param id Unique identifier for this component.
     * @param type Component type.
     * @param name Display name.
     * @param parent Optional non-owning reference to the parent component.
     */
    Container(std::string id, std::string type, std::string name, Component* parent = nullptr);

    /// @brief Destroys the container without destroying its child components.
    virtual ~Container() override;

    /**
     * @brief Adds an item to this container.
     * @param c Item to add; the container does not take ownership.
     * @throws std::invalid_argument If the item is null, is not an Item, already
     *         belongs to another container, or duplicates an existing child.
     * @throws InvalidTransition If the container is not in the Intake phase.
     */
    void add(Component* c) override;

    /**
     * @brief Removes a child item by identifier.
     * @param id Identifier of the item to remove.
     * @throws InvalidTransition If the container is not in the Intake phase.
     *
     * If no child has the given identifier, this operation has no effect.
     */
    void remove(const std::string& id) override;

    /// @brief Returns a copy of the list of non-owning child references.
    std::vector<Component*> getChildren() const override;

    /**
     * @brief Creates an iterator for traversing this container's children.
     * @return An iterator, or nullptr when traversal is unavailable.
     */
    Iterator* createIterator() override;

    /**
     * @brief Checks whether this container has children and all are completed.
     * @return true if at least one child exists and every child is completed.
     */
    bool isCompleted() const override;

    /**
     * @brief Advances the container to the next workflow phase.
     * @throws InvalidTransition If the phase transition is not permitted.
     *
     * A successful transition notifies the associated workflow mediator, when
     * one is available.
     */
    void advancePhase();

    /// @brief Returns the name of the current workflow phase.
    std::string getPhaseName() const;

    /**
     * @brief Checks whether all children are completed and reviewed.
     * @return true if the container has children and all are ready to publish.
     */
    bool isReadyToPublish() const;

    /**
     * @brief Attempts to advance a ready container from Review to Publish.
     * @return true if publishing was started; otherwise false without changing
     *         the phase.
     */
    bool tryPublish();
};

#endif
