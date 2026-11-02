#include "StartItemCommand.h"

#include "Item.h"

#include <stdexcept>

StartItemCommand::StartItemCommand(Item* item) : item(item)
{
}

void StartItemCommand::execute()
{
    if (item == nullptr)
        throw std::invalid_argument("Cannot start a null item");

    item->start();
}

std::string StartItemCommand::description() const
{
    return item == nullptr ? "Invalid item start" : "Started item " + item->getId();
}
