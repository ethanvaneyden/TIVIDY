/**
 * @file ApprovalHandler.h
 * @brief STUB - to be replaced if needed (Basic Approval).
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

