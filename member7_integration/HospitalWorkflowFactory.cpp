/**
 * @file HospitalWorkflowFactory.cpp
 * @brief Implementation of the Hospital scenario factory.
*/
#include "HospitalWorkflowFactory.h"

#include "HospitalAdmissionBuilder.h"
#include "HospitalAdmissionCoordinator.h"
#include "HospitalAdmissionDirector.h"
#include "HospitalPolicies.h"
#include "HospitalSeed.h"
#include "ParticipantRegistry.h"
#include "WorkloadStrategy.h"

std::unique_ptr<WorkflowBuilder> HospitalWorkflowFactory::createBuilder() const
{
    return std::unique_ptr<WorkflowBuilder>(new hospital::HospitalAdmissionBuilder());
}

std::unique_ptr<WorkflowDirector> HospitalWorkflowFactory::createDirector(WorkflowBuilder& builder) const
{
    return std::unique_ptr<WorkflowDirector>(new hospital::HospitalAdmissionDirector(builder));
}

std::unique_ptr<WorkflowMediator> HospitalWorkflowFactory::createCoordinator(
    ParticipantRegistry& participants,
    std::unique_ptr<AssignmentStrategy> strategy) const
{
    return std::unique_ptr<WorkflowMediator>(
        new hospital::HospitalAdmissionCoordinator(participants, std::move(strategy)));
}

std::unique_ptr<AssignmentStrategy> HospitalWorkflowFactory::createStrategy() const
{
    // Doctors with the fewest open cases receive the next patient.
    return std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy());
}

void HospitalWorkflowFactory::seedParticipants(ParticipantRegistry& participants) const
{
    if (participants.getByRole(hospital::HospitalRoles::chiefOfMedicine()).empty())
    {
        hospital::HospitalSeed::seedParticipants(participants);
    }
}

std::string HospitalWorkflowFactory::scenarioName() const { return "healthcare"; }
std::string HospitalWorkflowFactory::instancePrefix() const { return "hc"; }
std::string HospitalWorkflowFactory::containerPrefix() const { return "W"; }
std::string HospitalWorkflowFactory::itemPrefix() const { return "TRT"; }
std::string HospitalWorkflowFactory::operatorRole() const { return hospital::HospitalRoles::chiefOfMedicine(); }

bool HospitalWorkflowFactory::isEnterableValue(int value) const
{
    // 0 may be entered: it is the "cannot be approved" exception path.
    return value >= 0 && value <= hospital::MAX_SEVERITY;
}

bool HospitalWorkflowFactory::isSettleableValue(int value) const
{
    return hospital::HospitalPolicies::isValidSeverity(value);
}
