#ifndef TIVIDY_ADVANCEPHASECOMMAND_H
#define TIVIDY_ADVANCEPHASECOMMAND_H

#include "WorkCommand.h"
#include "Container.h"
#include <string>

class AdvancePhaseCommand : public WorkCommand {
private:
    Container* container;
public:
    explicit AdvancePhaseCommand(Container* container);
    void execute() override;
    std::string description() const override;
};

#endif
