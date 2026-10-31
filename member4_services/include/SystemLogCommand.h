#ifndef TIVIDY_SYSTEMLOGCOMMAND_H
#define TIVIDY_SYSTEMLOGCOMMAND_H

#include <string>
#include "WorkCommand.h"

class SystemLogCommand : public WorkCommand {
private:
    std::string message;

public:
    explicit SystemLogCommand(const std::string& message);
    void execute() override;
    std::string description() const override;
};

#endif