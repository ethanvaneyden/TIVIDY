/** @file RejectedState.cpp
* @brief Implements the lifecycle transitions declared in RejectedState.h.
* @author Member 2
* @date October 2026
*/
#include "RejectedState.h"
#include "CompletedState.h"
#include <memory>

std::unique_ptr<ItemState> RejectedState::resolveReview(Item&, Participant*, int)
{
    return std::unique_ptr<ItemState>(new CompletedState());
}

std::string RejectedState::name() const { return "Rejected"; }
