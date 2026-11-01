#include "AssignmentMarkingCoordinator.h"

#include "Container.h"
#include "EducationPolicies.h"
#include "InvalidTransition.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Submission.h"
#include "AssignmentEngine.h"
#include "ApproveContainerCommand.h"
#include "AdvancePhaseCommand.h"
#include "AssignItemCommand.h"
#include "EducationObserver.h"
#include "OverrideItemCommand.h"
#include "RejectItemCommand.h"
#include "RoundRobinStrategy.h"
#include "SystemLogCommand.h"
#include "WorkflowEvent.h"

#include <utility>

namespace education
{

    namespace
    {
        const char *const REJECTED_STATE = "Rejected";
        const char *const AVAILABLE_STATE = "Available";
        const char *const INTAKE_PHASE = "Intake";
        const char *const ACTIVE_PHASE = "Active";
        const char *const REVIEW_PHASE = "Review";

        std::string describe(const Item *item)
        {
            return item == nullptr ? std::string("<no item>") : item->getId();
        }
    }

    AssignmentMarkingCoordinator::AssignmentMarkingCoordinator(
        ParticipantRegistry &registry, std::unique_ptr<AssignmentStrategy> strategy)
        : chain(registry)
    {
        if (!strategy)
        {
            strategy.reset(new RoundRobinStrategy());
        }
        engine.reset(new AssignmentEngine(std::move(strategy)));
        participants = &registry;
        eventBus.subscribe(std::unique_ptr<Observer>(new EducationObserver(&history)));
    }

    AssignmentMarkingCoordinator::~AssignmentMarkingCoordinator() {}

    void AssignmentMarkingCoordinator::setAssignmentStrategy(
        std::unique_ptr<AssignmentStrategy> strategy)
    {
        engine->setStrategy(std::move(strategy));
    }

    std::string AssignmentMarkingCoordinator::getAssignmentStrategyName() const
    {
        return engine->strategyName();
    }

    void AssignmentMarkingCoordinator::executeCommand(
        std::unique_ptr<WorkCommand> command)
    {
        history.execute(std::move(command));
    }

    void AssignmentMarkingCoordinator::log(const std::string &line)
    {
        history.execute(std::unique_ptr<WorkCommand>(new SystemLogCommand(line)));

        WorkflowEvent event;
        event.type = "education.workflow.action";
        event.payload = line;
        eventBus.publish(event);
    }

    void AssignmentMarkingCoordinator::assignMarkers(const Container &assignment)
    {
        const std::vector<Participant *> markers = participants->getByRole(EducationRoles::ta());

        const std::vector<Component *> children = assignment.getChildren();
        for (std::vector<Component *>::const_iterator c = children.begin();
             c != children.end(); ++c)
        {
            Submission *submission = dynamic_cast<Submission *>(*c);
            if (submission == nullptr || submission->getStateName() != AVAILABLE_STATE)
            {
                continue;
            }

            Participant *marker = engine->assign(*submission, markers);
            if (marker != nullptr)
            {
                executeCommand(std::unique_ptr<WorkCommand>(
                    new AssignItemCommand(submission, marker)));
            }
            else if (submission->getMediator() == nullptr)
            {
                onAssignmentFailed(submission, "No marker is available");
            }
        }
    }

    void AssignmentMarkingCoordinator::publishIfReady(Container *assignment)
    {
        if (assignment != nullptr &&
            assignment->getPhaseName() == REVIEW_PHASE &&
            assignment->isReadyToPublish())
        {
            executeCommand(
                std::unique_ptr<WorkCommand>(new ApproveContainerCommand(assignment)));
            onPhaseAdvanced(assignment);
        }
    }

    // Tree events

    void AssignmentMarkingCoordinator::onItemAssigned(Item *item, Participant *assignee)
    {
        log("Submission " + describe(item) + " assigned to " +
            (assignee == nullptr ? std::string("<nobody>") : assignee->getName()));
    }

