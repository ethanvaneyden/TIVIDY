/**
 * @file EducationFacade.h
 * @brief Concrete ScenarioFacade for the education scenario.
*/

#pragma once

#include "ScenarioFacade.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <functional>

class ParticipantRegistry;
class Participant;
class WorkflowInstance;
class AssignmentStrategy;

namespace education
{
class AssignmentMarkingCoordinator;
}

class EducationFacade : public ScenarioFacade
{
    private:
        ParticipantRegistry* participants;  // non-owning (owned by TividyWorkflow)
        std::vector<std::unique_ptr<WorkflowInstance>> instances;
        std::map<std::string, education::AssignmentMarkingCoordinator*> coordinators; // owned by each instance

        WorkflowInstance* findInstance(const std::string& instanceId) const;
        education::AssignmentMarkingCoordinator* findCoordinator(const std::string& instanceId) const;
        Participant* operatorParticipant() const;

        /** @brief Open the assignment (assign TAs) the first time marking starts. */
        void openIfNeeded(WorkflowInstance* instance,
                          education::AssignmentMarkingCoordinator* coordinator);

        /** @return a new strategy for the name, or null if the name is unknown. */
        static std::unique_ptr<AssignmentStrategy> makeStrategy(const std::string& strategyName);

    public:
        /** @brief Adds the education staff to the registry if they are not there yet. */
        explicit EducationFacade(ParticipantRegistry* participants);
        virtual ~EducationFacade() override;

        std::string scenarioName() const override;
        std::string start(const std::string& name) override;
        std::vector<std::string> listInstances() const override;
        Root* getTree(const std::string& instanceId) const override;
        std::vector<std::string> getHistory(const std::string& instanceId) const override;

        /**
         * @brief Add a submission to the assignment.
         * @param containerName The student's name.
         */
        CommandResult submit(const std::string& instanceId,
                            const std::string& containerName) override;

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

        virtual std::string getStrategyName(const std::string& instanceId) const override;
};
