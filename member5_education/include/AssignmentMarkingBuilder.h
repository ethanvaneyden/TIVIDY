/**
 * @file AssignmentMarkingBuilder.h
 * @brief Concrete Builder for the Education scenario.
 *
 * Builds the Root -> Container -> Item tree for one assignment:
 *   module (Root) -> assignment (Container) -> student submissions (Submission).
 * Every node is created with a unique_ptr and handed to the WorkRegistry,
 * which owns it and frees it. The tree only holds plain pointers.
 *
 * The generic buildItem() adds to the "current" assignment (the last one
 * built or selected). To add a submission to a specific assignment, as
 * WorkflowInstance::addItem(parentContainerId, ...) does, use
 * buildSubmission(parentContainerId, ...) or selectContainer().
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

        /** @brief The assignment with this id, or throws std::invalid_argument. */
        Container* findAssignment(const std::string& containerId) const;

        /** @brief Create a submission, register it and attach it to the target. */
        void attachSubmission(Container* target,
                              const std::string& itemId,
                              const std::string& name,
                              const std::string& studentId);

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
        virtual void buildItem(const std::string& itemId, const std::string& name) override;

        /**
         * @brief Add a student submission with the student number to the current assignment.
         *
         * buildItem() uses the item id as the student number, because the
         * generic interface carries no student id.
         *
         * @param itemId    Unique submission id.
         * @param name      Student name.
         * @param studentId Student number.
         * @throws std::logic_error if there is no current assignment.
         * @throws std::invalid_argument if the id is already registered.
         * @throws InvalidTransition if the assignment is no longer in Intake.
         */
        void buildSubmission(const std::string& itemId,
                             const std::string& name,
                             const std::string& studentId);

        /**
         * @brief Add a student submission to a named assignment.
         *
         * The assignment becomes the current one, so later buildItem() calls
         * continue there. This is the call behind
         * WorkflowInstance::addItem(parentContainerId, itemId, name, subjectId).
         *
         * @param parentContainerId Id of an assignment already built by this builder.
         * @param itemId            Unique submission id.
         * @param name              Student name.
         * @param studentId         Student number.
         * @throws std::logic_error if buildRoot() has not been called.
         * @throws std::invalid_argument if parentContainerId is not an assignment
         *         of this module, or the item id is already registered.
         * @throws InvalidTransition if the assignment is no longer in Intake.
         */
        void buildSubmission(const std::string& parentContainerId,
                             const std::string& itemId,
                             const std::string& name,
                             const std::string& studentId);

        /**
         * @brief Make an existing assignment the current one.
         *
         * @param containerId Id of an assignment already built by this builder.
         * @throws std::logic_error if buildRoot() has not been called.
         * @throws std::invalid_argument if the id is not an assignment of this module.
         */
        void selectContainer(const std::string& containerId);

        /** @return the root built so far (owned by the registry), or nullptr. */
        virtual Root* getRoot() const override;

        /** @return the assignment currently being filled, or nullptr. */
        Container* getCurrentContainer() const;
};

} // namespace education

