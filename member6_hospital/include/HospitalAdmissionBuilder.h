/**
 * @file HospitalAdmissionBuilder.h
 * @brief Concrete Builder for the Hospital scenario.
 *
 * The registry owns the components built here; accessors return non-owning
 * pointers into that registry.
 */

#pragma once

#include "WorkflowBuilder.h"

#include <string>

class WorkRegistry;
class Root;
class Container;

namespace hospital
{

class HospitalAdmissionBuilder : public WorkflowBuilder
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
        HospitalAdmissionBuilder();

        /** @param workRegistry Registry that will own every node built here. */
        explicit HospitalAdmissionBuilder(WorkRegistry& workRegistry);
        virtual ~HospitalAdmissionBuilder();

        /** @brief Set the registry that will own every node built from now on. */
        void setRegistry(WorkRegistry& workRegistry);

        /**
         * @brief Start a new hospital.
         * @throws std::logic_error if no registry has been set.
         */
        virtual void buildRoot(const std::string& rootName) override;

        /**
         * @brief Add a ward under the hospital and make it current.
         * @throws std::logic_error if buildRoot() has not been called.
         * @throws std::invalid_argument if the id is already registered.
         */
        virtual void buildContainer(const std::string& containerId,
                                    const std::string& name) override;

        /**
         * @brief Add a treatment under the current ward.
         *
         * The generic interface carries no patient id, so the item id doubles
         * as the patient id.
         *
         * @throws std::logic_error if there is no current ward.
         * @throws std::invalid_argument if the id is already registered.
         */
        virtual void buildItem(const std::string& itemId,
                               const std::string& name) override;

        /**
         * @brief Add a treatment with full patient data under the current ward.
         * @param itemId    Unique treatment id.
         * @param name      Patient name.
         * @param patientId Patient identifier.
         * @param diagnosis Working diagnosis.
         */
        void buildTreatment(const std::string& itemId,
                            const std::string& name,
                            const std::string& patientId,
                            const std::string& diagnosis);

        /** @return the root built so far (owned by the registry), or nullptr. */
        virtual Root* getRoot() const override;

        /** @return the ward currently being filled, or nullptr. */
        Container* getCurrentContainer() const;
};

}
