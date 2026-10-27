#ifndef CONTAINER_H
#define CONTAINER_H
#include "Component.h"
#include <vector>
#include <memory>

#include "WorkflowPhase.h"

class Container : public Component {
private:
    std::vector<Component*> children;
    std::unique_ptr<WorkflowPhase> phase;

    void setPhase(std::unique_ptr<WorkflowPhase> next);
    friend class IntakePhase;
    friend class ActivePhase;
    friend class ReviewPhase;
    friend class PublishPhase;

public:
    Container(std::string id, std::string type, std::string name, Component* parent = nullptr);
    virtual ~Container() override;

    void add(Component* c) override;
    void remove(const std::string& id) override;
    std::vector<Component*> getChildren() const override;
    Iterator* createIterator() override;
    bool isCompleted() const override;

    void advancePhase();
    std::string getPhaseName() const;
    bool isReadyToPublish() const;
    // Called by the mediator after batch approval or personal review.
    // Returns false without changing anything while review is incomplete.
    bool tryPublish();
};

#endif
