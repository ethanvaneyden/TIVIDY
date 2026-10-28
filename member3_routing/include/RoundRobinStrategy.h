/**
 * @file AssignmentStrategy.h
 * @brief STUB — replaced by Member 3's real implementation.
 */

#pragma once

#include "AssignmentStrategy.h"

class RoundRobinStrategy : public AssignmentStrategy
{
private:
    const Participant *lastPicked;

public:
    RoundRobinStrategy();
    Participant *assign(const std::vector<Participant *> &pool) override;
    std::string name() const override;
};

