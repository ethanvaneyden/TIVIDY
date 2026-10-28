/**
 * @file WorkflowBuilder.h
 * @brief Abstract Builder for constructing the workflow tree.
*/

#pragma once

#include <memory>
#include <string>

class Root;
class Container;
class Item;

/**
 * @class WorkflowBuilder
 * @brief GoF Builder interface. Constructs a Root -> Container -> Item tree.
 *
 * Concrete builders (AssignmentMarkingBuilder, HospitalAdmissionBuilder)
 * live in Members 5 and 6's folders. They construct the tree; they do
 * not execute it. Execution is the engine's job.
*/
class WorkflowBuilder
{
    public:
        WorkflowBuilder() = default;
        virtual ~WorkflowBuilder() = default;

        /**
         * @brief Start a new tree.
         * @param rootName Human-readable name for the root.
        */
        virtual void buildRoot(const std::string& rootName) = 0;

        /**
         * @brief Add a container under the current parent.
         * @param containerId Unique identifier.
         * @param name Human-readable name.
        */
        virtual void buildContainer(const std::string& containerId,
                                    const std::string& name) = 0;

        /**
         * @brief Add an item under the current container.
         * @param itemId Unique identifier.
         * @param name Human-readable name.
        */
        virtual void buildItem(const std::string& itemId,
                               const std::string& name) = 0;

        /**
         * @brief Return the constructed root.
         * @return Owning pointer to the new tree.
        */
        virtual std::unique_ptr<Root> getResult() = 0;
};
