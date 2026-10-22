#include "Component.h"
#include <vector>
#include <memory>

class WorkflowPhase;
class Composite : public Component
{
private:
    std::vector<Component *> children;
    std::unique_ptr<WorkflowPhase> phase;

public:
    void add(Component *c) override;
    void remove(const std::string &id) override;
    std::vector<Component *> getChildren() override;

    void advancePhase();
    void approve();
    void reject();
    bool allItemsComplete();
    const std::string &getPhaseName() const;
};