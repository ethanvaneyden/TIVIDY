#include "HospitalAdmissionCoordinator.h"
#include "ActionHistory.h"
#include "AssignmentEngine.h"
#include "Container.h"
#include "EventBus.h"
#include "HospitalAuditCommand.h"
#include "HospitalEvents.h"
#include "HospitalPolicies.h"
#include "InvalidTransition.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Treatment.h"
#include "WorkflowEvent.h"
#include "WorkloadStrategy.h"

#include <exception>
#include <utility>

namespace hospital
{

namespace
{
    const char* const REJECTED_STATE = "Rejected";
    const char* const AVAILABLE_STATE = "Available";
    const char* const INTAKE_PHASE = "Intake";
    const char* const ACTIVE_PHASE = "Active";
    const char* const REVIEW_PHASE = "Review";

    std::string describe(const Item* item)
    {
        return item == nullptr ? std::string("<no item>") : item->getId();
    }

    std::string who(const Participant* p)
    {
        return p == nullptr ? std::string("<nobody>") : p->getName();
    }
}

HospitalAdmissionCoordinator::HospitalAdmissionCoordinator(
    ParticipantRegistry& registry, std::unique_ptr<AssignmentStrategy> strategy)
    : chain(registry), eventBus(nullptr), actionHistory(nullptr)
{
    if (!strategy)
    {
        strategy.reset(new WorkloadStrategy());
    }
    engine.reset(new AssignmentEngine(std::move(strategy)));
    participants = &registry;
}

HospitalAdmissionCoordinator::~HospitalAdmissionCoordinator() {}

void HospitalAdmissionCoordinator::connect(EventBus* bus, ActionHistory* actions)
{
    eventBus = bus;
    actionHistory = actions;
}

void HospitalAdmissionCoordinator::store(const std::string& line)
{
    history.push_back(line);
    if (actionHistory == nullptr)
    {
        return;
    }
    try
    {
        actionHistory->execute(std::unique_ptr<WorkCommand>(new HospitalAuditCommand(line)));
    }
    catch (...)
    {
        history.push_back("Audit record could not be stored");
    }
}

void HospitalAdmissionCoordinator::record(const char* eventType,
                                          const std::string& recipientId,
                                          const std::string& line)
{
    store(line);
    if (eventBus == nullptr)
    {
        return;
    }

    WorkflowEvent event;
    event.type = eventType;
    event.recipientId = recipientId;
    event.payload = line;

    try
    {
        eventBus->publish(event);
    }
    catch (const std::exception& error)
    {
        store(std::string("Notification for ") + eventType + " failed: " + error.what());
    }
    catch (...)
    {
        store(std::string("Notification for ") + eventType + " failed");
    }
}

void HospitalAdmissionCoordinator::publishIfReady(Container* ward)
{
    if (ward != nullptr &&
        ward->getPhaseName() == REVIEW_PHASE &&
        ward->isReadyToPublish())
    {
        ward->advancePhase();
        onPhaseAdvanced(ward);
    }
}

// Tree events

void HospitalAdmissionCoordinator::onItemAssigned(Item* item, Participant* assignee)
{
    record(events::TREATMENT_ASSIGNED, assignee == nullptr ? std::string() : assignee->getId(),
           "Treatment " + describe(item) + " assigned to " + who(assignee));
}

void HospitalAdmissionCoordinator::onItemStarted(Item* item)
{
    record(events::TREATMENT_STARTED,
           (item == nullptr || item->getAssignee() == nullptr) ? std::string() : item->getAssignee()->getId(),
           "Treatment started on " + describe(item));
}

void HospitalAdmissionCoordinator::onItemCompleted(Item* item, int value)
{
    Treatment* treatment = dynamic_cast<Treatment*>(item);
    if (treatment == nullptr)
    {
        return;
    }

    treatment->setSeverity(value);
    record(events::TREATMENT_COMPLETED,
           treatment->getAssignee() == nullptr ? std::string() : treatment->getAssignee()->getId(),
           "Treatment " + treatment->getId() + " completed with severity " +
           std::to_string(value) +
           (HospitalPolicies::isValidSeverity(value) ? "" : " (invalid severity)"));

    Container* ward = dynamic_cast<Container*>(treatment->getParent());
    if (ward != nullptr &&
        ward->getPhaseName() == ACTIVE_PHASE &&
        ward->isCompleted())
    {
        ward->advancePhase();
        onPhaseAdvanced(ward);
    }
}

void HospitalAdmissionCoordinator::onItemRejected(Item* item, const std::string& reason)
{
    record(events::TREATMENT_REJECTED,
           (item == nullptr || item->getAssignee() == nullptr) ? std::string() : item->getAssignee()->getId(),
           "Treatment " + describe(item) + " rejected: " + reason);
}

void HospitalAdmissionCoordinator::onItemOverridden(Item* item, Participant* reviewer,
                                                    int newValue, const std::string& reason)
{
    Treatment* treatment = dynamic_cast<Treatment*>(item);
    if (treatment != nullptr)
    {
        treatment->setSeverity(newValue);
    }

    record(events::TREATMENT_SETTLED, reviewer == nullptr ? std::string() : reviewer->getId(),
           "Treatment " + describe(item) + " settled by " + who(reviewer) +
           " with severity " + std::to_string(newValue) +
           (reason.empty() ? "" : " (" + reason + ")"));

    publishIfReady(item == nullptr ? nullptr : dynamic_cast<Container*>(item->getParent()));
}

void HospitalAdmissionCoordinator::onPhaseAdvanced(Container* container)
{
    if (container == nullptr)
    {
        return;
    }
    record(events::WARD_PHASE_ADVANCED, container->getId(),
           "Ward " + container->getId() + " moved to " + container->getPhaseName());
}

// Assignment and batch events

void HospitalAdmissionCoordinator::onItemAdded(Item* item)
{
    record(events::TREATMENT_ADDED, std::string(),
           "Treatment " + describe(item) + " added; doctor is assigned when the ward opens");
}

void HospitalAdmissionCoordinator::onAssignmentFailed(Item* item, const std::string& reason)
{
    record(events::ASSIGNMENT_FAILED, std::string(),
           "No doctor available for " + describe(item) + ": " + reason);
}

void HospitalAdmissionCoordinator::onContainerApprovalRequested(Container* container,
                                                                Participant* operatorParticipant)
{
    if (container == nullptr)
    {
        return;
    }

    if (container->getPhaseName() != REVIEW_PHASE)
    {
        record(events::BATCH_IGNORED, container->getId(),
               "Batch approval by " + who(operatorParticipant) + " ignored: " +
               container->getId() + " is in " + container->getPhaseName());
        return;
    }

    lastBatch = approveBatch(*container);
    record(events::BATCH_PROCESSED, container->getId(),
           "Batch approval by " + who(operatorParticipant) + " on " + container->getId() + ": " +
           std::to_string(lastBatch.approvedIds.size()) + " approved, " +
           std::to_string(lastBatch.rejectedIds.size()) + " rejected");

    publishIfReady(container);
}

void HospitalAdmissionCoordinator::onContainerRejected(Container* container,
                                                       const std::string& reason)
{
    record(events::WARD_REJECTED, container == nullptr ? std::string() : container->getId(),
           "Ward " + (container == nullptr ? std::string("<none>") : container->getId()) +
           " rejected: " + reason);
}

void HospitalAdmissionCoordinator::onApprovalResolved(Item* item, bool approved)
{
    record(events::APPROVAL_RESOLVED,
           (item == nullptr || item->getReviewer() == nullptr) ? std::string() : item->getReviewer()->getId(),
           "Treatment " + describe(item) + (approved ? " approved" : " could not be approved"));
}

std::vector<std::string> HospitalAdmissionCoordinator::getHistoryAsStrings() const
{
    return history;
}

// Hospital operations

int HospitalAdmissionCoordinator::openWard(Container& ward)
{
    if (ward.getPhaseName() != INTAKE_PHASE)
    {
        throw InvalidTransition("A ward can only be opened from the Intake phase");
    }

    ward.advancePhase();
    onPhaseAdvanced(&ward);

    const std::vector<Participant*> doctors = participants->getByRole(HospitalRoles::doctor());
    int assigned = 0;

    const std::vector<Component*> children = ward.getChildren();
    for (std::vector<Component*>::const_iterator c = children.begin();
         c != children.end(); ++c)
    {
        Item* item = dynamic_cast<Item*>(*c);
        if (item == nullptr || item->getStateName() != AVAILABLE_STATE)
        {
            continue;
        }

        Participant* doctor = engine->assign(*item, doctors);
        if (doctor != nullptr)
        {
            item->assign(doctor);
            ++assigned;
        }
        else if (item->getMediator() == nullptr)
        {
            onAssignmentFailed(item, "No worker is available");
        }
    }
    return assigned;
}

HospitalAdmissionCoordinator::BatchResult
HospitalAdmissionCoordinator::approveBatch(const Container& ward)
{
    if (ward.getPhaseName() != REVIEW_PHASE)
    {
        throw InvalidTransition("Batch approval requires the Review phase");
    }

    BatchResult result;
    const std::vector<Component*> children = ward.getChildren();
    for (std::vector<Component*>::const_iterator c = children.begin();
         c != children.end(); ++c)
    {
        Treatment* treatment = dynamic_cast<Treatment*>(*c);
        if (treatment == nullptr)
        {
            result.skippedIds.push_back((*c)->getId());
            continue;
        }

        const std::string& id = treatment->getId();
        if (treatment->getStateName() == REJECTED_STATE)
        {
            result.skippedIds.push_back(id);
        }
        else if (!treatment->isCompleted())
        {
            result.pendingIds.push_back(id);
        }
        else if (treatment->isReviewed())
        {
            result.skippedIds.push_back(id);
        }
        else
        {
            if (!treatment->hasSeverity() && treatment->hasResult())
            {
                treatment->setSeverity(treatment->getResult());
            }

            const int severity = treatment->getSeverity();
            if (chain.approve(*treatment, severity))
            {
                result.approvedIds.push_back(id);
                onApprovalResolved(treatment, true);
            }
            else
            {
                treatment->reject("Severity " + std::to_string(severity) +
                                  " cannot be auto-approved");
                result.rejectedIds.push_back(id);
                onApprovalResolved(treatment, false);
            }
        }
    }
    return result;
}

const HospitalAdmissionCoordinator::BatchResult& HospitalAdmissionCoordinator::getLastBatch() const
{
    return lastBatch;
}

bool HospitalAdmissionCoordinator::resolveRejected(Treatment& item, bool accept,
                                                   int newSeverity, Participant* chief)
{
    if (item.getStateName() != REJECTED_STATE)
    {
        return false;
    }
    if (chief == nullptr || chief->getRole() != HospitalRoles::chiefOfMedicine())
    {
        return false;
    }

    const int finalSeverity = accept ? item.getSeverity() : newSeverity;
    if (!HospitalPolicies::isValidSeverity(finalSeverity))
    {
        return false;
    }

    item.resolveReview(chief, finalSeverity, "Chief of Medicine override");
    item.setSeverity(finalSeverity);
    return true;
}

bool HospitalAdmissionCoordinator::isReadyToPublish(const Container& ward) const
{
    return ward.isReadyToPublish();
}

}
