/**
 * @file AssignmentMarkingCoordinator.h
 * @brief Concrete Mediator for the Education scenario.
 * Notes: switch the base once WorkflowCoordinator is
 * pushed, and drop any history/logging it already provides.
 * 
 * Notes: the real ApprovalHandler may report to the mediator itself
 * (onApprovalResolved). Until then approveBatch() calls it after each result.
 */

#pragma once

#include "WorkflowMediator.h"
#include "EducationApprovalChain.h"

#include <string>
#include <vector>

class ParticipantRegistry;

namespace education
{

class Submission;

class AssignmentMarkingCoordinator : public WorkflowMediator
{
    public:
        /**
         * @struct BatchResult
         * @brief What happened to each submission of the assignment.
         */
        struct BatchResult
        {
            std::vector<std::string> approvedIds; /* Approved by the chain. */
            std::vector<std::string> rejectedIds; /* Chain could not approve. */
            std::vector<std::string> pendingIds;  /* Not completed (no mark yet). */
            std::vector<std::string> skippedIds;  /* Already reviewed, or rejected earlier. */
        };

    private:
        EducationApprovalChain chain;
        std::vector<std::string> history;
        BatchResult lastBatch;

        void log(const std::string& line);

    public:
        explicit AssignmentMarkingCoordinator(ParticipantRegistry& participants);
        virtual ~AssignmentMarkingCoordinator();

        // Tree events
        virtual void onItemAssigned(Item* item, Participant* assignee);
        virtual void onItemStarted(Item* item);
        virtual void onItemCompleted(Item* item, int value);
        virtual void onItemRejected(Item* item, const std::string& reason);
        virtual void onItemOverridden(Item* item, Participant* reviewer,
                                      int newValue, const std::string& reason);
        virtual void onPhaseAdvanced(Container* container);

        // Assignment and batch events
        virtual void onItemAdded(Item* item);
        virtual void onAssignmentFailed(Item* item, const std::string& reason);
        virtual void onContainerApprovalRequested(Container* container,
                                                  Participant* operatorParticipant);
        virtual void onContainerRejected(Container* container, const std::string& reason);
        virtual void onApprovalResolved(Item* item, bool approved);

        // Utility
        virtual std::vector<std::string> getHistoryAsStrings() const;

        // Education operations

        /**
         * @brief Run the approval chain over every completed, unreviewed submission.
         *
         * Approved submissions get their reviewer recorded ("Batch").
         * Submissions the chain cannot approve are rejected.
         *
         * @throws InvalidTransition if the container is not in the Review phase.
         */
        BatchResult approveBatch(const Container& container);

        /** @return the result of the most recent batch approval. */
        const BatchResult& getLastBatch() const;

        /**
         * @brief The lecturer resolves a rejected submission personally.
         *
         * @param item     The rejected submission.
         * @param accept   true keeps the TA's mark, false uses newMark.
         * @param newMark  The lecturer's own mark (used when accept is false).
         * @param lecturer Participant with the Lecturer role.
         * @return false if the submission is not rejected, the participant is
         *         not a lecturer, or the final mark is outside 0-100.
         */
        bool resolveRejected(Submission& item, bool accept, int newMark, Participant* lecturer);

        /**
         * @brief May the assignment publish?
         *
         * Delegates to Container::isReadyToPublish(): every submission must be
         * completed and reviewed. A rejected submission is not completed, so a
         * single rejected submission blocks the whole assignment.
         */
        bool isReadyToPublish(const Container& container) const;
};

}

