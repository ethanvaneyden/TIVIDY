#pragma once
#include "Component.h"
#include <vector>
#include <memory>

/**
 * @brief Top-level component for a workflow component tree.
 *
 * The root tracks Container children. Raw-pointer children are non-owning;
 * children added with unique_ptr are owned by the root. Adding a child sets
 * its parent to this root; removing a child detaches it.
 */
class Root : public Component {
private:
    std::vector<Component*> children;
    std::vector<std::unique_ptr<Component>> ownedChildren;

public:
    /**
     * @brief Constructs a root component.
     * @param id Component identifier.
     * @param type Component type.
     * @param name Display name.
     * @param parent Optional non-owning parent component.
     */
    Root(std::string id, std::string type, std::string name, Component* parent = nullptr);

    /// @brief Destroys the root and any children it owns.
    virtual ~Root() = default;

    /**
     * @brief Adds a container as a child and assigns its parent to this root.
     * @param c Container to add.
     * @throws std::invalid_argument If c is null, is not a Container, already
     *         belongs to another parent, or duplicates an existing child.
     */
    void add(Component* c) override;

    /**
     * @brief Adds and takes ownership of a child container.
     * @param c Owned child container.
     */
    void add(std::unique_ptr<Component> c);

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
