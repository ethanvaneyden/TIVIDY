#include "RejectedState.h"
#include "CompletedState.h"
#include "Item.h"
#include <memory>

void RejectedState::complete(Item& item, int)
{
    item.setState(std::unique_ptr<ItemState>(new CompletedState()));
}

std::string RejectedState::name() const { return "Rejected"; }
