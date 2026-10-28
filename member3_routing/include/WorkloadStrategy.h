/**
 * @file AssignmentStrategy.h
 * @brief STUB — replaced by Member 3's real implementation.
 */

#pragma once

#include "AssignmentStrategy.h"

class WorkloadStrategy : public AssignmentStrategy
{
public:
    Participant *assign(const std::vector<Participant *> &pool) override;
    std::string name() const override;
};
