#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H
#include <memory>
#include <string>
#include <vector>

class Registry;
class Container;
class WorkflowMediator;
class AssignmentStrategy;
class ParticipantRegistry;
class WorkflowBuilder;
class Root;

class WorkflowInstance
{
private:
    std::string instanceId;
    std::string definitionType;

    std::unique_ptr<Registry> registry;
    std::unique_ptr<WorkflowMediator> coordinator;
    std::unique_ptr<AssignmentStrategy> strategy;
    std::unique_ptr<WorkflowBuilder> builder;
    ParticipantRegistry *participants;
    Root *root;

public:
    WorkflowInstance(
        std::string instanceId,
        std::string definitionType,
        std::unique_ptr<Registry> registry,
        Container *root,
        std::unique_ptr<WorkflowMediator> coordinator,
        std::unique_ptr<AssignmentStrategy> strategy,
        std::unique_ptr<WorkflowBuilder> builder,
        ParticipantRegistry *participants);

    ~WorkflowInstance();

    WorkflowInstance(const WorkflowInstance &) = delete;
    WorkflowInstance &operator=(const WorkflowInstance &) = delete;

    const std::string &getInstanceId() const;
    const std::string &getDefinitionType() const;

    Container *getRoot() const;
    std::vector<std::string> getHistory() const;

    void addItem(const std::string &itemId, const std::string &name, const std::string &subjectId);

    void addContainer(
        const std::string &containerId,
        const std::string &name);

    void completeItem(const std::string &itemId, int value);

    void approveContainer(const std::string &containerId);

    void rejectContainer(const std::string &containerId, const std::string &reason);
};

#endif