#include "RejectedState.h"
#include "CompletedState.h"
#include <memory>

std::unique_ptr<ItemState> RejectedState::resolveReview(Item&, Participant*, int)
{
    return std::unique_ptr<ItemState>(new CompletedState());
}

std::string RejectedState::name() const { return "Rejected"; }
