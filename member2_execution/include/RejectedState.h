#pragma once

#include "ItemState.h"

class RejectedState : public InvalidTransitionState
{
public:
    std::unique_ptr<ItemState> resolveReview(
        Item& item,
        Participant* reviewer,
        int newValue) override;
    std::string name() const override;
    bool isRejected() const override { return true; }
};
