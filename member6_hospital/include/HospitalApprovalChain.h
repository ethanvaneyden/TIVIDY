/**
 * @file HospitalApprovalChain.h
 * @brief Concrete Chain of Responsibility for treatment severity.
 */

#pragma once

#include "ApprovalHandler.h"

#include <string>

class Item;
class ParticipantRegistry;

namespace hospital
{

/**
 * @class HospitalSeverityHandler
 * @brief Shared behaviour for the three concrete handlers.
 */
class HospitalSeverityHandler : public ApprovalHandler
{
    private:
        std::string role;
        const ParticipantRegistry& registry;

    public:
        HospitalSeverityHandler(const std::string& approverRole,
                                const ParticipantRegistry& participants);
        virtual ~HospitalSeverityHandler();

        /**
         * @brief Approve the item if its severity falls in this handler's band.
         *
         * The approver is never the item's assignee. If the severity is in
         * the band but no eligible approver exists, the request is passed on
         * and ends up unapproved.
         */
        virtual bool handle(Item& item, int value);
};

/** @brief Approves severity 1-3. */
class TriageNurseHandler : public HospitalSeverityHandler
{
    public:
        explicit TriageNurseHandler(const ParticipantRegistry& participants);
};

/** @brief Approves severity 4-6. */
class AdmittingDoctorHandler : public HospitalSeverityHandler
{
    public:
        explicit AdmittingDoctorHandler(const ParticipantRegistry& participants);
};

/**
 * @brief Approves severity 7-10 and is the final gatekeeper: severity 0 or
 *        any invalid value falls off the end and is not approved.
 */
class ChiefOfMedicineHandler : public HospitalSeverityHandler
{
    public:
        explicit ChiefOfMedicineHandler(const ParticipantRegistry& participants);
};

/**
 * @class HospitalApprovalChain
 * @brief Owns the three handlers and links them:
 *        Triage Nurse -> Admitting Doctor -> Chief of Medicine.
 */
class HospitalApprovalChain
{
    private:
        TriageNurseHandler triage;
        AdmittingDoctorHandler admitting;
        ChiefOfMedicineHandler chief;

        HospitalApprovalChain(const HospitalApprovalChain&) = delete;
        HospitalApprovalChain& operator=(const HospitalApprovalChain&) = delete;

    public:
        explicit HospitalApprovalChain(const ParticipantRegistry& participants);

        /** @brief First handler in the chain. */
        ApprovalHandler& head();

        /**
         * @brief Run an item through the chain.
         * @return true if approved, false if it fell off the end.
         */
        bool approve(Item& item, int value);
};

}
