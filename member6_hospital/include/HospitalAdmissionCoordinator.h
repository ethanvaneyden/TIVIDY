/**
 * @file HospitalAdmissionCoordinator.h
 * @brief Concrete Mediator for the Hospital scenario.
 *
 * Listens to events from the tree and coordinates the healthcare flow:
 * assignment of doctors, severity recording, phase progression, batch
 * approval through the chain, and the Chief of Medicine's override.
 */

#pragma once

#include "WorkflowMediator.h"
#include "AssignmentStrategy.h"
#include "HospitalApprovalChain.h"

#include <memory>
#include <string>
#include <vector>

class AssignmentEngine;
class ParticipantRegistry;

namespace hospital
{

class Treatment;

class HospitalAdmissionCoordinator : public WorkflowMediator
{
    public:
        /**
         * @struct BatchResult
         * @brief What happened to each treatment of the ward.
         */
        struct BatchResult
        {
            std::vector<std::string> approvedIds; /* Approved by the chain. */
            std::vector<std::string> rejectedIds; /* Chain could not approve. */
            std::vector<std::string> pendingIds;  /* Not completed (no severity yet). */
            std::vector<std::string> skippedIds;  /* Already reviewed, or rejected earlier. */
        };

    private:
        HospitalApprovalChain chain;
        std::unique_ptr<AssignmentEngine> engine;
        std::vector<std::string> history;
        BatchResult lastBatch;

        void log(const std::string& line);
        void publishIfReady(Container* ward);

    public:
        /**
         * @param registry Where doctors and approvers are looked up.
         * @param strategy Assignment strategy for doctors; a workload-based
         *                 strategy is used when null.
         */
        explicit HospitalAdmissionCoordinator(
            ParticipantRegistry& registry,
            std::unique_ptr<AssignmentStrategy> strategy = std::unique_ptr<AssignmentStrategy>());
        virtual ~HospitalAdmissionCoordinator();

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

        // Hospital operations

        /**
         * @brief Open a ward for treatment: Intake -> Active, then assign a
         *        doctor to every available treatment through the assignment engine.
         *
         * Items can only be assigned in the Active phase, so assignment is
         * done here, one treatment at a time, which lets a workload strategy
         * balance the doctors.
         *
         * @return the number of treatments that received a doctor.
         * @throws InvalidTransition if the ward is empty or not in Intake.
         */
        int openWard(Container& ward);

        /**
         * @brief Run the approval chain over every completed, unreviewed treatment.
         *
         * Approved treatments get their reviewer recorded ("Batch").
         * Treatments the chain cannot approve are rejected.
         *
         * @throws InvalidTransition if the ward is not in the Review phase.
         */
        BatchResult approveBatch(const Container& ward);

        /** @return the result of the most recent batch approval. */
        const BatchResult& getLastBatch() const;

        /**
         * @brief The Chief of Medicine resolves a rejected treatment personally.
         *
         * @param item     The rejected treatment.
         * @param accept   true keeps the doctor's severity, false uses newSeverity.
         * @param newSeverity The chief's own severity (used when accept is false).
         * @param chief    Participant with the Chief of Medicine role.
         * @return false if the treatment is not rejected, the participant is
         *         not the chief, or the final severity is outside 1-10.
         */
        bool resolveRejected(Treatment& item, bool accept, int newSeverity, Participant* chief);

        /** @brief May the ward publish? Delegates to Container::isReadyToPublish(). */
        bool isReadyToPublish(const Container& ward) const;
};

}
