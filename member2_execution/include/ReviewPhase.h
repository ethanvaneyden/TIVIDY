/**
 * @file ReviewPhase.h
 * @brief STUB — replaced by Member 2's full implementation.
*/

#ifndef REVIEWPHASE_H
#define REVIEWPHASE_H

#include "WorkflowPhase.h"

class ReviewPhase : public WorkflowPhase
{
    public:
        ReviewPhase() = default;
        ~ReviewPhase() override = default;

        void advance(Container& container) override;
        std::string name() const override;
};

#endif