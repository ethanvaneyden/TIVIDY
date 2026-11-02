/**
 * @file EducationFacade.h
 * @brief ScenarioFacade for the education industry (assignment marking).
 *
 * All behaviour lives in WorkflowScenarioFacade; this class only supplies
 * the education component family (EducationWorkflowFactory) and the legacy
 * meaning of submit().
*/

#pragma once

#include "WorkflowScenarioFacade.h"

class EducationFacade : public WorkflowScenarioFacade
{
    protected:
        /** @brief Legacy: adds a submission to the first assignment. */
        CommandResult legacySubmit(const std::string& instanceId,
                                   const std::string& name) override;

    public:
        /** @brief Adds the education staff to the registry if they are not there yet. */
        explicit EducationFacade(ParticipantRegistry* participants);
        ~EducationFacade() override;
};
