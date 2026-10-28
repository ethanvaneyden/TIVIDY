#ifndef ROOT_H
#define ROOT_H
#include "Component.h"
#include <vector>
#include <memory>

/**
 * @brief Top-level component for a workflow component tree.
 *
 * The root stores non-owning references to Container children. Adding a child
 * sets its parent to this root; removing a child detaches it.
 */
class Root : public Component {
private:
    std::vector<Component*> children;

public:
    /**
     * @brief Constructs a root component.
     * @param id Component identifier.
     * @param type Component type.
     * @param name Display name.
     * @param parent Optional non-owning parent component.
     */
    Root(std::string id, std::string type, std::string name, Component* parent = nullptr);

    /// @brief Destroys the root without destroying its children.
    virtual ~Root() = default;

    /**
     * @brief Adds a container as a child and assigns its parent to this root.
     * @param c Container to add.
     * @throws std::invalid_argument If c is null, is not a Container, already
     *         belongs to another parent, or duplicates an existing child.
     */
    void add(Component* c) override;

    /**
     * @brief Removes and detaches the child with the given identifier.
     * @param id Identifier of the child to remove.
     *
     * Has no effect if no child has the identifier.
     */
    void remove(const std::string& id) override;

    /// @brief Returns a copy of the root's non-owning child references.
    std::vector<Component*> getChildren() const override;

    /**
     * @brief Checks whether the root has children and all are completed.
     * @return true if at least one child exists and every child is completed.
     */
    bool isCompleted() const override;

    /**
     * @brief Creates a depth-first iterator for the root and its descendants.
     * @return Owning pointer to the iterator; the caller must delete it.
     */
    Iterator* createIterator() override;
};

#endif