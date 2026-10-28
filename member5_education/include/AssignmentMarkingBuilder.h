/**
 * @file AssignmentMarkingBuilder.h
 * @brief Concrete Builder for the Education scenario.
 * Notes: WorkflowBuilder::getResult() returns unique_ptr<Root>,
 * but the registry owns the root, so there is nothing to hand over. Until the
 * interface changes, getResult() throws and callers use getRoot().
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
        virtual void buildRoot(const std::string& rootName);

        /**
         * @brief Add an assignment under the module and make it current.
         * @throws std::logic_error if buildRoot() has not been called.
         * @throws std::invalid_argument if the id is already registered.
         */
        virtual void buildContainer(const std::string& containerId,
                                    const std::string& name);

        /**
         * @brief Add a student submission under the current assignment.
         * @throws std::logic_error if there is no current assignment.
         * @throws std::invalid_argument if the id is already registered.
         */
        virtual void buildItem(const std::string& itemId, const std::string& name);

        /** @brief Not supported yet: see the Notes above. Always throws std::logic_error. */
        virtual std::unique_ptr<Root> getResult();

        /** @return the root built so far (owned by the registry), or nullptr. */
        Root* getRoot() const;

        /** @return the assignment currently being filled, or nullptr. */
        Container* getCurrentContainer() const;
};

}

