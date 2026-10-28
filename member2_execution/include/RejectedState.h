#pragma once

#include "ItemState.h"

class RejectedState : public ItemState
{
public:
    void complete(Item& item, int) override;
    std::string name() const override;
    bool isRejected() const override { return true; }
};
