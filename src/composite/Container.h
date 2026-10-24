#include "Component.h"
#include <vector>
#include <memory>

class WorkflowPhase;
class Container : public Component
{
private:
    std::vector<Component *> children;
    std::unique_ptr<WorkflowPhase> phase;

public:
    Container();
    virtual ~Container() = default;

    void add(Component *c) override;
    void remove(const std::string &id) override;
    std::vector<Component *> getChildren() override;
    Iterator *createIterator() override;

    void advancePhase();
    void approve();
    void reject();
    bool allItemsComplete();
    const std::string &getPhaseName() const;
};