#ifndef COLLEAGUE_H
#define COLLEAGUE_H
class WorkflowMediator;

/// @brief This class represents any part of the program that needs to be mediated
class Colleague
{
protected:
    WorkflowMediator *mediator = nullptr;

public:
    Colleague() = default;
    virtual ~Colleague() = default;

    /// @brief Sets the items mediator
    /// @param m 
    void setMediator(WorkflowMediator *m);

    /// @brief Returns the items current mediator
    /// @return WorkflowMediator*
    WorkflowMediator *getMediator() const;
};

#endif