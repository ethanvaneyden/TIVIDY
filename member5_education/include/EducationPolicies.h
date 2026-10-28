/**
 * @file EducationPolicies.h
 * @brief Education-specific mark rules: validation, approval bands and
 */

#ifndef EDUCATIONPOLICIES_H
#define EDUCATIONPOLICIES_H

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
         * @param mark Candidate mark.
         * @return true if 0 <= mark <= 100. Invalid input is refused by the
         *         command; it never reaches the approval chain.
         */
        static bool isValidMark(int mark);

        /**
         * @brief Can the chain approve this mark automatically?
         * @param mark A valid mark (0-100).
         * @return true for 40-100, false for 0-39 and for invalid marks.
         */
        static bool isAutoApprovable(int mark);

        /**
         * @brief Which role approves this mark?
         * @param mark Candidate mark.
         * @return Role name, or an empty string when nobody can approve it
         *         (below 40, or outside 0-100).
         */
        static std::string approverRoleFor(int mark);

        /**
         * @brief Pick the participant who approves a mark.
         *
         * Looks up everyone with the approving role and skips the item's
         * assignee, so nobody approves their own work. Among the eligible
         * participants the one with the fewest pending tasks wins; ties go
         * to the first in registry order.
         *
         * @param mark     The mark awaiting approval.
         * @param assignee Participant who produced the mark (may be null).
         * @param registry Where participants are looked up.
         * @return The approver, or nullptr when no one is eligible.
         */
        static Participant* selectApprover(int mark,
                                           const Participant* assignee,
                                           const ParticipantRegistry& registry);
};

} // namespace education

#endif /*EDUCATIONPOLICIES_H*/
