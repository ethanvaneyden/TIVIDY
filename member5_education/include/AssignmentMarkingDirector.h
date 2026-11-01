/**
 * @file AssignmentMarkingDirector.h
 * @brief Concrete Director for the Education scenario.
 *
 * Extends the generic WorkflowDirector and drives the builder it was given.
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

        /** @brief Module, one assignment and the demo submissions. */
        void buildDemo(WorkflowBuilder& target, const std::string& rootName);

    public:
        explicit AssignmentMarkingDirector(AssignmentMarkingBuilder& workflowBuilder);
        virtual ~AssignmentMarkingDirector();

        /** @brief A module with one assignment and no submissions yet. */
        void buildEmptyWorkflow();

        /** @brief A module with one assignment and the EducationSeed submissions. */
        void buildDemoWorkflow();

        /**
         * @brief Build the demo module on the given builder.
         * @return the root built (owned by the registry).
         */
        virtual Root* buildDemoTree(WorkflowBuilder& target,
                                    const std::string& rootName) override;

        /**
         * @brief Build a tree from an explicit specification.
         * @param target     The builder to drive.
         * @param rootName   Name of the root.
         * @param containers Pairs of (containerId, name).
         * @param items      Triples of (containerId, itemId, name).
         * @return the root built (owned by the registry).
         * @throws std::invalid_argument if an item names an unknown container
         *         (nothing is built).
         */
        virtual Root* buildTree(
            WorkflowBuilder& target,
            const std::string& rootName,
            const std::vector<std::pair<std::string, std::string>>& containers,
            const std::vector<std::tuple<std::string, std::string, std::string>>& items) override;

        static const char* moduleName();
        static const char* assignmentId();
};

}
