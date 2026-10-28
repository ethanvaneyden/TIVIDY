#pragma once

#include "ItemState.h"

// Historical class name retained for compatibility; the runtime state is Available.
class CreatedState : public ItemState
{
public:
    void assign(Item& item) override;
    std::string name() const override;
};
