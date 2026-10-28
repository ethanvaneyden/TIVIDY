#include "WorkloadStrategy.h"
#include "Participant.h"

Participant *WorkloadStrategy::assign(const std::vector<Participant *> &pool)
{
    Participant *best = nullptr;
    for (Participant *p : pool)
    {
        if (best == nullptr || p->getPendingTasks() < best->getPendingTasks()) // lowest pendingtasks wins

            best = p;
    }
    return best;
}

std::string WorkloadStrategy::name() const
{
    return "Workload";
}