/**
 * @file HospitalWorkflowFactory.h
 * @brief Abstract Factory implementation for the healthcare scenario.
*/

#pragma once

#include "WorkflowComponentFactory.h"

class WorkflowBuilder;

class HospitalWorkflowFactory : public WorkflowComponentFactory
{
    public:
        HospitalWorkflowFactory() = default;
        ~HospitalWorkflowFactory() override = default;

        std::unique_ptr<WorkflowBuilder> createBuilder() const override;
        std::unique_ptr<AssignmentStrategy> createStrategy() const override;
        std::unique_ptr<ExternalSystem> createExternalSystem() const override;
        std::string scenarioName() const override;
};
