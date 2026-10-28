#include "RoundRobinStrategy.h"
#include "Participant.h"

RoundRobinStrategy::RoundRobinStrategy()
{
    this->lastPicked = nullptr;
}
Participant *RoundRobinStrategy::assign(const std::vector<Participant *> &pool)
{
    if (pool.empty())
        return nullptr;

    size_t next = 0;   
    for (size_t i = 0; i < pool.size(); ++i)
    {
        if (pool[i] == lastPicked)
        {
            next = (i + 1) % pool.size();
            break;
        }
    }

    lastPicked = pool[next];
    return pool[next];
}
std::string RoundRobinStrategy::name() const
{
    return "RoundRobin";
}