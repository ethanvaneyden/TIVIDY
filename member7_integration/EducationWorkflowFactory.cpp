/**
 * @file EducationWorkflowFactory.cpp
 * @brief Implementation of the Education scenario factory.
*/
#include "EducationWorkflowFactory.h"

#include "AssignmentMarkingBuilder.h"
#include "AssignmentMarkingCoordinator.h"
#include "AssignmentMarkingDirector.h"
#include "EducationPolicies.h"
#include "EducationSeed.h"
#include "ParticipantRegistry.h"
#include "WorkloadStrategy.h"

std::unique_ptr<WorkflowBuilder> EducationWorkflowFactory::createBuilder() const
{
    return std::unique_ptr<WorkflowBuilder>(new education::AssignmentMarkingBuilder());
}

std::unique_ptr<WorkflowDirector> EducationWorkflowFactory::createDirector(WorkflowBuilder& builder) const
{
    return std::unique_ptr<WorkflowDirector>(new education::AssignmentMarkingDirector(builder));
}

std::unique_ptr<WorkflowMediator> EducationWorkflowFactory::createCoordinator(
    ParticipantRegistry& participants,
    std::unique_ptr<AssignmentStrategy> strategy) const
{
    return std::unique_ptr<WorkflowMediator>(
        new education::AssignmentMarkingCoordinator(participants, std::move(strategy)));
}

std::unique_ptr<AssignmentStrategy> EducationWorkflowFactory::createStrategy() const
{
    // Markers with the fewest pending submissions receive the next one.
    return std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy());
}

void EducationWorkflowFactory::seedParticipants(ParticipantRegistry& participants) const
{
    if (participants.getByRole(education::EducationRoles::lecturer()).empty())
    {
        education::EducationSeed::seedParticipants(participants);
    }
}

std::string EducationWorkflowFactory::scenarioName() const { return "education"; }
std::string EducationWorkflowFactory::instancePrefix() const { return "edu"; }
std::string EducationWorkflowFactory::containerPrefix() const { return "A"; }
std::string EducationWorkflowFactory::itemPrefix() const { return "SUB"; }
std::string EducationWorkflowFactory::operatorRole() const { return education::EducationRoles::lecturer(); }

bool EducationWorkflowFactory::isEnterableValue(int value) const
{
    return education::EducationPolicies::isValidMark(value);
}

bool EducationWorkflowFactory::isSettleableValue(int value) const
{
    return education::EducationPolicies::isValidMark(value);
}
