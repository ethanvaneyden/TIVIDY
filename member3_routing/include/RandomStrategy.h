#pragma once

#include <random>
#include "AssignmentStrategy.h"

/**
 * @class RandomStrategy
 * @brief Concrete Strategy: chooses a worker uniformly at random.
 *
 * Needs no seed from the caller. The generator is seeded with a fixed
 * constant inside the class, so runs are repeatable (useful for demos and
 * tests). Change SEED to std::random_device{}() for different picks each run.
 */
class RandomStrategy : public AssignmentStrategy
{
private:
    std::mt19937 rng;

public:
    RandomStrategy();
    /**
     * @brief Returns a uniformly random worker from the pool.
     * @param pool Candidate workers.
     * @return A worker from the pool, or nullptr if the pool is empty.
     */
    Participant *assign(const std::vector<Participant *> &pool) override;
    /// @brief Returns "Random".
    std::string name() const override;
};
