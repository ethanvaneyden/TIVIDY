/**
 * @file WorkflowDirector.h
 * @brief Director that orchestrates a WorkflowBuilder.
*/

#pragma once

#include <string>
#include <vector>
#include <tuple>

class Root;
class WorkflowBuilder;

/**
 * @class WorkflowDirector
 * @brief GoF Director. Tells a builder which steps to run and in what order.
 *
 * The director is generic: it drives whichever builder it is given.
 * Scenario-specific tree shapes are defined by the concrete builders.
*/
class WorkflowDirector
{
    public:
        WorkflowDirector() = default;
        ~WorkflowDirector() = default;

        /**
         * @brief Build a small demo tree. Useful for tests and the smoke path.
         * @param builder The builder to drive.
         * @param rootName Name of the root container.
         * @return Non-owning pointer to the constructed, registry-owned tree.
        */
        virtual Root* buildDemoTree(WorkflowBuilder& builder, const std::string& rootName) = 0;

        /**
         * @brief Build a tree from an explicit specification.
         *
         * @param builder        The builder to drive.
         * @param rootName       Name of the root.
         * @param containers     Pairs of (containerId, name).
         * @param items          Triples of (containerId, itemId, name).
         * @return Non-owning pointer to the constructed, registry-owned tree.
        */
        virtual Root* buildTree(
            WorkflowBuilder& builder,
            const std::string& rootName,
            const std::vector<std::pair<std::string, std::string>>& containers,
            const std::vector<std::tuple<std::string, std::string, std::string>>& items) = 0;
};
