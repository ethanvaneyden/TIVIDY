/**
 * @file ApprovalHandler.h
 * @brief STUB - replaced by Member 4's full implementation.
 *
 * Minimal Chain of Responsibility handler so Members 5 and 6 can write their
 * concrete handlers now. When Member 4 pushes the real ApprovalHandler.h,
 * take theirs (a merge conflict on this path is expected and intended) and
 * adjust EducationBandHandler::handle() in EducationApprovalChain.cpp to the
 * real signature. That is the only place in the education code that touches
 * this interface.
 *
 * Contract assumed here:
 *   - handle() returns true if some handler approved the item,
 *     false if the request fell off the end of the chain.
 *   - setNext() links handlers and returns the handler just linked.
 */

#pragma once

class Item;

class ApprovalHandler
{
    protected:
        ApprovalHandler* next;

    public:
        ApprovalHandler() : next(nullptr) {}
        virtual ~ApprovalHandler() {}

        /** @brief Link the next handler. @return the handler passed in. */
        ApprovalHandler* setNext(ApprovalHandler* n)
        {
            next = n;
            return n;
        }

        /**
         * @brief Try to approve an item.
         * @param item  Item awaiting approval.
         * @param value The result entered for the item (e.g. a mark).
         * @return true if approved by this handler or a later one.
         */
        virtual bool handle(Item& item, int value)
        {
            return next != nullptr ? next->handle(item, value) : false;
        }
};

