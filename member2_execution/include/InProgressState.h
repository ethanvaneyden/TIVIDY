#pragma once

#include "ItemState.h"

class InProgressState : public InvalidTransitionState
{
public:
    std::unique_ptr<ItemState> complete(Item& item, int value) override;
    std::string name() const override;
};
