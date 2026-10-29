#ifndef TIVIDY_REJECTITEMCOMMAND_H
#define TIVIDY_REJECTITEMCOMMAND_H

#include "WorkCommand.h"
#include "Item.h"
#include <string>

class RejectItemCommand : public WorkCommand {
private:
    Item* item;
    std::string reason;

public:
    RejectItemCommand(Item* item, const std::string& reason);

    void execute() override;
    std::string description() const override;
};

#endif
