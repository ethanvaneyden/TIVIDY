#ifndef ACTIVEPHASE_H
#define ACTIVEPHASE_H

#include "WorkflowPhase.h"

class ActivePhase : public WorkflowPhase
{
public:
    void advance(Container& container) override;
    std::string name() const override;
};

#endif
