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
    Container(std::string id, std::string type, std::string name, Component* parent = nullptr);
    ~Container() override = default;

    /**
     * @brief Adds an item to this container.
     * @throws std::invalid_argument If child is a Container (no deep nesting).
     */
    void add(Component* c) override;
    void remove(const std::string& id) override;
    std::vector<Component*> getChildren() const override;
    bool isCompleted() const override;
    bool isReadyToPublish() const;
    Iterator* createIterator() override;

    void advancePhase();
    std::string getPhaseName() const;
    void setPhase(std::unique_ptr<WorkflowPhase> next);
};