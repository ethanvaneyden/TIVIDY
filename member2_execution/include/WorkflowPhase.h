#pragma once

#include <memory>
#include <string>

class Container;

/**
 * @brief Abstract State interface for Container phases.
 *
 * advance() returns the next phase, or nullptr when the current phase is
 * terminal. Phase implementations must not call Container::setPhase().
 */
class WorkflowPhase {
public:
    virtual ~WorkflowPhase() = default;

    virtual std::unique_ptr<WorkflowPhase> advance(Container& container) = 0;

    virtual std::string name() const = 0;
};
