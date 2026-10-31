// member7_integration/HospitalWorkflowFactory.cpp
/**
 * @file HospitalWorkflowFactory.cpp
 * @brief Implementation of the Hospital scenario factory.
*/
#include "HospitalWorkflowFactory.h"

#include "HospitalAdmissionBuilder.h"
#include "WorkloadStrategy.h" // Reusing a generic strategy

std::unique_ptr<WorkflowBuilder> HospitalWorkflowFactory::createBuilder() const
{
    return std::unique_ptr<WorkflowBuilder>(new hospital::HospitalAdmissionBuilder());
}

std::unique_ptr<AssignmentStrategy> HospitalWorkflowFactory::createStrategy() const
{
    // Reusing the generic WorkloadStrategy. A hospital-specific strategy could be swapped here.
    return std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy());
}

std::string HospitalWorkflowFactory::scenarioName() const
{
    return "healthcare";
}