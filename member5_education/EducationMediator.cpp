#include "EducationMediator.h"

#include "Container.h"
#include "EducationPolicies.h"
#include "Item.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Submission.h"

namespace education
{

EducationMediator::EducationMediator(ParticipantRegistry& participants)
    : chain(participants)
{
}

EducationMediator::~EducationMediator() {}

void EducationMediator::notify(Colleague* sender, const std::string& event)
{
    // Education batch approval remains an explicit operator action.
    WorkflowMediator::notify(sender, event);
}

EducationMediator::BatchResult EducationMediator::approveBatch(const Container& container)
{
    BatchResult result;
    const std::vector<Component*> children = container.getChildren();

    for (std::vector<Component*>::const_iterator c = children.begin();
         c != children.end(); ++c)
    {
        const std::string& id = (*c)->getId();
        Submission* submission = dynamic_cast<Submission*>(*c);
        if (submission == nullptr || !submission->hasMark())
        {
            result.pendingIds.push_back(id);
            continue;
        }

        if (submission->isReviewed() || submission->getStateName() == "Rejected")
        {
            result.skippedIds.push_back(id);
            continue;
        }

        if (chain.approve(*submission, submission->getMark()))
        {
            result.approvedIds.push_back(id);
        }
        else
        {
            submission->reject("Mark " + std::to_string(submission->getMark()) +
                                " cannot be auto-approved");
            result.rejectedIds.push_back(id);
        }
    }
    return result;
}

bool EducationMediator::resolveRejected(Item& item, bool accept, int newMark,
                                        Participant* lecturer)
{
    Submission* submission = dynamic_cast<Submission*>(&item);
    if (submission == nullptr || submission->getStateName() != "Rejected" ||
        !submission->hasMark())
    {
        return false;
    }
    if (lecturer == nullptr || lecturer->getRole() != EducationRoles::lecturer())
    {
        return false;
    }

    const int finalMark = accept ? submission->getMark() : newMark;
    if (!EducationPolicies::isValidMark(finalMark))
    {
        return false;
    }

    submission->resolveReview(lecturer, finalMark);
    return true;
}

bool EducationMediator::isReadyToPublish(const Container& container) const
{
    return container.isReadyToPublish();
}

} // namespace education
