#ifndef RANDOMSTRATEGY_H
#define RANDOMSTRATEGY_H

#include <random>
#include "AssignmentStrategy.h"

class RandomStrategy : public AssignmentStrategy
{
private:
    std::mt19937 rng;

public:
    RandomStrategy();
    Participant *assign(const std::vector<Participant *> &pool) override;
    std::string name() const override;
};

#endif