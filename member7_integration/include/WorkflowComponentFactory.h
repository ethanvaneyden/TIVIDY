/**
 * @file WorkflowComponentFactory.h
 * @brief Abstract Factory for scenario-specific workflow components.
*/

#pragma once

#include <memory>
#include <string>

class WorkflowBuilder;
class WorkflowDirector;
class WorkflowMediator;
class AssignmentStrategy;
class ParticipantRegistry;

/**
 * @class WorkflowComponentFactory
 * @brief GoF Abstract Factory. Creates a compatible family of components
 *        for one scenario: builder, director, mediator and default strategy.
 *
 * The scenario facade is written only against this interface, so education
 * and healthcare (and any future industry) run on exactly the same engine
 * code. The factory also carries the few scenario constants the engine must
 * not hard-code (id prefixes, operator role, legal value range).
*/
class WorkflowComponentFactory
{
    public:
        WorkflowComponentFactory() = default;
        virtual ~WorkflowComponentFactory() = default;

        /** @brief Create the scenario's builder (registry is set later). */
        virtual std::unique_ptr<WorkflowBuilder> createBuilder() const = 0;

        /** @brief Create the director that knows the scenario's build recipes. */
        virtual std::unique_ptr<WorkflowDirector> createDirector(WorkflowBuilder& builder) const = 0;

        /** @brief Create the scenario's Mediator (coordinator). */
        virtual std::unique_ptr<WorkflowMediator> createCoordinator(
            ParticipantRegistry& participants,
            std::unique_ptr<AssignmentStrategy> strategy) const = 0;

        /** @brief Create the scenario's default assignment strategy. */
        virtual std::unique_ptr<AssignmentStrategy> createStrategy() const = 0;

        /** @brief Register the scenario's staff if they are not registered yet. */
        virtual void seedParticipants(ParticipantRegistry& participants) const = 0;

        /** @brief scenario specific name (e.g. "education", "healthcare"). */
        virtual std::string scenarioName() const = 0;

        /** @brief Prefix for instance ids, e.g. "edu". */
        virtual std::string instancePrefix() const = 0;

        /** @brief Prefix for generated container ids, e.g. "A" or "W". */
        virtual std::string containerPrefix() const = 0;

        /** @brief Prefix for generated item ids, e.g. "SUB" or "TRT". */
        virtual std::string itemPrefix() const = 0;

        /** @brief Role of the participant who settles rejected items. */
        virtual std::string operatorRole() const = 0;

        /** @brief Can this value be entered when completing an item? */
        virtual bool isEnterableValue(int value) const = 0;

        /** @brief Can this value settle a rejected item? */
        virtual bool isSettleableValue(int value) const = 0;
};
