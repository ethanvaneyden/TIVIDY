/**
 * @file ActivePhase.h
 * @brief STUB — replaced by Member 2's full implementation.
*/

#ifndef ACTIVEPHASE_H
#define ACTIVEPHASE_H

#include "WorkflowPhase.h"

class ActivePhase : public WorkflowPhase
{
    public:
        ActivePhase() = default;
        ~ActivePhase() override = default;

        void advance(Container& container) override;
        std::string name() const override;
};

#endif