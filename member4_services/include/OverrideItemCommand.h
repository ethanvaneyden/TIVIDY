#ifndef TIVIDY_OVERRIDEITEMCOMMAND_H
#define TIVIDY_OVERRIDEITEMCOMMAND_H

#include "WorkCommand.h"
#include "Item.h"
#include "Participant.h"
#include <string>

class OverrideItemCommand : public WorkCommand {
private:
    Item* item;
    Participant* operatorParticipant;
    int newValue;
    std::string reason;
public:
    OverrideItemCommand(Item* item, Participant* operatorParticipant, int newValue, const std::string& reason);
    void execute() override;
    std::string description() const override;
};

#endif