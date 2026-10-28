#include "InProgressState.h"
#include "CompletedState.h"
#include <memory>

std::unique_ptr<ItemState> InProgressState::complete(Item&, int)
{
    return std::unique_ptr<ItemState>(new CompletedState());
}

std::string InProgressState::name() const { return "InProgress"; }
