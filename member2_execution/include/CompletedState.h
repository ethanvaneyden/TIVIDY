/**
 * @file CompletedState.h
 * @brief STUB concrete state — replaced by Member 2's full implementation.
*/

#ifndef COMPLETEDSTATE_H
#define COMPLETEDSTATE_H

#include "ItemState.h"

class CompletedState : public ItemState
{
    public:
        CompletedState() = default;
        ~CompletedState() override = default;

        void assign(Item& item) override;
        void start(Item& item) override;
        void complete(Item& item, int value) override;
        void reject(Item& item, const std::string& reason) override;
        std::string name() const override;
};

#endif