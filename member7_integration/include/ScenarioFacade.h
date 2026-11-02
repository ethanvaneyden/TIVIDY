/**
 * @file ScenarioFacade.h
 * @brief Abstract base for per-scenario facades.
 *
 * Each scenario (education, healthcare, and any future scenario) exposes
 * the same operations through this interface. The top-level TividyWorkflow
 * facade delegates to the appropriate ScenarioFacade based on the instance ID.
 *
 * This satisfies the Open/Closed Principle: adding a new scenario means
 * writing a new subclass, with no changes to TividyWorkflow or the REST layer.
*/

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Result of a command executed through the facade.
 *
 * This is the stable contract between the facade chain and the REST layer.
 * Each value maps to one HTTP status in RestServer.
*/
enum class CommandResult
{
    Success,
    InvalidTransition,
    Rejected,
    IntegrationFailure,
    NotFound
};

class Root;
class WorkflowInstance;
class ParticipantRegistry;

/**
 * @class ScenarioFacade
 * @brief Abstract facade for one workflow scenario.
*/
class ScenarioFacade
{
    public:
        ScenarioFacade() = default;
        virtual ~ScenarioFacade() = default;

        ScenarioFacade(const ScenarioFacade&) = delete;
        ScenarioFacade& operator=(const ScenarioFacade&) = delete;

        /** @brief Scenario identifier, e.g. "education" or "healthcare".*/
        virtual std::string scenarioName() const = 0;

        /**
         * @brief Start a new instance of this scenario.
         * @param name Display name for the instance.
         * @return The new instance's ID.
        */
        virtual std::string start(const std::string& name) = 0;

        /** @brief List all instance IDs managed by this facade.*/
        virtual std::vector<std::string> listInstances() const = 0;

        /** @brief Return the tree root for an instance, or nullptr.*/
        virtual Root* getTree(const std::string& instanceId) const = 0;

        /** @brief Return the history strings for an instance.*/
        virtual std::vector<std::string> getHistory(const std::string& instanceId) const = 0;

        /**
         * @brief Legacy convenience used by the first UI prototype.
         *
         * Education: adds a submission (named containerName) to the first
         * assignment. Healthcare: adds a ward named containerName. New code
         * should use addContainer() / addItem(), which behave the same in
         * every scenario.
        */
        virtual CommandResult submit(const std::string& instanceId,
                                    const std::string& containerName) = 0;

        /**
         * @brief Add a container (assignment / ward) to an instance.
         * @param instanceId Target instance.
         * @param name Display name of the new container.
        */
        virtual CommandResult addContainer(const std::string& instanceId,
                                           const std::string& name) = 0;

        /**
         * @brief Add a work item (submission / patient) to a container in Intake.
         * @param attributes Scenario data such as "age" or "diagnosis".
        */
        virtual CommandResult addItem(const std::string& instanceId,
                                      const std::string& containerId,
                                      const std::string& name,
                                      const std::map<std::string, std::string>& attributes) = 0;

        /**
         * @brief Open a container: Intake -> Active, workers assigned by the strategy.
        */
        virtual CommandResult openContainer(const std::string& instanceId,
                                            const std::string& containerId) = 0;

        /** @brief Remove an instance and everything it owns. */
        virtual CommandResult removeInstance(const std::string& instanceId) = 0;

        /**
         * @brief Human readable explanation of the last non-success result.
         *
         * Empty after a successful call. Used by the REST layer so the UI can
         * show *why* an action was refused.
        */
        virtual std::string lastMessage() const = 0;

        /** @brief Start an item.*/
        virtual CommandResult startItem(const std::string& instanceId,
                                        const std::string& itemId) = 0;

        /** @brief Complete an item with a value.*/
        virtual CommandResult completeItem(const std::string& instanceId,
                                            const std::string& itemId,
                                            int value) = 0;

        /** @brief Approve every item in a container via the approval chain.*/
        virtual CommandResult approveContainer(const std::string& instanceId,
                                                const std::string& containerId) = 0;

        /** @brief Operator override of a Rejected item.*/
        virtual CommandResult overrideItem(const std::string& instanceId,
                                            const std::string& itemId,
                                            int value,
                                            bool accept) = 0;
                                        
        /**
         * @brief Replaces the assignment strategy on a running instance.
         * @param instanceId The instance to modify.
         * @param strategyName One of "Workload", "RoundRobin", "Random".
         * @return Success if applied, NotFound if instance or strategy name is unknown.
        */
        virtual CommandResult setStrategy(const std::string& instanceId,
                                        const std::string& strategyName) = 0;

        /**
         * @brief Reports the active strategy name.
        */
        virtual std::string getStrategyName(const std::string& instanceId) const = 0;
};