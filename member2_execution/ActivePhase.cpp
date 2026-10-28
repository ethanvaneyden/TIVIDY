#include "ActivePhase.h"
#include "ReviewPhase.h"
#include "Container.h"
#include "InvalidTransition.h"

std::unique_ptr<WorkflowPhase> ActivePhase::advance(Container& container)
{
    if (!container.isCompleted())
        throw InvalidTransition("All items must have completed work before review");
    return std::unique_ptr<WorkflowPhase>(new ReviewPhase());
}

std::string ActivePhase::name() const { return "Active"; }
