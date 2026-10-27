/**
 * @file AssignedState.h
 * @brief STUB concrete state — replaced by Member 2's full implementation.
*/

#ifndef ASSIGNEDSTATE_H
#define ASSIGNEDSTATE_H

#include "ItemState.h"

class AssignedState : public ItemState
{
    public:
        AssignedState() = default;
        ~AssignedState() override = default;

        void assign(Item& item) override;
        void start(Item& item) override;
        void complete(Item& item, int value) override;
        void reject(Item& item, const std::string& reason) override;
        std::string name() const override;
};

#endif