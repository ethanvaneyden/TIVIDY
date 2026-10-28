#include "CreatedState.h"
#include "AssignedState.h"
#include <memory>

std::unique_ptr<ItemState> CreatedState::assign(Item&, Participant*)
{
    return std::unique_ptr<ItemState>(new AssignedState());
}

std::string CreatedState::name() const { return "Available"; }
