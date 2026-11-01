/**
 * @class WorkloadStrategy
 * @brief Concrete Strategy: chooses the worker with the fewest pending tasks.
 *
 * Stateless: reads Participant::getPendingTasks() on every call. It only
 * balances correctly if Item increments the count when an item is assigned
 * and decrements it when the item is completed or rejected.
 */

#pragma once

#include "AssignmentStrategy.h"

class WorkloadStrategy : public AssignmentStrategy
{
public:
    /**
     * @brief Returns the worker with the lowest pending-task count.
     * @param pool Candidate workers.
     * @return The least-loaded worker (the first one wins a tie),
     *         or nullptr if the pool is empty.
     */
    Participant *assign(const std::vector<Participant *> &pool) override;
    /// @brief Returns "Workload".
    std::string name() const override;
};
