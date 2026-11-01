#pragma once

#include "ItemState.h"

// Historical class name retained for compatibility; the runtime state is Available.
class CreatedState : public InvalidTransitionState
{
public:
    std::unique_ptr<ItemState> assign(Item& item, Participant* participant) override;
    std::string name() const override;
};
