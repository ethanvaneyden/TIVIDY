/**
 * @file WorkflowComponentFactory.h
 * @brief Abstract Factory for scenario-specific workflow components.
*/

#pragma once

#include <memory>
#include <string>

class WorkflowBuilder;
class AssignmentStrategy;

/**
 * @class WorkflowComponentFactory
 * @brief GoF Abstract Factory. Creates a compatible family of components
 *        for one scenario.
 *
 * A family contains a builder, an assignment strategy and an external
 * system adapter. All three are scenario-specific but conform to the
 * same interfaces, so the facade can treat education and healthcare
 * identically.
*/
class WorkflowComponentFactory
{
    public:
        WorkflowComponentFactory() = default;
        virtual ~WorkflowComponentFactory() = default;

        /**
         * @brief Create the scenario's workflow builder.
        */
        virtual std::unique_ptr<WorkflowBuilder> createBuilder() const = 0;

        /**
         * @brief Create the scenario's assignment strategy.
        */
        virtual std::unique_ptr<AssignmentStrategy> createStrategy() const = 0;

        /**
         * @brief scenario specific name (e.g. "education", "healthcare").
        */
        virtual std::string scenarioName() const = 0;
};
