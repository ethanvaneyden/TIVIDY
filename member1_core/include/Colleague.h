#pragma once

class WorkflowMediator;

/**
 * @brief Base class for objects that communicate through a Mediator.
 * 
 * Inherited by Component, AssignmentEngine, ApprovalHandler, etc.
 * Holds a non-owning pointer to the Mediator.
 */
class Colleague {
protected:
    WorkflowMediator* mediator = nullptr;

public:
    virtual ~Colleague() = default;

    /**
     * @brief Injects the Mediator into this colleague.
     * @param m Non-owning pointer to the Mediator.
     */
    void setMediator(WorkflowMediator* m) { mediator = m; }

    /**
     * @brief Returns the current Mediator (or nullptr).
     */
    WorkflowMediator* getMediator() const { return mediator; }
};