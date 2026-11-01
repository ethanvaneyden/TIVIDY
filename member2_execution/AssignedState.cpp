/** @file AssignedState.cpp
* @brief Implements the lifecycle transitions declared in AssignedState.h.
* @author Member 2
* @date October 2026
*/
#include "AssignedState.h"
#include "InProgressState.h"
#include <memory>

std::unique_ptr<ItemState> AssignedState::start(Item&)
{
    return std::unique_ptr<ItemState>(new InProgressState());
}

std::string AssignedState::name() const { return "Assigned"; }
