#pragma once

#include "ItemState.h"

class CompletedState : public ItemState
{
public:
    void reject(Item& item, const std::string&) override;
    std::string name() const override;
    bool isCompleted() const override { return true; }
};
