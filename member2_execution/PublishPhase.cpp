#include "PublishPhase.h"
#include "Container.h"
#include "InvalidTransition.h"
#include <memory>

void PublishPhase::advance(Container& )
{
    throw InvalidTransition("Publish is the final phase");
}

std::string PublishPhase::name() const { return "Publish"; }
