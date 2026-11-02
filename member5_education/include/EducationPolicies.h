/**
 * @file EducationPolicies.h
 * @brief Education-specific mark rules: validation, approval bands and
 *        approver selection.
 *
 * Bands (default):
 *   - below 40  : no approver, the chain cannot approve -> Rejected
 *   - 40 to 49  : Moderator
 *   - 50 to 70  : TA (a different TA from the assignee)
 *   - 71 to 100 : Lecturer
 */

#pragma once

#include <string>

class Participant;
class ParticipantRegistry;

namespace education
{

/** Lowest mark an operator may enter. */
const int MIN_MARK = 0;
/** Highest mark an operator may enter. */
const int MAX_MARK = 100;
/** Marks below this value cannot be auto-approved. */
const int REJECT_BELOW = 40;
/** Highest mark handled by the Moderator band. */
const int MODERATOR_MAX = 49;
/** Highest mark handled by the TA band. */
const int TA_MAX = 70;

/**
 * @class EducationRoles
 * @brief Role names used when registering and looking up participants.
 */
class EducationRoles
{
    public:
        static const std::string& ta();
        static const std::string& lecturer();
        static const std::string& moderator();
};

/**
 * @class EducationPolicies
 * @brief Stateless rules for marks in the Education scenario.
 */
class EducationPolicies
{
    public:
        /**
         * @brief Is this a mark the operator is allowed to enter?
         * @return true if MIN_MARK <= mark <= MAX_MARK.
         */
        static bool isValidMark(int mark);

        /** @return true for 40-100, false for 0-39 and for invalid marks. */
        static bool isAutoApprovable(int mark);

        /**
         * @brief Which role approves this mark?
         * @return Role name, or an empty string when nobody can approve it.
         */
        static std::string approverRoleFor(int mark);

        /**
         * @brief Pick the participant who approves a mark.
         * @param mark     The mark awaiting approval.
         * @param assignee Participant who produced it (never selected; may be null).
         * @param registry Where participants are looked up.
         * @return The approver with the fewest pending tasks, or nullptr when
         *         no one is eligible.
         */
        static Participant* selectApprover(int mark,
                                           const Participant* assignee,
                                           const ParticipantRegistry& registry);
};

}

