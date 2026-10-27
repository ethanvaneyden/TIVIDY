#include "IntakePhase.h"
#include "ActivePhase.h"
#include "Container.h"
#include "InvalidTransition.h"
#include <memory>

void IntakePhase::advance(Container& container)
{
    if (container.getChildren().empty())
        throw InvalidTransition("Cannot activate an empty container");
    container.setPhase(std::unique_ptr<WorkflowPhase>(new ActivePhase()));
}

std::string IntakePhase::name() const { return "Intake"; }
