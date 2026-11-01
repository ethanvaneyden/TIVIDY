/**
 * @class RoundRobinStrategy
 * @brief Concrete Strategy: takes turns, cycling through the pool in order.
 *
 * Stateful: remembers the last worker chosen and picks the one after it,
 * wrapping to the start. The last pick is stored as a pointer rather than an
 * index, so it stays correct if the pool changes between calls. If the last
 * pick is no longer in the pool, the cycle restarts from the first worker.
 */

#pragma once

#include "AssignmentStrategy.h"

class RoundRobinStrategy : public AssignmentStrategy
{
private:
    const Participant *lastPicked;

public:
    /// @brief Creates a strategy with no previous pick (first call returns pool[0]).
    RoundRobinStrategy();

    /**
     * @brief Returns the worker after the previous pick, wrapping around.
     * @param pool Candidate workers.
     * @return The next worker, or nullptr if the pool is empty.
     */
    Participant *assign(const std::vector<Participant *> &pool) override;
    /// @brief Returns "RoundRobin".
    std::string name() const override;
};
