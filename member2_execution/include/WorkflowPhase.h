#pragma once

#include <string>
class Container;

// Phase changes are validated here. Scenario policies and approvals are
// coordinated by the mediator, rather than hard-coded into these phases.
class WorkflowPhase
{
public:
    virtual ~WorkflowPhase() = default;
    virtual void advance(Container& container) = 0;
    virtual std::string name() const = 0;
};
