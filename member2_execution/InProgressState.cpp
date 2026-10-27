#include "InProgressState.h"
#include "Item.h"

/* STUB — real transitions belong to Member 2. */

void InProgressState::assign(Item& item)
{
    
}

void InProgressState::start(Item& item) { /* [TBD] */ }
void InProgressState::complete(Item& item, int value) { /* [TBD] */ }
void InProgressState::reject(Item& item, const std::string& reason) { /* [TBD] */ }

std::string InProgressState::name() const
{
    return "";
}