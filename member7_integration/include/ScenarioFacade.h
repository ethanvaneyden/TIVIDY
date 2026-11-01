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

#include <memory>
#include <string>
#include <vector>

/**
 * @brief Result of a command executed through the facade.
 *
 * Placed here (rather than in a WorkCommand.h file that may not exist yet)
 * so the whole REST + facade chain has a stable contract. Once Member 4
 * lands WorkCommand.h, this enum can be moved there without changing any
 * caller — the values match.
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
         * @brief Add a container (submission / patient) to an instance.
         * @param instanceId Target instance.
         * @param containerName Display name.
        */
        virtual CommandResult submit(const std::string& instanceId,
                                    const std::string& containerName) = 0;

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
};