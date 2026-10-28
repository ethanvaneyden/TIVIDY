#include "CompletedState.h"
#include "RejectedState.h"
#include "Item.h"
#include <memory>

void CompletedState::reject(Item& item, const std::string&)
{
    item.setState(std::unique_ptr<ItemState>(new RejectedState()));
}

std::string CompletedState::name() const { return "Completed"; }
