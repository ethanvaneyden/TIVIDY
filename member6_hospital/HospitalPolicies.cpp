#include "HospitalPolicies.h"
#include "Participant.h"
#include "ParticipantRegistry.h"

#include <vector>

namespace hospital
{

const std::string& HospitalRoles::doctor()
{
    static const std::string role("Doctor");
    return role;
}

const std::string& HospitalRoles::triageNurse()
{
    static const std::string role("TriageNurse");
    return role;
}

const std::string& HospitalRoles::admittingDoctor()
{
    static const std::string role("AdmittingDoctor");
    return role;
}

const std::string& HospitalRoles::chiefOfMedicine()
{
    static const std::string role("ChiefOfMedicine");
    return role;
}

bool HospitalPolicies::isValidSeverity(int severity)
{
    return severity >= MIN_SEVERITY && severity <= MAX_SEVERITY;
}

std::string HospitalPolicies::approverRoleFor(int severity)
{
    if (!isValidSeverity(severity))
    {
        return std::string();
    }
    if (severity <= TRIAGE_MAX)
    {
        return HospitalRoles::triageNurse();
    }
    if (severity <= ADMITTING_MAX)
    {
        return HospitalRoles::admittingDoctor();
    }
    return HospitalRoles::chiefOfMedicine();
}

Participant* HospitalPolicies::selectApprover(int severity,
                                              const Participant* assignee,
                                              const ParticipantRegistry& registry)
{
    const std::string role = approverRoleFor(severity);
    if (role.empty())
    {
        return nullptr;
    }

    std::vector<Participant*> candidates = registry.getByRole(role);
    Participant* best = nullptr;
    for (std::vector<Participant*>::const_iterator it = candidates.begin();
         it != candidates.end(); ++it)
    {
        Participant* candidate = *it;
        if (candidate == nullptr)
        {
            continue;
        }
        if (assignee != nullptr && candidate->getId() == assignee->getId())
        {
            continue;
        }
        if (best == nullptr ||
            candidate->getPendingTasks() < best->getPendingTasks())
        {
            best = candidate;
        }
    }
    return best;
}

}
