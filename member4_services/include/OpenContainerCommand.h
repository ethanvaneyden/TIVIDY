#ifndef TIVIDY_OPENCONTAINERCOMMAND_H
#define TIVIDY_OPENCONTAINERCOMMAND_H

#include "WorkCommand.h"
#include "Container.h"
#include <string>

/**
 * @brief Command: open a container (Intake -> Active) and let the Mediator
 *        assign a worker to every item through the active strategy.
 */
class OpenContainerCommand : public WorkCommand
{
private:
    Container *container;
    int assigned;

public:
    explicit OpenContainerCommand(Container *container);
    void execute() override;
    std::string description() const override;

    /** @brief Number of items that received a worker (valid after execute()). */
    int assignedCount() const;
};

#endif
