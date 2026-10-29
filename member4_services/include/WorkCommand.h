#ifndef TIVIDY_WORKCOMMAND_H
#define TIVIDY_WORKCOMMAND_H

#include <string>

class WorkCommand {
public:
    virtual void execute() = 0;
    virtual std::string description() const = 0;
    virtual ~WorkCommand() = default;
};

#endif // TIVIDY_WORKCOMMAND_H
