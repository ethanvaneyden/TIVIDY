/**
 * @file EducationFacade.h
 * @brief Concrete ScenarioFacade for the education scenario.
*/

#pragma once

#include "ScenarioFacade.h"

#include <memory>
#include <string>
#include <vector>

class ParticipantRegistry;
class Participant;
class WorkflowInstance;

class EducationFacade : public ScenarioFacade
{
    private:
        ParticipantRegistry* participants;  // non-owning (owned by TividyWorkflow)
        std::vector<std::unique_ptr<WorkflowInstance>> instances;

        WorkflowInstance* findInstance(const std::string& instanceId) const;
        Participant* operatorParticipant() const;

    public:
        explicit EducationFacade(ParticipantRegistry* participants);
        virtual ~EducationFacade() override;

        std::string scenarioName() const override;
        std::string start(const std::string& name) override;
        std::vector<std::string> listInstances() const override;
        Root* getTree(const std::string& instanceId) const override;
        std::vector<std::string> getHistory(const std::string& instanceId) const override;

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
};