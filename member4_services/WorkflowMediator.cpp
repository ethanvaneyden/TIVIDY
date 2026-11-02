/**
 * @file WorkflowMediator.cpp
 * @brief Default implementations of the optional Mediator capabilities.
 */
#include "WorkflowMediator.h"

#include "AssignmentStrategy.h"
#include "InvalidTransition.h"

int WorkflowMediator::onContainerOpenRequested(Container*)
{
    throw InvalidTransition("This workflow mediator cannot open containers");
}

bool WorkflowMediator::setAssignmentStrategy(std::unique_ptr<AssignmentStrategy>&)
{
    return false;
}

std::string WorkflowMediator::assignmentStrategyName() const
{
    return "None";
}

ActionHistory* WorkflowMediator::getActionHistory()
{
    return nullptr;
}
