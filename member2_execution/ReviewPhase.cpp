#include "ReviewPhase.h"
#include "PublishPhase.h"
#include "Container.h"
#include "InvalidTransition.h"
#include <memory>

void ReviewPhase::advance(Container& container)
{
    if (!container.isReadyToPublish())
        throw InvalidTransition("All items must be completed and reviewed before publishing");
    container.setPhase(std::unique_ptr<WorkflowPhase>(new PublishPhase()));
}

std::string ReviewPhase::name() const { return "Review"; }