    void AssignmentMarkingCoordinator::onItemStarted(Item *item)
    {
        log("Marking started on " + describe(item));
    }

    void AssignmentMarkingCoordinator::onItemCompleted(Item *item, int value)
    {
        Submission *submission = dynamic_cast<Submission *>(item);
        if (submission == nullptr)
        {
            return;
        }

        submission->setMark(value);
        log("Submission " + submission->getId() + " marked " + std::to_string(value) +
            (EducationPolicies::isValidMark(value) ? "" : " (invalid mark)"));

        // Once every submission is marked, the assignment moves on to Review.
        Container *assignment = dynamic_cast<Container *>(submission->getParent());
        if (assignment != nullptr &&
            assignment->getPhaseName() == ACTIVE_PHASE &&
            assignment->isCompleted())
        {
            executeCommand(std::unique_ptr<WorkCommand>(new AdvancePhaseCommand(assignment)));
            onPhaseAdvanced(assignment);
        }
    }

    void AssignmentMarkingCoordinator::onItemRejected(Item *item, const std::string &reason)
    {
        log("Submission " + describe(item) + " rejected: " + reason);
    }

    void AssignmentMarkingCoordinator::onItemOverridden(Item *item, Participant *reviewer,
                                                        int newValue, const std::string &reason)
    {
        Submission *submission = dynamic_cast<Submission *>(item);
        if (submission != nullptr)
        {
            submission->setMark(newValue);
        }
        log("Submission " + describe(item) + " settled by " +
            (reviewer == nullptr ? std::string("<nobody>") : reviewer->getName()) +
            " with mark " + std::to_string(newValue) +
            (reason.empty() ? "" : " (" + reason + ")"));

        publishIfReady(item == nullptr ? nullptr : dynamic_cast<Container *>(item->getParent()));
    }

    void AssignmentMarkingCoordinator::onPhaseAdvanced(Container *container)
    {
        if (container == nullptr)
        {
            return;
        }
        log("Assignment " + container->getId() + " moved to " + container->getPhaseName());

        if (container->getPhaseName() == "Publish")
        {
            WorkflowEvent event;
            event.type = "ResultsPublished";
            event.recipientId = container->getId();
            event.payload = "Results published for " + container->getId();
            eventBus.publish(event);
        }

        if (container->getPhaseName() == ACTIVE_PHASE)
        {
            assignMarkers(*container);
        }
    }

    // Assignment and batch events

    void AssignmentMarkingCoordinator::onItemAdded(Item *item)
    {
        log("Submission " + describe(item) + " added; a marker is assigned when the assignment opens");
    }

    void AssignmentMarkingCoordinator::onAssignmentFailed(Item *item, const std::string &reason)
    {
        log("No marker available for " + describe(item) + ": " + reason);
    }

    void AssignmentMarkingCoordinator::onContainerApprovalRequested(Container *container,
                                                                    Participant *operatorParticipant)
    {
        if (container == nullptr)
        {
            return;
        }

        const std::string who =
            operatorParticipant == nullptr ? std::string("<nobody>") : operatorParticipant->getName();

        if (container->getPhaseName() != REVIEW_PHASE)
        {
            log("Batch approval by " + who + " ignored: " + container->getId() +
                " is in " + container->getPhaseName());
            return;
        }

        lastBatch = approveBatch(*container);
        log("Batch approval by " + who + " on " + container->getId() + ": " +
            std::to_string(lastBatch.approvedIds.size()) + " approved, " +
            std::to_string(lastBatch.rejectedIds.size()) + " rejected");

        publishIfReady(container);
    }

    void AssignmentMarkingCoordinator::onContainerRejected(Container *container,
                                                           const std::string &reason)
    {
        log("Assignment " + (container == nullptr ? std::string("<none>") : container->getId()) +
            " rejected: " + reason);
    }

