#pragma once
class WorkflowMediator;

/**
 * @brief Base class for objects that communicate through a workflow mediator.
 *
 * The mediator pointer is non-owning; its lifetime is managed elsewhere.
 */
class Colleague
{
protected:
    WorkflowMediator *mediator = nullptr;

public:
    /// @brief Constructs a colleague with no mediator.
    Colleague() = default;

    /// @brief Destroys the colleague.
    virtual ~Colleague() = default;

    /**
     * @brief Sets the mediator used for workflow coordination.
     * @param m Non-owning mediator pointer, or nullptr to clear it.
     */
    void setMediator(WorkflowMediator *m);

    /// @brief Returns the current non-owning mediator, or nullptr if unset.
    WorkflowMediator *getMediator() const;
};
