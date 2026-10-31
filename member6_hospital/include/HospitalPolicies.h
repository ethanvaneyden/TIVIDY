/**
 * @file HospitalPolicies.h
 * @brief Hospital-specific severity rules: validation, approval bands and
 *        approver selection.
 *
 * Bands (agreed default):
 *   - 1 to 3    : Triage Nurse
 *   - 4 to 6    : Admitting Doctor
 *   - 7 to 10   : Chief of Medicine
 *   - anything else (0, negative, above 10) : no approver, the chain cannot
 *     approve -> the treatment is Rejected
 */

#pragma once

#include <string>

class Participant;
class ParticipantRegistry;

namespace hospital
{

/** Lowest severity that can be approved. */
const int MIN_SEVERITY = 1;
/** Highest severity that can be approved. */
const int MAX_SEVERITY = 10;
/** Highest severity handled by the Triage Nurse band. */
const int TRIAGE_MAX = 3;
/** Highest severity handled by the Admitting Doctor band. */
const int ADMITTING_MAX = 6;

/**
 * @class HospitalRoles
 * @brief Role names used when registering and looking up participants.
 */
class HospitalRoles
{
    public:
        /** @brief Role that performs the treatment work. */
        static const std::string& doctor();
        static const std::string& triageNurse();
        static const std::string& admittingDoctor();
        static const std::string& chiefOfMedicine();
};

/**
 * @class HospitalPolicies
 * @brief Stateless rules for severity in the Hospital scenario.
 */
class HospitalPolicies
{
    public:
        /**
         * @brief Is this a severity the chain is able to approve?
         * @return true if MIN_SEVERITY <= severity <= MAX_SEVERITY.
         */
        static bool isValidSeverity(int severity);

        /**
         * @brief Which role approves this severity?
         * @return Role name, or an empty string when nobody can approve it.
         */
        static std::string approverRoleFor(int severity);

        /**
         * @brief Pick the participant who approves a severity.
         * @param severity The severity awaiting approval.
         * @param assignee Participant who produced it (never selected; may be null).
         * @param registry Where participants are looked up.
         * @return The approver with the fewest pending tasks, or nullptr when
         *         no one is eligible.
         */
        static Participant* selectApprover(int severity,
                                           const Participant* assignee,
                                           const ParticipantRegistry& registry);
};

}
