/**
 * @file EducationWorkflowFactory.cpp
 * @brief Implementation of the education factory.
*/

#include "EducationWorkflowFactory.h"
#include "WorkflowBuilder.h"
#include "AssignmentStrategy.h"   // [DEPENDS ON] Member 3
#include "ExternalSystem.h"       // [DEPENDS ON] Member 5

/* [DEPENDS ON] Member 5: AssignmentMarkingBuilder.h, LMSAdapter.h — not yet
 * implemented. Once they exist, uncomment the includes and bodies below.
 *
 * [DEPENDS ON] Member 3: AssignmentStrategy.h + a concrete strategy
 * (RoundRobinStrategy / WorkloadStrategy) — not yet implemented.
*/

// #include "AssignmentMarkingBuilder.h"
// #include "LMSAdapter.h"
// #include "RoundRobinStrategy.h"

std::unique_ptr<WorkflowBuilder>
EducationWorkflowFactory::createBuilder() const
{
    /* [TBD — Member 5] return std::unique_ptr<AssignmentMarkingBuilder>(new AssignmentMarkingBuilder());*/
    return nullptr;
}

std::unique_ptr<AssignmentStrategy>
EducationWorkflowFactory::createStrategy() const
{
    /* [TBD — Member 3] return std::unique_ptr<RoundRobinStrategy>(new RoundRobinStrategy());*/
    return nullptr;
}

std::unique_ptr<ExternalSystem>
EducationWorkflowFactory::createExternalSystem() const
{
    /* [TBD — Member 5] return std::unique_ptr<LMSAdapter>(new LMSAdapter());*/
    return nullptr;
}

std::string EducationWorkflowFactory::scenarioName() const
{
    return "education";
}