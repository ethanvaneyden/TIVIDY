/**
 * @file CreatedState.h
 * @brief STUB concrete state — replaced by Member 2's full implementation.
*/

#ifndef CREATEDSTATE_H
#define CREATEDSTATE_H

#include "ItemState.h"

class CreatedState : public ItemState
{
    public:
        CreatedState() = default;
        ~CreatedState() override = default;

        void assign(Item& item) override;
        void start(Item& item) override;
        void complete(Item& item, int value) override;
        void reject(Item& item, const std::string& reason) override;
        std::string name() const override;
};

#endif