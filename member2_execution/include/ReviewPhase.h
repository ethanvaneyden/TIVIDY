#ifndef REVIEWPHASE_H
#define REVIEWPHASE_H

#include "WorkflowPhase.h"

class ReviewPhase : public WorkflowPhase
{
public:
    void advance(Container& container) override;
    std::string name() const override;
};

#endif
