/**
 * @file RejectedState.h
 * @brief STUB concrete state — replaced by Member 2's full implementation.
*/

#ifndef REJECTEDSTATE_H
#define REJECTEDSTATE_H

#include "ItemState.h"

class RejectedState : public ItemState
{
    public:
        RejectedState() = default;
        ~RejectedState() override = default;

        void assign(Item& item) override;
        void start(Item& item) override;
        void complete(Item& item, int value) override;
        void reject(Item& item, const std::string& reason) override;
        std::string name() const override;
};

#endif