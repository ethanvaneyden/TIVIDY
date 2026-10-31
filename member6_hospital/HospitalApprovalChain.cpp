#include "HospitalApprovalChain.h"
#include "HospitalPolicies.h"
#include "Item.h"
#include "Participant.h"
#include "ParticipantRegistry.h"

namespace hospital
{

HospitalSeverityHandler::HospitalSeverityHandler(const std::string& approverRole,
                                                 const ParticipantRegistry& participants)
    : role(approverRole), registry(participants)
{
}

HospitalSeverityHandler::~HospitalSeverityHandler() {}

bool HospitalSeverityHandler::handle(Item& item, int value)
{
    if (HospitalPolicies::approverRoleFor(value) == role)
    {
        Participant* approver =
            HospitalPolicies::selectApprover(value, item.getAssignee(), registry);
        if (approver != nullptr)
        {
            item.recordReview(approver, "Batch");
            return true;
        }
    }
    return ApprovalHandler::handle(item, value);
}

TriageNurseHandler::TriageNurseHandler(const ParticipantRegistry& participants)
    : HospitalSeverityHandler(HospitalRoles::triageNurse(), participants)
{
}

AdmittingDoctorHandler::AdmittingDoctorHandler(const ParticipantRegistry& participants)
    : HospitalSeverityHandler(HospitalRoles::admittingDoctor(), participants)
{
}

ChiefOfMedicineHandler::ChiefOfMedicineHandler(const ParticipantRegistry& participants)
    : HospitalSeverityHandler(HospitalRoles::chiefOfMedicine(), participants)
{
}

HospitalApprovalChain::HospitalApprovalChain(const ParticipantRegistry& participants)
    : triage(participants), admitting(participants), chief(participants)
{
    triage.setNext(&admitting);
    admitting.setNext(&chief);
}

ApprovalHandler& HospitalApprovalChain::head()
{
    return triage;
}

bool HospitalApprovalChain::approve(Item& item, int value)
{
    return head().handle(item, value);
}

}
