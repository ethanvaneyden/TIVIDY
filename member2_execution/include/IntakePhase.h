#pragma once

#include "WorkflowPhase.h"

class IntakePhase : public WorkflowPhase
{
public:
    void advance(Container& container) override;
    std::string name() const override;
};
