#include "AssignedState.h"
#include "Item.h"

/* STUB — real transitions belong to Member 2. */

void AssignedState::assign(Item& item)
{
    
}

void AssignedState::start(Item& item) { /* [TBD] */ }
void AssignedState::complete(Item& item, int value) { /* [TBD] */ }
void AssignedState::reject(Item& item, const std::string& reason) { /* [TBD] */ }

std::string AssignedState::name() const
{
    return "";
}