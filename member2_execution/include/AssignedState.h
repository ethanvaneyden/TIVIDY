#pragma once

#include "ItemState.h"

class AssignedState : public InvalidTransitionState
{
public:
    std::unique_ptr<ItemState> start(Item& item) override;
    std::string name() const override;
};
