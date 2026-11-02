#ifndef TIVIDY_BATCHAPPROVECOMMAND_H
#define TIVIDY_BATCHAPPROVECOMMAND_H

#include "WorkCommand.h"
#include "Container.h"
#include "Participant.h"
#include <string>

/**
 * @brief Command: request batch approval of a container. The Mediator runs
 *        every completed item through the approval Chain of Responsibility.
 */
class BatchApproveCommand : public WorkCommand
{
private:
    Container *container;
    Participant *operatorParticipant;

public:
    BatchApproveCommand(Container *container, Participant *operatorParticipant);
    void execute() override;
    std::string description() const override;
};

#endif
