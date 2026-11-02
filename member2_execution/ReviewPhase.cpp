/** @file ReviewPhase.cpp
* @brief Implements the lifecycle transitions declared in ReviewPhase.h.
* @author Member 2
* @date October 2026
*/
#include "ReviewPhase.h"
#include "PublishPhase.h"
#include "Container.h"
#include "InvalidTransition.h"

std::unique_ptr<WorkflowPhase> ReviewPhase::advance(Container& container)
{
    if (!container.isReadyToPublish())
        throw InvalidTransition("All items must be completed and reviewed before publishing");
    return std::unique_ptr<WorkflowPhase>(new PublishPhase());
}

std::string ReviewPhase::name() const { return "Review"; }
