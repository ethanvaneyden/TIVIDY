/**
 * @file HospitalWorkflowFactory.cpp
 * @brief Implementation of the healthcare factory.
*/

#include "HospitalWorkflowFactory.h"
#include "WorkflowBuilder.h"
#include "AssignmentStrategy.h"   // [DEPENDS ON] Member 3
#include "ExternalSystem.h"       // [DEPENDS ON] Member 5

/* [DEPENDS ON] Member 6: HospitalAdmissionBuilder.h, HospitalAdapter.h — not
 * yet implemented. Same pattern as EducationWorkflowFactory.cpp.*/

// #include "HospitalAdmissionBuilder.h"
// #include "HospitalAdapter.h"
// #include "RoleBasedStrategy.h"

std::unique_ptr<WorkflowBuilder>
HospitalWorkflowFactory::createBuilder() const
{
    /* [TBD — Member 6] return std::unique_ptr<HospitalAdmissionBuilder>(new HospitalAdmissionBuilder());*/
    return nullptr;
}

std::unique_ptr<AssignmentStrategy>
HospitalWorkflowFactory::createStrategy() const
{
    /* [TBD — Member 3] return std::unique_ptr<RoleBasedStrategy>(new RoleBasedStrategy());*/
    return nullptr;
}

std::unique_ptr<ExternalSystem>
HospitalWorkflowFactory::createExternalSystem() const
{
    /* [TBD — Member 6] return std::unique_ptr<HospitalAdapter>(new HospitalAdapter());*/
    return nullptr;
}

std::string HospitalWorkflowFactory::scenarioName() const
{
    return "healthcare";
}