/**
 * @file WorkflowPhase.h
 * @brief STUB — replaced by Member 2's full implementation.
*/

#ifndef WORKFLOWPHASE_H
#define WORKFLOWPHASE_H

#include <string>

class Container;

class WorkflowPhase
{
    public:
        WorkflowPhase() = default;
        virtual ~WorkflowPhase() = default;

        virtual void advance(Container& container) = 0;
        virtual std::string name() const = 0;
};

#endif