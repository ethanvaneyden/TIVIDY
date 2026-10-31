#ifndef TIVIDY_ASSIGNITEMCOMMAND_H
#define TIVIDY_ASSIGNITEMCOMMAND_H

#include "WorkCommand.h"
#include "../member1_core/include/Item.h"
#include "../member1_core/include/Participant.h"
#include <string>

#include "WorkCommand.h"

class AssignItemCommand : public WorkCommand {
private:
    Item* item;
    Participant* newAssignee;
public:
    AssignItemCommand(Item* item, Participant* newAssignee);
    void execute() override;
    std::string description() const override;
};

#endif