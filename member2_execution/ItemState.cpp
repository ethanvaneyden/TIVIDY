/** @file ItemState.cpp
* @brief Implements the lifecycle transitions declared in ItemState.h.
* @author Member 2
* @date October 2026
*/
#include "ItemState.h"
#include "InvalidTransition.h"

std::unique_ptr<ItemState> InvalidTransitionState::assign(Item&, Participant*)
{
    throw InvalidTransition("Cannot assign an item in " + name());
}

std::unique_ptr<ItemState> InvalidTransitionState::start(Item&)
{
    throw InvalidTransition("Cannot start an item in " + name());
}

std::unique_ptr<ItemState> InvalidTransitionState::complete(Item&, int)
{
    throw InvalidTransition("Cannot complete an item in " + name());
}

std::unique_ptr<ItemState> InvalidTransitionState::reject(Item&, const std::string&)
{
    throw InvalidTransition("Cannot reject an item in " + name());
}

std::unique_ptr<ItemState> InvalidTransitionState::resolveReview(Item&, Participant*, int)
{
    throw InvalidTransition("Cannot resolve a review in " + name());
}
