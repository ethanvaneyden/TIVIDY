#include "CompletedState.h"
#include "RejectedState.h"
#include <memory>

std::unique_ptr<ItemState> CompletedState::reject(Item&, const std::string&)
{
    return std::unique_ptr<ItemState>(new RejectedState());
}

std::string CompletedState::name() const { return "Completed"; }
