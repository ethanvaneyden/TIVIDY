/**
 * @file EducationWorkflowFactory.cpp
 * @brief Implementation of the Education scenario factory.
*/
#include "EducationWorkflowFactory.h"

#include "AssignmentMarkingBuilder.h"
#include "LMSAdapter.h"
#include "WorkloadStrategy.h"

std::unique_ptr<WorkflowBuilder> EducationWorkflowFactory::createBuilder() const
{
    return std::unique_ptr<WorkflowBuilder>(new education::AssignmentMarkingBuilder());
}

std::unique_ptr<AssignmentStrategy> EducationWorkflowFactory::createStrategy() const
{
    return std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy());
}

std::unique_ptr<ExternalSystem> EducationWorkflowFactory::createExternalSystem() const
{
    return std::unique_ptr<ExternalSystem>(new education::LMSAdapter());
}

std::string EducationWorkflowFactory::scenarioName() const
{
    return "education";
}