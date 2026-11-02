/**
 * @file WorkflowScenarioFacade.h
 * @brief Shared implementation of ScenarioFacade for every industry.
 *
 * GoF Facade + Template Method. The class hides builders, directors,
 * mediators, registries and commands behind a handful of operations, and
 * implements each operation once as a fixed skeleton. Everything that
 * differs between industries is obtained from the WorkflowComponentFactory
 * (Abstract Factory), so EducationFacade and HospitalFacade contain almost
 * no code of their own.
 */
#pragma once

#include "ScenarioFacade.h"
#include "WorkflowComponentFactory.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class ParticipantRegistry;
class Participant;
class WorkflowInstance;
class AssignmentStrategy;
class Item;
class Container;

class WorkflowScenarioFacade : public ScenarioFacade
{
    private:
        ParticipantRegistry* participants;                  // non-owning (owned by TividyWorkflow)
        std::unique_ptr<WorkflowComponentFactory> factory;  // the scenario's component family
        std::vector<std::unique_ptr<WorkflowInstance>> instances;
        int instanceCounter;
        std::map<std::string, int> idCounters;              // "<instance>|<prefix>" -> last number
        std::string message;

        WorkflowInstance* findInstance(const std::string& instanceId) const;
        Participant* operatorParticipant() const;
        std::string nextId(const std::string& instanceId, const std::string& prefix,
                           const WorkflowInstance& instance);

        /** @brief Runs body against an instance and converts exceptions to a CommandResult. */
        CommandResult run(const std::string& instanceId,
                          const std::function<void(WorkflowInstance&)>& body);

        /** @brief Opens the item's container (assigning workers) if it is still in Intake. */
        void openContainerOf(WorkflowInstance& instance, Item& item);

        static std::unique_ptr<AssignmentStrategy> makeStrategy(const std::string& strategyName);

    protected:
        WorkflowScenarioFacade(ParticipantRegistry* participants,
                               std::unique_ptr<WorkflowComponentFactory> componentFactory);

    public:
        ~WorkflowScenarioFacade() override;

        std::string scenarioName() const override;
        std::string start(const std::string& name) override;
        std::vector<std::string> listInstances() const override;
        Root* getTree(const std::string& instanceId) const override;
        std::vector<std::string> getHistory(const std::string& instanceId) const override;

        CommandResult submit(const std::string& instanceId,
                             const std::string& containerName) override;
        CommandResult addContainer(const std::string& instanceId,
                                   const std::string& name) override;
        CommandResult addItem(const std::string& instanceId,
                              const std::string& containerId,
                              const std::string& name,
                              const std::map<std::string, std::string>& attributes) override;
        CommandResult openContainer(const std::string& instanceId,
                                    const std::string& containerId) override;
        CommandResult removeInstance(const std::string& instanceId) override;

        CommandResult startItem(const std::string& instanceId,
                                const std::string& itemId) override;
        CommandResult completeItem(const std::string& instanceId,
                                   const std::string& itemId,
                                   int value) override;
        CommandResult approveContainer(const std::string& instanceId,
                                       const std::string& containerId) override;
        CommandResult overrideItem(const std::string& instanceId,
                                   const std::string& itemId,
                                   int value,
                                   bool accept) override;

        CommandResult setStrategy(const std::string& instanceId,
                                  const std::string& strategyName) override;
        std::string getStrategyName(const std::string& instanceId) const override;
        std::string lastMessage() const override;

    protected:
        /** @brief Hook: what the legacy submit() does. Default adds a container. */
        virtual CommandResult legacySubmit(const std::string& instanceId, const std::string& name);
};
