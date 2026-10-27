/**
 * @file InProgressState.h
 * @brief STUB concrete state — replaced by Member 2's full implementation.
*/

#ifndef INPROGRESSSTATE_H
#define INPROGRESSSTATE_H

#include "ItemState.h"

class InProgressState : public ItemState
{
    public:
        InProgressState() = default;
        ~InProgressState() override = default;

        void assign(Item& item) override;
        void start(Item& item) override;
        void complete(Item& item, int value) override;
        void reject(Item& item, const std::string& reason) override;
        std::string name() const override;
};

#endif