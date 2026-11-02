#pragma once

#include "ItemState.h"

class CompletedState : public InvalidTransitionState
{
public:
    std::unique_ptr<ItemState> reject(Item& item, const std::string& reason) override;
    std::string name() const override;
    bool isCompleted() const override { return true; }
};
