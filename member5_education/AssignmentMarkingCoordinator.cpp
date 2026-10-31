#include "AssignmentMarkingCoordinator.h"

#include "Container.h"
#include "EducationPolicies.h"
#include "InvalidTransition.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Submission.h"
#include "AssignmentEngine.h"

namespace education
{

    namespace
    {
        const char *const REJECTED_STATE = "Rejected";
        const char *const ACTIVE_PHASE = "Active";
        const char *const REVIEW_PHASE = "Review";

        std::string describe(const Item *item)
        {
            return item == nullptr ? std::string("<no item>") : item->getId();
        }
    }

    AssignmentMarkingCoordinator::AssignmentMarkingCoordinator(ParticipantRegistry &participants)
        : chain(participants)
    {
    }

    AssignmentMarkingCoordinator::~AssignmentMarkingCoordinator() {}

    void AssignmentMarkingCoordinator::log(const std::string &line)
    {
        history.push_back(line);
    }

    // Tree events

    void AssignmentMarkingCoordinator::onItemAssigned(Item *item, Participant *assignee)
    {
        assignee->incrementPendingTasks();
        log("Submission " + describe(item) + " assigned to " +
            (assignee == nullptr ? std::string("<nobody>") : assignee->getName()));
    }

    void AssignmentMarkingCoordinator::onItemStarted(Item *item)
    {
        log("Marking started on " + describe(item));
    }

    void AssignmentMarkingCoordinator::onItemCompleted(Item *item, int value)
    {
        item->getAssignee()->decrementPendingTasks();
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
            assignment->advancePhase();
        }
    }

    void AssignmentMarkingCoordinator::onItemRejected(Item *item, const std::string &reason)
    {
        if (item != nullptr && item->getAssignee() != nullptr)
        {
            item->getAssignee()->decrementPendingTasks();
        }
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
    }

    void AssignmentMarkingCoordinator::onPhaseAdvanced(Container *container)
    {
        if (container == nullptr)
        {
            return;
        }
        log("Assignment " + container->getId() + " moved to " + container->getPhaseName());
    }

    // Assignment and batch events

    void AssignmentMarkingCoordinator::onItemAdded(Item *item)
    {
        auto eligble = participants->getByRole("TA");
        assignmentEngine->assign(*item, eligble);
        log("Submission " + describe(item) + " added");
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
        return history;
    }

    // Education operations

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
                // The mark normally arrives through onItemCompleted(); if the
                // item was completed without that event, take it from the result.
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
                    submission->reject("Mark " + std::to_string(mark) +
                                       " cannot be auto-approved");
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

        item.resolveReview(lecturer, finalMark);
        item.setMark(finalMark);
        return true;
    }

    bool AssignmentMarkingCoordinator::isReadyToPublish(const Container &container) const
    {
        return container.isReadyToPublish();
    }

}
