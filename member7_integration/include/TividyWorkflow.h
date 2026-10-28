/**
 * @file TividyWorkflow.h
 * @brief Facade for the TIVIDY workflow engine.
*/

#pragma once

#include <memory>
#include <string>
#include <vector>

/* [DEPENDS ON] Member 4: WorkCommand.h — needed for CommandResult.
 * I define a local enum here as a placeholder and re-export once the
 * real CommandResult lands. See the "TBD" note below.*/
enum class CommandResult
{
    Success,
    InvalidTransition,
    Rejected,
    IntegrationFailure,
    NotFound
};

class Container;
class WorkflowInstance;
class ParticipantRegistry;
class ExternalSystem;

/**
 * @class TividyWorkflow
 * @brief Single API surface for the REST layer.
 *
 * All workflow logic lives behind this facade. The REST handlers call
 * methods here; no REST handler touches the engine directly.
*/
class TividyWorkflow
{
    private:
        std::unique_ptr<ParticipantRegistry> participants;
        std::vector<std::unique_ptr<WorkflowInstance>> educationInstances;
        std::vector<std::unique_ptr<WorkflowInstance>> healthcareInstances;

        /* [TBD — Member 2] engine. For now, no engine class exists yet,
         * so the facade holds the instances it creates. Once WorkflowEngine
         * is available, replace this with std::unique_ptr<WorkflowEngine>
         * and have it own the instances.*/

        WorkflowInstance* findInstance(const std::string& instanceId);
        const WorkflowInstance* findInstance(const std::string& instanceId) const;

    public:
        TividyWorkflow();
        ~TividyWorkflow();

        TividyWorkflow(const TividyWorkflow&) = delete;
        TividyWorkflow& operator=(const TividyWorkflow&) = delete;

        /* --- Scenario lifecycle ---*/

        /**
         * @brief Start an education workflow instance.
         * @param name scenario assignment name.
         * @return The new instance's ID, or empty on failure.
        */
        std::string startEducation(const std::string& name);

        /**
         * @brief Start a healthcare workflow instance.
        */
        std::string startHealthcare(const std::string& name);

        /**
         * @brief List education instance IDs.
        */
        std::vector<std::string> listEducationInstances() const;

        /**
         * @brief List healthcare instance IDs.
        */
        std::vector<std::string> listHealthcareInstances() const;

        /* --- Inspection ---*/

        /**
         * @brief Get the Container of an instance's tree.
         * @param instanceId Instance to inspect.
         * @return Non-owning pointer to the Container, or nullptr.
        */
        Container* getTree(const std::string& instanceId) const;

        /**
         * @brief Get the history of an instance.
        */
        std::vector<std::string> getHistory(const std::string& instanceId) const;

        /* --- Operator actions (education) ---*/

        CommandResult submitEducation(const std::string& instanceId,
                                        const std::string& containerName);
        CommandResult startItem(const std::string& instanceId,
                                const std::string& itemId);
        CommandResult completeItem(const std::string& instanceId,
                                    const std::string& itemId, int value);
        CommandResult approveContainer(const std::string& instanceId,
                                        const std::string& containerId);
        CommandResult overrideItem(const std::string& instanceId,
                                    const std::string& itemId,
                                    int value, bool accept);

        /* --- Operator actions (healthcare) ---*/

        CommandResult admitPatient(const std::string& instanceId,
                                   const std::string& containerName);
};
