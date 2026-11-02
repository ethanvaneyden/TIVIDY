/**
 * @file EducationWorkflowFactory.h
 * @brief Abstract Factory implementation for the education scenario.
*/

#pragma once

#include "WorkflowComponentFactory.h"

class EducationWorkflowFactory : public WorkflowComponentFactory
{
    public:
        EducationWorkflowFactory() = default;
        ~EducationWorkflowFactory() override = default;

        std::unique_ptr<WorkflowBuilder> createBuilder() const override;
        std::unique_ptr<WorkflowDirector> createDirector(WorkflowBuilder& builder) const override;
        std::unique_ptr<WorkflowMediator> createCoordinator(
            ParticipantRegistry& participants,
            std::unique_ptr<AssignmentStrategy> strategy) const override;
        std::unique_ptr<AssignmentStrategy> createStrategy() const override;
        void seedParticipants(ParticipantRegistry& participants) const override;
        std::string scenarioName() const override;
        std::string instancePrefix() const override;
        std::string containerPrefix() const override;
        std::string itemPrefix() const override;
        std::string operatorRole() const override;
        bool isEnterableValue(int value) const override;
        bool isSettleableValue(int value) const override;
};
