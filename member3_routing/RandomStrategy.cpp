#include "RandomStrategy.h"
#include "Participant.h"

RandomStrategy::RandomStrategy() : rng(42)
{
}
Participant *RandomStrategy::assign(const std::vector<Participant *> &pool)
{
    if (pool.empty())
        return nullptr;

    std::uniform_int_distribution<size_t> dist(0, pool.size() - 1); // random index
    return pool[dist(rng)];                                         // rng is random number engine
}
std::string RandomStrategy::name() const
{
    return "Random";
}