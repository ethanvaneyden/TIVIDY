/**
 * @file AssignmentMarkingDirector.h
 * @brief Director for the Education scenario: the "recipes" for a workflow.
 *
 * Extends the generic WorkflowDirector. It keeps the two education recipes
 * (buildEmptyWorkflow, buildDemoWorkflow) that drive the builder it was
 * created with, and implements the generic buildDemoTree / buildTree for any
 * WorkflowBuilder it is handed.
 */

#pragma once

#include "WorkflowDirector.h"

#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace education
{

class AssignmentMarkingBuilder;

class AssignmentMarkingDirector : public WorkflowDirector
{
    private:
        AssignmentMarkingBuilder& builder;

        /** @brief Module, one assignment and the demo submissions, built on any builder. */
        void buildDemo(WorkflowBuilder& target, const std::string& rootName);

    public:
        explicit AssignmentMarkingDirector(AssignmentMarkingBuilder& workflowBuilder);
        virtual ~AssignmentMarkingDirector();

        /** @brief A module with one assignment and no submissions yet. */
        void buildEmptyWorkflow();

        /**
         * @brief A module with one assignment and the demo submissions from
         *        EducationSeed (one for every mark band).
         */
        void buildDemoWorkflow();

        /**
         * @brief Build the demo module on the given builder.
         *
         * An AssignmentMarkingBuilder also stores each student number; any
         * other builder gets the submissions through buildItem().
         *
         * @return the root built (owned by the registry).
         */
        virtual Root* buildDemoTree(WorkflowBuilder& target,
                                    const std::string& rootName) override;

        /**
         * @brief Build a tree from an explicit specification.
         *
         * Each item names its container. A builder only knows its current
         * container, so every container is built first and then its own
         * items, which puts each item under the container it names.
         *
         * @param target     The builder to drive.
         * @param rootName   Name of the root.
         * @param containers Pairs of (containerId, name).
         * @param items      Triples of (containerId, itemId, name).
         * @return the root built (owned by the registry).
         * @throws std::invalid_argument if an item names a container that is
         *         not in the list. Nothing is built in that case.
         */
        virtual Root* buildTree(
            WorkflowBuilder& target,
            const std::string& rootName,
            const std::vector<std::pair<std::string, std::string>>& containers,
            const std::vector<std::tuple<std::string, std::string, std::string>>& items) override;

        static const char* moduleName();
        static const char* assignmentId();
};

} // namespace education
