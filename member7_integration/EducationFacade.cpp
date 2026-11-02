/**
 * @file EducationFacade.cpp
 */
#include "EducationFacade.h"

#include "AssignmentMarkingDirector.h"
#include "EducationWorkflowFactory.h"

EducationFacade::EducationFacade(ParticipantRegistry* participants)
    : WorkflowScenarioFacade(participants,
                             std::unique_ptr<WorkflowComponentFactory>(new EducationWorkflowFactory()))
{
}

EducationFacade::~EducationFacade() = default;

CommandResult EducationFacade::legacySubmit(const std::string& instanceId, const std::string& name)
{
    return addItem(instanceId, education::AssignmentMarkingDirector::assignmentId(), name,
                   std::map<std::string, std::string>());
}
