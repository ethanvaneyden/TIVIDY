#include "InProgressState.h"
#include "CompletedState.h"
#include "Item.h"
#include <memory>

void InProgressState::complete(Item& item, int)
{
    item.setState(std::unique_ptr<ItemState>(new CompletedState()));
}

std::string InProgressState::name() const { return "InProgress"; }
