/**
 * @file AssignmentMarkingBuilder.h
 * @brief Concrete Builder for the Education scenario.
 *
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
        bool runtimeMode; /* Root came from the registry, not from buildRoot(). */

        /** @brief Use the registry's Root when buildRoot() was never called here. */
        void adoptRootFromRegistry();

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
        virtual void setRegistry(WorkRegistry& workRegistry) override;

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
         * @brief Add a submission under the current assignment.
         *
         * @throws std::logic_error if there is no current assignment.
         * @throws std::invalid_argument if the id is already registered.
         */
        virtual void buildItem(const std::string& itemId, const std::string& name) override;

        using WorkflowBuilder::buildItem;

        /**
         * @brief Build a submission from generic attributes.
         *
         * "subjectId" (or "studentId") is the student number. Other
         * attributes are stored on the submission for display.
         */
        virtual void buildItem(const std::string& itemId,
                               const std::string& name,
                               const std::map<std::string, std::string>& attributes) override;

        /**
         * @brief Add a submission with full student data under the current assignment.
         * @param itemId    Unique submission id.
         * @param name      Student name.
         * @param studentId Student number.
         * @throws InvalidTransition if the assignment is no longer in Intake.
         */
        void buildSubmission(const std::string& itemId,
                             const std::string& name,
                             const std::string& studentId);

        /**
         * @brief Add a submission under a named assignment and make it current.
         * @param parentContainerId Id of an assignment of this module.
         * @param itemId            Unique submission id.
         * @param name              Student name.
         * @param studentId         Student number.
         * @throws std::invalid_argument if the parent is not an assignment of
         *         this module, or the id is already registered.
         * @throws InvalidTransition if the assignment is no longer in Intake.
         */
        void buildSubmission(const std::string& parentContainerId,
                             const std::string& itemId,
                             const std::string& name,
                             const std::string& studentId);

        /**
         * @brief Make an existing assignment the current one.
         * @throws std::invalid_argument if the id is not an assignment of this module.
         */
        void selectContainer(const std::string& containerId);

        /** @return the root built so far (owned by the registry), or nullptr. */
        virtual Root* getRoot() const override;

        /** @return the assignment currently being filled, or nullptr. */
        Container* getCurrentContainer() const;
};

}

