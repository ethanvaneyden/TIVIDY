#ifndef ROOT_H
#define ROOT_H
#include "Component.h"
#include <vector>
#include <memory>

/**
 * @brief Top-level component for a workflow component tree.
 *
 * The root stores non-owning references to its children. Adding a child sets
 * its parent to this root; removing a child detaches it.
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
     * @brief Adds a child component and assigns its parent to this root.
     * @param c Child to add; null pointers are ignored.
     */
    void add(Component* c) override;

    /**
     * @brief Removes and detaches the child with the given identifier.
     * @param id Identifier of the child to remove.
     *
     * Has no effect if no child has the identifier.
     */
    void remove(const std::string& id) override;

    /**
     * @brief Returns the root's children.
     * @return An empty vector in the current implementation.
     */
    std::vector<Component*> getChildren() const override;

    /**
     * @brief Creates an iterator for traversing the root.
     * @return nullptr in the current implementation.
     */
    Iterator* createIterator() override;
};

#endif