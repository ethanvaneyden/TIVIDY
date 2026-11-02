#pragma once

#include "WorkflowPhase.h"

class PublishPhase : public WorkflowPhase
{
public:
    std::unique_ptr<WorkflowPhase> advance(Container& container) override;
    std::string name() const override;
};
