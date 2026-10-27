#ifndef PUBLISHPHASE_H
#define PUBLISHPHASE_H

#include "WorkflowPhase.h"

class PublishPhase : public WorkflowPhase
{
public:
    void advance(Container& container) override;
    std::string name() const override;
};

#endif
