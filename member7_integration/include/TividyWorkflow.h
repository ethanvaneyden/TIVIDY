/**
 * @file TividyWorkflow.h
 * @brief Facade for the TIVIDY workflow engine.
*/
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ScenarioFacade.h" // For CommandResult

class ParticipantRegistry;

/**
 * @class TividyWorkflow
 * @brief Single API surface for the REST layer.
 *
 * All workflow logic lives behind this facade. The REST handlers call
 * methods here; no REST handler touches the engine directly.
 *
 * This top-level facade is a thin router. It owns the scenario-specific
 * facades and delegates all calls to the appropriate one based on the
 * instance ID. This respects SRP and OCP.
*/
class TividyWorkflow
{
    private:
        std::unique_ptr<ParticipantRegistry> participants;
        std::unique_ptr<ScenarioFacade> educationFacade;
        std::unique_ptr<ScenarioFacade> healthcareFacade;

        // Maps an instance ID to the facade that owns it.
        std::unordered_map<std::string, ScenarioFacade*> instanceIndex;

        /**
         * @brief Finds the owning facade for a given instance ID.
         * @param instanceId The instance to look up.
         * @return A non-owning pointer to the facade, or nullptr if not found.
        */
        ScenarioFacade* findFacade(const std::string& instanceId) const;

    public:
        TividyWorkflow();
        ~TividyWorkflow();

        TividyWorkflow(const TividyWorkflow&) = delete;
        TividyWorkflow& operator=(const TividyWorkflow&) = delete;

        /* -- Scenario lifecycle --*/

        /**
         * @brief Start an education workflow instance.
         * @param name The name for the new workflow.
         * @return The new instance's ID, or an empty string on failure.
        */
        std::string startEducation(const std::string& name);

        /**
         * @brief Start a healthcare workflow instance.
         * @param name The name for the new workflow.
         * @return The new instance's ID, or an empty string on failure.
        */
        std::string startHealthcare(const std::string& name);

        /**
         * @brief List all education instance IDs.
        */
        std::vector<std::string> listEducationInstances() const;

        /**
         * @brief List all healthcare instance IDs.
        */
        std::vector<std::string> listHealthcareInstances() const;

        /* -- Inspection --*/

        /**
         * @brief Get the Root component of an instance's tree.
         * @param instanceId The instance to inspect.
         * @return A non-owning pointer to the Root, or nullptr if not found.
        */
        Root* getTree(const std::string& instanceId) const;

        /**
         * @brief Get the execution history of an instance.
         * @param instanceId The instance to inspect.
         * @return A vector of history strings.
        */
        std::vector<std::string> getHistory(const std::string& instanceId) const;

        /* -- Operator actions (routed) --*/

        /**
         * @brief Delegates a submission action to the correct scenario facade.
        */
        CommandResult submit(const std::string& instanceId,
                            const std::string& containerName);

        /**
         * @brief Delegates a start-item action to the correct scenario facade.
        */
        CommandResult startItem(const std::string& instanceId,
                                const std::string& itemId);

        /**
         * @brief Delegates a complete-item action to the correct scenario facade.
        */
        CommandResult completeItem(const std::string& instanceId,
                                    const std::string& itemId, int value);

        /**
         * @brief Delegates an approve-container action to the correct scenario facade.
        */
        CommandResult approveContainer(const std::string& instanceId,
                                        const std::string& containerId);

        /**
         * @brief Delegates an override-item action to the correct scenario facade.
        */
        CommandResult overrideItem(const std::string& instanceId,
                                    const std::string& itemId, int value, bool accept);

        /* -- Scenario-specific actions (routed) --*/

        /**
         * @brief Admit a patient. Calls the submit method on the healthcare facade.
        */
        CommandResult admitPatient(const std::string& instanceId,
                                    const std::string& containerName);
};