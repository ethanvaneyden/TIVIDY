/** @file CreatedState.cpp
* @brief Implements the lifecycle transitions declared in CreatedState.h.
* @author Member 2
* @date October 2026
*/
#include "CreatedState.h"
#include "AssignedState.h"
#include <memory>

std::unique_ptr<ItemState> CreatedState::assign(Item&, Participant*)
{
    return std::unique_ptr<ItemState>(new AssignedState());
}

std::string CreatedState::name() const { return "Available"; }
