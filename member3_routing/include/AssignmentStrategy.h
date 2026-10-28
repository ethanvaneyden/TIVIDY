/**
 * @file AssignmentStrategy.h
 * @brief Strategy interface for choosing which worker receives an item.
 */

/**
 * @class AssignmentStrategy
 * @brief Abstract Strategy: one algorithm for choosing a worker from a pool.
 *
 * Strategies only decide who gets the work. They know nothing about roles,
 * approval or item state; roles are handled by the Chain of Responsibility,
 * and the pool is built by the caller (e.g. ParticipantRegistry::getByRole).
 *
 * @see RoundRobinStrategy, WorkloadStrategy, RandomStrategy, AssignmentEngine
 */

#pragma once

#include <string>
#include <vector>

class Participant;

class AssignmentStrategy
{
public:
    virtual ~AssignmentStrategy() = default;
    /**
     * @brief Chooses one worker from the pool.
     * @param pool Candidate workers. The engine removes nullptrs before calling.
     * @return The chosen worker, or nullptr if the pool is empty.
     * @note Never owns or deletes the returned Participant.
     */
    virtual Participant *assign(const std::vector<Participant *> &pool) = 0;
    /**
     * @brief Display name of the algorithm
     * @return e.g. "RoundRobin", "Workload", "Random".
     */
    virtual std::string name() const = 0;
};
