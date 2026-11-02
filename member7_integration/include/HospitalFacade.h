/**
 * @file HospitalFacade.h
 * @brief Concrete ScenarioFacade for the healthcare scenario.
*/

#pragma once

#include "ScenarioFacade.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class ParticipantRegistry;
class Participant;
class WorkflowInstance;
class AssignmentStrategy;

namespace hospital
{
class HospitalAdmissionCoordinator;
}

class HospitalFacade : public ScenarioFacade
{
    private:
        ParticipantRegistry* participants;
        std::vector<std::unique_ptr<WorkflowInstance>> instances;
        std::map<std::string, hospital::HospitalAdmissionCoordinator*> coordinators;

        WorkflowInstance* findInstance(const std::string& instanceId) const;
        hospital::HospitalAdmissionCoordinator* findCoordinator(const std::string& instanceId) const;
        Participant* operatorParticipant() const;

        void openIfNeeded(WorkflowInstance* instance,
                          hospital::HospitalAdmissionCoordinator* coordinator);

        static std::unique_ptr<AssignmentStrategy> makeStrategy(const std::string& strategyName);

    public:
        explicit HospitalFacade(ParticipantRegistry* participants);
        virtual ~HospitalFacade() override;

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

        CommandResult setStrategy(const std::string& instanceId,
                                    const std::string& strategyName) override;

        virtual std::string getStrategyName(const std::string& instanceId) const override;
};