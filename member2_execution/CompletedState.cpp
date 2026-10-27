#include "CompletedState.h"
#include "Item.h"

/* STUB — real transitions belong to Member 2. */

void CompletedState::assign(Item& item)
{
    
}

void CompletedState::start(Item& item) { /* [TBD] */ }
void CompletedState::complete(Item& item, int value) { /* [TBD] */ }
void CompletedState::reject(Item& item, const std::string& reason) { /* [TBD] */ }

std::string CompletedState::name() const
{
    return "";
}