#include "CreatedState.h"
#include "Item.h"

/* STUB — real transitions belong to Member 2. */

void CreatedState::assign(Item& item)
{
    /* [TBD — Member 2] item.setState(std::unique_ptr<ItemState>(new AssignedState())); */
}

void CreatedState::start(Item& item) { /* [TBD] */ }
void CreatedState::complete(Item& item, int value) { /* [TBD] */ }
void CreatedState::reject(Item& item, const std::string& reason) { /* [TBD] */ }

std::string CreatedState::name() const
{
    return "Created";
}