#include "RejectedState.h"
#include "Item.h"

/* STUB — real transitions belong to Member 2. */

void RejectedState::assign(Item& item)
{
    
}

void RejectedState::start(Item& item) { /* [TBD] */ }
void RejectedState::complete(Item& item, int value) { /* [TBD] */ }
void RejectedState::reject(Item& item, const std::string& reason) { /* [TBD] */ }

std::string RejectedState::name() const
{
    return "";
}