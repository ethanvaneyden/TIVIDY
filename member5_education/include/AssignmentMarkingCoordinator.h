/**
 * @file AssignmentMarkingCoordinator.h
 * @brief Concrete Mediator for the Education scenario.
 *
 * Listens to events from the tree and coordinates the marking flow:
 * assignment of TAs, mark recording, phase progression, batch approval
 * through the chain, and the Lecturer's resolution of rejected marks.
 */

#pragma once

#include "WorkflowMediator.h"
#include "AssignmentStrategy.h"
#include "EducationApprovalChain.h"

#include <memory>
#include <string>
#include <vector>

class AssignmentEngine;
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
        std::unique_ptr<AssignmentEngine> engine;
        ParticipantRegistry* participants;
        std::vector<std::string> history;
        BatchResult lastBatch;

        void log(const std::string& line);

        /** @brief Assign a TA to every Available submission. */
        void assignMarkers(const Container& assignment);

        /** @brief Publish the assignment if it is in Review and ready. */
        void publishIfReady(Container* assignment);

    public:
        /**
         * @param registry Where TAs and approvers are looked up.
         * @param strategy Assignment strategy for TAs; round robin is used when null.
         */
        explicit AssignmentMarkingCoordinator(
            ParticipantRegistry& registry,
            std::unique_ptr<AssignmentStrategy> strategy = std::unique_ptr<AssignmentStrategy>());
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
         * @brief Move an assignment Intake -> Active and assign a TA to every submission.
         * @return the number of submissions that have a TA.
         * @throws InvalidTransition if the assignment is empty or not in Intake.
         */
        int openAssignment(Container& assignment);

        /**
         * @brief Run the approval chain over every completed, unreviewed submission.
         *
         * Approved submissions get a reviewer; the rest are rejected.
         *
         * @throws InvalidTransition if the container is not in the Review phase.
         */
        BatchResult approveBatch(const Container& container);

        /** @return the result of the most recent batch approval. */
        const BatchResult& getLastBatch() const;

        /**
         * @brief The Lecturer resolves a rejected submission personally.
         * @param item     The rejected submission.
         * @param accept   true keeps the TA's mark, false uses newMark.
         * @param newMark  The Lecturer's own mark.
         * @param lecturer Participant with the Lecturer role.
         * @return false if the submission is not rejected, the participant is
         *         not a Lecturer, or the final mark is outside 0-100.
         */
        bool resolveRejected(Submission& item, bool accept, int newMark, Participant* lecturer);

        /** @brief True when every submission is completed and reviewed. */
        bool isReadyToPublish(const Container& container) const;
};

}
