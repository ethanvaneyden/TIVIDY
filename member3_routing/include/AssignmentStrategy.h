/**
 * @file AssignmentStrategy.h
 * @brief STUB — replaced by Member 3's real implementation.
 */

#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include <string>
#include <vector>

class Participant;

class AssignmentStrategy
{
public:
    virtual ~AssignmentStrategy() = default;
    virtual Participant *assign(const std::vector<Participant *> &pool) = 0;
    virtual std::string name() const = 0;
};

#endif