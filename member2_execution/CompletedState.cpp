/** @file CompletedState.cpp
* @brief Implements the lifecycle transitions declared in CompletedState.h.
* @author Member 2
* @date October 2026
*/
#include "CompletedState.h"
#include "RejectedState.h"
#include <memory>

std::unique_ptr<ItemState> CompletedState::reject(Item&, const std::string&)
{
    return std::unique_ptr<ItemState>(new RejectedState());
}

std::string CompletedState::name() const { return "Completed"; }
