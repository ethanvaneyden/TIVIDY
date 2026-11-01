#ifndef TIVIDY_APPROVECONTAINERCOMMAND_H
#define TIVIDY_APPROVECONTAINERCOMMAND_H

#include "WorkCommand.h"
#include "../member1_core/include/Container.h"
#include <string>

class ApproveContainerCommand : public WorkCommand {
private:
    Container* container;
public:
    explicit ApproveContainerCommand(Container* container);
    void execute() override;
    std::string description() const override;
};

#endif
