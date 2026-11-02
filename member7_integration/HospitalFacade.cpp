/**
 * @file HospitalFacade.cpp
 */
#include "HospitalFacade.h"

#include "HospitalWorkflowFactory.h"

HospitalFacade::HospitalFacade(ParticipantRegistry* participants)
    : WorkflowScenarioFacade(participants,
                             std::unique_ptr<WorkflowComponentFactory>(new HospitalWorkflowFactory()))
{
}

HospitalFacade::~HospitalFacade() = default;
