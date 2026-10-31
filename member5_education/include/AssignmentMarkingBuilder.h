/**
 * @file AssignmentMarkingBuilder.h
 * @brief Concrete Builder for the Education scenario.
 * The registry owns the components built here; accessors return non-owning
 * pointers into that registry.
 */

#pragma once

#include "WorkflowBuilder.h"

#include <memory>
#include <string>

class WorkRegistry;
class Root;
class Container;

namespace education
{

class AssignmentMarkingBuilder : public WorkflowBuilder
{
    private:
        WorkRegistry* registry;
        Root* root;
        Container* currentContainer;

    public:
        /**
         * @brief Create a builder with no registry yet.
         *
         * This is what a factory can create with no arguments. Call
         * setRegistry() before buildRoot().
         */
        AssignmentMarkingBuilder();

        /** @param workRegistry Registry that will own every node built here. */
        explicit AssignmentMarkingBuilder(WorkRegistry& workRegistry);
        virtual ~AssignmentMarkingBuilder();

        /** @brief Set the registry that will own every node built from now on. */
        void setRegistry(WorkRegistry& workRegistry);

        /**
         * @brief Start a new module.
         * @throws std::logic_error if no registry has been set.
         */
        virtual void buildRoot(const std::string& rootName) override;

        /**
         * @brief Add an assignment under the module and make it current.
         * @throws std::logic_error if buildRoot() has not been called.
         * @throws std::invalid_argument if the id is already registered.
         */
        virtual void buildContainer(const std::string& containerId,
                                    const std::string& name) override;

        /**
         * @brief Add a student submission under the current assignment.
         * @throws std::logic_error if there is no current assignment.
         * @throws std::invalid_argument if the id is already registered.
         */
        virtual void buildItem(const std::string& itemId,
                               const std::string& name) override;

        /** @return the root built so far (owned by the registry), or nullptr. */
        virtual Root* getRoot() const override;

        /** @return the assignment currently being filled, or nullptr. */
        Container* getCurrentContainer() const;
};

}
