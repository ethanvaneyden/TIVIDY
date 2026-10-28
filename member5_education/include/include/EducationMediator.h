/**
 * @file EducationMediator.h
 * @brief Concrete Mediator for the Education scenario.
 */

#ifndef EDUCATIONMEDIATOR_H
#define EDUCATIONMEDIATOR_H

#include "WorkflowMediator.h"
#include "EducationApprovalChain.h"

#include <map>
#include <string>
#include <vector>

class Container;
class Item;
class Participant;
class ParticipantRegistry;

namespace education
{

class EducationMediator : public WorkflowMediator
{
    public:
        /**
         * @struct BatchResult
         * @brief What happened to each child of the container.
         */
        struct BatchResult
        {
            std::vector<std::string> approvedIds; /**< Approved by the chain. */
            std::vector<std::string> rejectedIds; /**< Chain could not approve. */
            std::vector<std::string> pendingIds;  /**< No mark recorded yet. */
            std::vector<std::string> skippedIds;  /**< Already reviewed or rejected. */
        };

    private:
        struct Record
        {
            Item* item;
            int mark;
            bool rejected;
            bool reviewed;
            Record() : item(nullptr), mark(0), rejected(false), reviewed(false) {}
        };

        EducationApprovalChain chain;
        std::map<std::string, Record> records;

    public:
        explicit EducationMediator(ParticipantRegistry& participants);
        virtual ~EducationMediator();

        /** @brief Placeholder until Member 4's event contract is final. */
        virtual void notify(Colleague* sender, const std::string& event);

        /**
         * @brief Remember the mark entered for an item.
         *
         * Call this before Item::complete(). Marks outside 0-100 are
         * refused, and so is a new mark for an item that was already reviewed.
         *
         * @return true if the mark was recorded.
         */
        bool recordMark(Item& item, int mark);

        /** @return true if a mark has been recorded for this item id. */
        bool hasMark(const std::string& itemId) const;

        /** @return the recorded mark, or -1 if none. */
        int getMark(const std::string& itemId) const;

        /**
         * @brief Run the chain over every child of the container.
         *
         * Items the chain approves get their reviewer recorded ("Batch").
         * Items it cannot approve are sent to Item::reject() and remembered
         * as rejected until the lecturer resolves them.
         */
        BatchResult approveBatch(const Container& container);

        /**
         * @brief The lecturer resolves a rejected item personally.
         *
         * @param item     The rejected item.
         * @param accept   true keeps the TA's mark, false uses newMark.
         * @param newMark  The lecturer's own mark (used when accept is false).
         * @param lecturer Participant with the Lecturer role.
         * @return false if the item is not rejected, the participant is not a
         *         lecturer, or the final mark is outside 0-100.
         *
         * TODO(Member 2): the item state machine must allow Rejected to
         * Completed for this call to succeed on real items.
         */
        bool resolveRejected(Item& item, bool accept, int newMark, Participant* lecturer);

        /**
         * @brief May the container publish?
         *
         * True only if every child is completed and nothing is still rejected
         * (atomic publish).
         */
        bool isReadyToPublish(const Container& container) const;
};

} // namespace education

#endif /*EDUCATIONMEDIATOR_H*/
