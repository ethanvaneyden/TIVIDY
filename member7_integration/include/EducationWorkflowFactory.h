/**
 * @file EducationWorkflowFactory.h
 * @brief Abstract Factory implementation for the education scenario.
*/

#pragma once

#include "WorkflowComponentFactory.h"

class WorkflowBuilder;

class EducationWorkflowFactory : public WorkflowComponentFactory
{
    public:
        EducationWorkflowFactory() = default;
        ~EducationWorkflowFactory() override = default;

        std::unique_ptr<WorkflowBuilder> createBuilder() const override;
        std::unique_ptr<AssignmentStrategy> createStrategy() const override;
        std::string scenarioName() const override;
};
