#include "CreatedState.h"
#include "AssignedState.h"
#include "Item.h"
#include <memory>

void CreatedState::assign(Item& item)
{
    item.setState(std::unique_ptr<ItemState>(new AssignedState()));
}

std::string CreatedState::name() const { return "Available"; }
