/** @file IntakePhase.cpp
* @brief Implements the lifecycle transitions declared in IntakePhase.h.
* @author Member 2
* @date October 2026
*/
#include "IntakePhase.h"
#include "ActivePhase.h"
#include "Container.h"
#include "InvalidTransition.h"

std::unique_ptr<WorkflowPhase> IntakePhase::advance(Container& container)
{
    if (container.getChildren().empty())
        throw InvalidTransition("Cannot activate an empty container");
    return std::unique_ptr<WorkflowPhase>(new ActivePhase());
}

std::string IntakePhase::name() const { return "Intake"; }