    void AssignmentMarkingCoordinator::onApprovalResolved(Item *item, bool approved)
    {
        log("Submission " + describe(item) + (approved ? " approved" : " could not be approved"));
    }

    std::vector<std::string> AssignmentMarkingCoordinator::getHistoryAsStrings() const
    {
        return history.getHistoryAsStrings();
    }

    // Education operations

    int AssignmentMarkingCoordinator::openAssignment(Container &assignment)
    {
        if (assignment.getPhaseName() != INTAKE_PHASE)
        {
            throw InvalidTransition("An assignment can only be opened from the Intake phase");
        }

        executeCommand(std::unique_ptr<WorkCommand>(new AdvancePhaseCommand(&assignment)));
        onPhaseAdvanced(&assignment);

        int assigned = 0;
        const std::vector<Component *> children = assignment.getChildren();
        for (std::vector<Component *>::const_iterator c = children.begin();
             c != children.end(); ++c)
        {
            const Item *item = dynamic_cast<const Item *>(*c);
            if (item != nullptr && item->getAssignee() != nullptr)
            {
                ++assigned;
            }
        }
        return assigned;
    }

    AssignmentMarkingCoordinator::BatchResult
    AssignmentMarkingCoordinator::approveBatch(const Container &container)
    {
        if (container.getPhaseName() != REVIEW_PHASE)
        {
            throw InvalidTransition("Batch approval requires the Review phase");
        }

        BatchResult result;
        const std::vector<Component *> children = container.getChildren();

        for (std::vector<Component *>::const_iterator c = children.begin();
             c != children.end(); ++c)
        {
            // getChildren() hands back Component*, so cast down to Submission*.
            Submission *submission = dynamic_cast<Submission *>(*c);
            if (submission == nullptr)
            {
                result.skippedIds.push_back((*c)->getId());
                continue;
            }

            const std::string &id = submission->getId();

            if (submission->getStateName() == REJECTED_STATE)
            {
                result.skippedIds.push_back(id); // waiting for the lecturer
            }
            else if (!submission->isCompleted())
            {
                result.pendingIds.push_back(id);
            }
            else if (submission->isReviewed())
            {
                result.skippedIds.push_back(id);
            }
            else
            {
                // Fall back to the item's result if onItemCompleted() was missed.
                if (!submission->hasMark() && submission->hasResult())
                {
                    submission->setMark(submission->getResult());
                }

                const int mark = submission->getMark();
                if (chain.approve(*submission, mark))
                {
                    result.approvedIds.push_back(id);
                    onApprovalResolved(submission, true);
                }
                else
                {
                    executeCommand(std::unique_ptr<WorkCommand>(new RejectItemCommand(
                        submission, "Mark " + std::to_string(mark) +
                                        " cannot be auto-approved")));
                    result.rejectedIds.push_back(id);
                    onApprovalResolved(submission, false);
                }
            }
        }
        return result;
    }

    const AssignmentMarkingCoordinator::BatchResult &AssignmentMarkingCoordinator::getLastBatch() const
    {
        return lastBatch;
    }

    bool AssignmentMarkingCoordinator::resolveRejected(Submission &item, bool accept, int newMark,
                                                       Participant *lecturer)
    {
        if (item.getStateName() != REJECTED_STATE)
        {
            return false;
        }
        if (lecturer == nullptr || lecturer->getRole() != EducationRoles::lecturer())
        {
            return false;
        }

        const int finalMark = accept ? item.getMark() : newMark;
        if (!EducationPolicies::isValidMark(finalMark))
        {
            return false;
        }

        executeCommand(std::unique_ptr<WorkCommand>(new OverrideItemCommand(
            &item, lecturer, finalMark, "Lecturer override")));
        item.setMark(finalMark);

        publishIfReady(dynamic_cast<Container *>(item.getParent()));
        return true;
    }

    bool AssignmentMarkingCoordinator::isReadyToPublish(const Container &container) const
    {
        return container.isReadyToPublish();
    }

}
