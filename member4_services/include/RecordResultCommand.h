#ifndef TIVIDY_RECORDRESULTCOMMAND_H
#define TIVIDY_RECORDRESULTCOMMAND_H

#include "WorkCommand.h"
#include "../member1_core/include/Item.h"
#include <string>

class RecordResultCommand : public WorkCommand {
private:
    Item* item;
    int newValue;
public:
    RecordResultCommand(Item* item, int newValue);
    void execute() override;
    std::string description() const override;
};

#endif
