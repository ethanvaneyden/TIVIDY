/**
 * @file PublishPhase.h
 * @brief STUB — replaced by Member 2's full implementation.
*/

#ifndef PUBLISHPHASE_H
#define PUBLISHPHASE_H

#include "WorkflowPhase.h"

class PublishPhase : public WorkflowPhase
{
    public:
        PublishPhase() = default;
        ~PublishPhase() override = default;

        void advance(Container& container) override;
        std::string name() const override;
};

#endif