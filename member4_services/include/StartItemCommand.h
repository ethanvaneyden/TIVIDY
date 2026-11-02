#pragma once

#include "WorkCommand.h"

#include <string>

class Item;

class StartItemCommand : public WorkCommand
{
private:
    Item* item;

public:
    explicit StartItemCommand(Item* item);
    void execute() override;
    std::string description() const override;
};
