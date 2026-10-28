/**
 * @file AssignmentStrategy.h
 * @brief STUB — replaced by Member 3's real implementation.
 */

#ifndef WORKLOAD_H
#define WORKLOAD_H

#include "AssignmentStrategy.h"

class WorkloadStrategy : public AssignmentStrategy
{
public:
        Participant *assign(const std::vector<Participant *> &pool) override;
    std::string name() const override;
};

#endif