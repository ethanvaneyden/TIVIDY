#include "EducationApprovalChain.h"

#include "EducationPolicies.h"
#include "Item.h"
#include "Participant.h"
#include "ParticipantRegistry.h"

namespace education
{

EducationBandHandler::EducationBandHandler(const std::string& approverRole,
                                           const ParticipantRegistry& participants)
    : role(approverRole), registry(participants)
{
}

EducationBandHandler::~EducationBandHandler() {}

bool EducationBandHandler::handle(Item& item, int value)
{
    // Notes: adapt this signature if the real ApprovalHandler takes
    // an ApprovalRequest instead of (Item&, int).
    if (EducationPolicies::approverRoleFor(value) == role)
    {
        Participant* approver =
            EducationPolicies::selectApprover(value, item.getAssignee(), registry);
        if (approver != nullptr)
        {
            item.recordReview(approver, "Batch");
            return true;
        }
    }
    return ApprovalHandler::handle(item, value);
}

TAApprovalHandler::TAApprovalHandler(const ParticipantRegistry& participants)
    : EducationBandHandler(EducationRoles::ta(), participants)
{
}

ModeratorApprovalHandler::ModeratorApprovalHandler(const ParticipantRegistry& participants)
    : EducationBandHandler(EducationRoles::moderator(), participants)
{
}

LecturerApprovalHandler::LecturerApprovalHandler(const ParticipantRegistry& participants)
    : EducationBandHandler(EducationRoles::lecturer(), participants)
{
}

EducationApprovalChain::EducationApprovalChain(const ParticipantRegistry& participants)
    : moderator(participants), ta(participants), lecturer(participants)
{
    moderator.setNext(&ta);
    ta.setNext(&lecturer);
}

ApprovalHandler& EducationApprovalChain::head()
{
    return moderator;
}

bool EducationApprovalChain::approve(Item& item, int value)
{
    return head().handle(item, value);
}

}
