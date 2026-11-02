#ifndef TIVIDY_STARTITEMCOMMAND_H
#define TIVIDY_STARTITEMCOMMAND_H

#include "WorkCommand.h"
#include "Item.h"
#include <string>

/** @brief Command: the assignee starts work on an item (Assigned -> InProgress). */
class StartItemCommand : public WorkCommand
{
private:
    Item *item;

public:
    explicit StartItemCommand(Item *item);
    void execute() override;
    std::string description() const override;
};

#endif
