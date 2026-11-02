/**
 * @file HospitalFacade.h
 * @brief ScenarioFacade for the healthcare industry (hospital admission).
 *
 * All behaviour lives in WorkflowScenarioFacade; this class only supplies
 * the hospital component family (HospitalWorkflowFactory).
*/

#pragma once

#include "WorkflowScenarioFacade.h"

class HospitalFacade : public WorkflowScenarioFacade
{
    public:
        /** @brief Adds the hospital staff to the registry if they are not there yet. */
        explicit HospitalFacade(ParticipantRegistry* participants);
        ~HospitalFacade() override;
};
