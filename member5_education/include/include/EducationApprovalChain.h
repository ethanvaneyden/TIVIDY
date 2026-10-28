/**
 * @file EducationApprovalChain.h
 * @brief Concrete Chain of Responsibility for marks.
 */

#ifndef EDUCATIONAPPROVALCHAIN_H
#define EDUCATIONAPPROVALCHAIN_H

#include "ApprovalHandler.h"

#include <string>

class Item;
class ParticipantRegistry;

namespace education
{

/**
 * @class EducationBandHandler
 * @brief Shared behaviour for the three concrete handlers.
 */
class EducationBandHandler : public ApprovalHandler
{
    private:
        std::string role;
        const ParticipantRegistry& registry;

    public:
        EducationBandHandler(const std::string& approverRole,
                             const ParticipantRegistry& participants);
        virtual ~EducationBandHandler();

        /**
         * @brief Approve the item if its mark falls in this handler's band.
         *
         * The approver is never the item's assignee. If the mark is in the
         * band but no eligible approver exists, the request is passed on and
         * ends up Rejected.
         */
        virtual bool handle(Item& item, int value);
};

/** @brief Approves marks 50-70. */
class TAApprovalHandler : public EducationBandHandler
{
    public:
        explicit TAApprovalHandler(const ParticipantRegistry& participants);
};

/** @brief Approves marks 40-49. */
class ModeratorApprovalHandler : public EducationBandHandler
{
    public:
        explicit ModeratorApprovalHandler(const ParticipantRegistry& participants);
};

/** @brief Approves marks 71-100. */
class LecturerApprovalHandler : public EducationBandHandler
{
    public:
        explicit LecturerApprovalHandler(const ParticipantRegistry& participants);
};

/**
 * @class EducationApprovalChain
 * @brief Owns the three handlers and links them: Moderator -> TA -> Lecturer.
 *
 * The order does not matter because the bands do not overlap, but it is fixed
 * so behaviour is deterministic and easy to explain.
 */
class EducationApprovalChain
{
    private:
        ModeratorApprovalHandler moderator;
        TAApprovalHandler ta;
        LecturerApprovalHandler lecturer;

        EducationApprovalChain(const EducationApprovalChain&) = delete;
        EducationApprovalChain& operator=(const EducationApprovalChain&) = delete;

    public:
        explicit EducationApprovalChain(const ParticipantRegistry& participants);

        /** @brief First handler in the chain. */
        ApprovalHandler& head();

        /**
         * @brief Run an item through the chain.
         * @return true if approved, false if it fell off the end.
         */
        bool approve(Item& item, int value);
};

} // namespace education

#endif /*EDUCATIONAPPROVALCHAIN_H*/
