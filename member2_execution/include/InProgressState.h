#pragma once

#include "ItemState.h"

class InProgressState : public ItemState
{
public:
    void complete(Item& item, int) override;
    std::string name() const override;
};
