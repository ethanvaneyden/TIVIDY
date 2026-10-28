#pragma once

#include "ItemState.h"

class AssignedState : public ItemState
{
public:
    void start(Item& item) override;
    std::string name() const override;
};
