/**
 * @file IntakePhase.h
 * @brief STUB — replaced by Member 2's full implementation.
*/

#ifndef INTAKEPHASE_H
#define INTAKEPHASE_H

#include "WorkflowPhase.h"

class IntakePhase : public WorkflowPhase
{
    public:
        IntakePhase() = default;
        ~IntakePhase() override = default;

        void advance(Container& container) override;
        std::string name() const override;
};

#endif