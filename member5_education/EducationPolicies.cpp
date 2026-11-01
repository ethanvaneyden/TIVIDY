#include "EducationPolicies.h"

#include "Participant.h"
#include "ParticipantRegistry.h"

#include <vector>

namespace education
{

const std::string& EducationRoles::ta()
{
    static const std::string role("TA");
    return role;
}

const std::string& EducationRoles::lecturer()
{
    static const std::string role("Lecturer");
    return role;
}

const std::string& EducationRoles::moderator()
{
    static const std::string role("Moderator");
    return role;
}

bool EducationPolicies::isValidMark(int mark)
{
    return mark >= MIN_MARK && mark <= MAX_MARK;
}

bool EducationPolicies::isAutoApprovable(int mark)
{
    return isValidMark(mark) && mark >= REJECT_BELOW;
}

std::string EducationPolicies::approverRoleFor(int mark)
{
    if (!isAutoApprovable(mark))
    {
        return std::string();
    }
    if (mark <= MODERATOR_MAX)
    {
        return EducationRoles::moderator();
    }
    if (mark <= TA_MAX)
    {
        return EducationRoles::ta();
    }
    return EducationRoles::lecturer();
}

Participant* EducationPolicies::selectApprover(int mark,
                                               const Participant* assignee,
                                               const ParticipantRegistry& registry)
{
    const std::string role = approverRoleFor(mark);
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
        // Nobody approves their own work.
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
