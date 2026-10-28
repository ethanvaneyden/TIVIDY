#include "AssignedState.h"
#include "InProgressState.h"
#include "Item.h"
#include <memory>

void AssignedState::start(Item& item)
{
    item.setState(std::unique_ptr<ItemState>(new InProgressState()));
}

std::string AssignedState::name() const { return "Assigned"; }
