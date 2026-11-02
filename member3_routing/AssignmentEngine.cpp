#include "AssignmentEngine.h"
#include "Participant.h"
#include "Item.h"
#include "WorkflowMediator.h"
#include <utility>

AssignmentEngine::AssignmentEngine(std::unique_ptr<AssignmentStrategy> s) : strategy(std::move(s))
{
}

void AssignmentEngine::setStrategy(std::unique_ptr<AssignmentStrategy> s)
{
    strategy = std::move(s);
}
std::string AssignmentEngine::strategyName() const
{
    if (strategy != nullptr)
        return strategy->name();

    return "NONE";
}

Participant *AssignmentEngine::assign(Item &item, const std::vector<Participant *> &pool)
{
    std::vector<Participant *> clean;
    for (Participant *p : pool)
    {
        if (p != nullptr)
            clean.push_back(p);
    }

    Participant *assignee = strategy ? strategy->assign(clean) : nullptr;
    if (assignee == nullptr)
    {
        WorkflowMediator *workflowMediator = item.getMediator();
        if (workflowMediator != nullptr)
            workflowMediator->onAssignmentFailed(&item, "No worker is available");
    }
    return assignee;
}