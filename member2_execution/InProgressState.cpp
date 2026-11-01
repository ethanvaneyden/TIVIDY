/** @file InProgressState.cpp
* @brief Implements the lifecycle transitions declared in InProgressState.h.
* @author Member 2
* @date October 2026
*/
#include "InProgressState.h"
#include "CompletedState.h"
#include <memory>

std::unique_ptr<ItemState> InProgressState::complete(Item&, int)
{
    return std::unique_ptr<ItemState>(new CompletedState());
}

std::string InProgressState::name() const { return "InProgress"; }
