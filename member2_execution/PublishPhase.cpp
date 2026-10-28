#include "PublishPhase.h"

std::unique_ptr<WorkflowPhase> PublishPhase::advance(Container&)
{
    return nullptr;
}

std::string PublishPhase::name() const { return "Publish"; }
