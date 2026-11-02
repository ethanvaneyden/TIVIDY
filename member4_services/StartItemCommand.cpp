#include "StartItemCommand.h"

StartItemCommand::StartItemCommand(Item *item) : item(item)
{
}

void StartItemCommand::execute()
{
    if (item != nullptr)
    {
        item->start();
    }
}

std::string StartItemCommand::description() const
{
    if (item == nullptr)
    {
        return "Invalid item start";
    }
    return "Started work on " + item->getId();
}
