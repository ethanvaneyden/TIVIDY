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
    Root(std::string id, std::string type, std::string name, Component* parent = nullptr);
    ~Root() override = default;

    /**
     * @brief Adds a container to the root.
     * @throws std::invalid_argument If child is not a Container.
     */
    void add(Component* c) override;
    
    void remove(const std::string& id) override;
    std::vector<Component*> getChildren() const override;
    bool isCompleted() const override;
    Iterator* createIterator() override;
};