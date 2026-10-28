#include "EducationMediator.h"

#include "Container.h"
#include "EducationPolicies.h"
#include "Item.h"
#include "Participant.h"
#include "ParticipantRegistry.h"

namespace education
{

EducationMediator::EducationMediator(ParticipantRegistry& participants)
    : chain(participants)
{
}

EducationMediator::~EducationMediator() {}

void EducationMediator::notify(Colleague* sender, const std::string& event)
{
    // TODO(Member 4): react to ItemCompleted / BatchApproved events once the
    // real mediator contract exists. Education logic is currently driven
    // through recordMark(), approveBatch() and resolveRejected().
    WorkflowMediator::notify(sender, event);
}

bool EducationMediator::recordMark(Item& item, int mark)
{
    if (!EducationPolicies::isValidMark(mark))
    {
        return false;
    }

    std::map<std::string, Record>::iterator it = records.find(item.getId());
    if (it != records.end() && it->second.reviewed)
    {
        return false;
    }

    Record& record = records[item.getId()];
    record.item = &item;
    record.mark = mark;
    record.rejected = false;
    record.reviewed = false;
    return true;
}

bool EducationMediator::hasMark(const std::string& itemId) const
{
    return records.find(itemId) != records.end();
}

int EducationMediator::getMark(const std::string& itemId) const
{
    std::map<std::string, Record>::const_iterator it = records.find(itemId);
    return it == records.end() ? -1 : it->second.mark;
}

EducationMediator::BatchResult EducationMediator::approveBatch(const Container& container)
{
    BatchResult result;
    const std::vector<Component*> children = container.getChildren();

    for (std::vector<Component*>::const_iterator c = children.begin();
         c != children.end(); ++c)
    {
        const std::string& id = (*c)->getId();
        std::map<std::string, Record>::iterator it = records.find(id);

        // No mark yet, or the recorded item is not this child.
        if (it == records.end() || static_cast<Component*>(it->second.item) != *c)
        {
            result.pendingIds.push_back(id);
            continue;
        }

        Record& record = it->second;
        if (record.reviewed || record.rejected)
        {
            result.skippedIds.push_back(id);
            continue;
        }

        if (chain.approve(*record.item, record.mark))
        {
            record.reviewed = true;
            result.approvedIds.push_back(id);
        }
        else
        {
            record.item->reject("Mark " + std::to_string(record.mark) +
                                " cannot be auto-approved");
            record.rejected = true;
            result.rejectedIds.push_back(id);
        }
    }
    return result;
}

bool EducationMediator::resolveRejected(Item& item, bool accept, int newMark,
                                        Participant* lecturer)
{
    std::map<std::string, Record>::iterator it = records.find(item.getId());
    if (it == records.end() || it->second.item != &item || !it->second.rejected)
    {
        return false;
    }
    if (lecturer == nullptr || lecturer->getRole() != EducationRoles::lecturer())
    {
        return false;
    }

    const int finalMark = accept ? it->second.mark : newMark;
    if (!EducationPolicies::isValidMark(finalMark))
    {
        return false;
    }

    item.complete(finalMark); // TODO(Member 2): must allow Rejected -> Completed
    item.recordReview(lecturer, "Personal");

    it->second.mark = finalMark;
    it->second.rejected = false;
    it->second.reviewed = true;
    return true;
}

bool EducationMediator::isReadyToPublish(const Container& container) const
{
    for (std::map<std::string, Record>::const_iterator it = records.begin();
         it != records.end(); ++it)
    {
        if (it->second.rejected)
        {
            return false;
        }
    }
    return container.isCompleted();
}

} // namespace education
