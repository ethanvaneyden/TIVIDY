#pragma once

#include <memory>
#include <string>
#include <vector>

// Forward declarations
class WorkRegistry;
class Root;
class Container;
class Item;
class WorkflowMediator;
class AssignmentStrategy;
class WorkflowBuilder;
class ParticipantRegistry;
class Participant;

/**
 * @brief Represents a single, independent running workflow instance.
 *
 * Owns the WorkRegistry (the tree), the Coordinator (Mediator),
 * the Strategy, and the Builder. Guarantees instance isolation.
 */
class WorkflowInstance {
private:
    std::string instanceId;
    std::string definitionType;
    std::vector<std::string> history;

    // --- Owned Components ---
    std::unique_ptr<WorkRegistry> registry;
    std::unique_ptr<WorkflowMediator> coordinator;
    std::unique_ptr<AssignmentStrategy> strategy;
    std::unique_ptr<WorkflowBuilder> builder;

    // --- Non-Owning References ---
    ParticipantRegistry* participants; // Owned by the Facade
    Root* root;                         // Owned by the WorkRegistry

public:
    WorkflowInstance(
        std::string instanceId,
        std::string definitionType,
        std::unique_ptr<WorkRegistry> registry, // <--- Updated
        Root* root,
        std::unique_ptr<WorkflowMediator> coordinator,
        std::unique_ptr<AssignmentStrategy> strategy,
        std::unique_ptr<WorkflowBuilder> builder,
        ParticipantRegistry* participants
    );

    ~WorkflowInstance();

    WorkflowInstance(const WorkflowInstance&) = delete;
    WorkflowInstance& operator=(const WorkflowInstance&) = delete;

    // --- Metadata Accessors ---
    const std::string& getInstanceId() const;
    const std::string& getDefinitionType() const;
    Root* getRoot() const;
    std::vector<std::string> getHistory() const;

    // --- Runtime Mutations ---
    void addItem(const std::string& parentContainerId,
                 const std::string& itemId,
                 const std::string& name,
                 const std::string& subjectId);

    void startItem(const std::string& itemId);
    void completeItem(const std::string& itemId, int value);
    void rejectItem(const std::string& itemId, const std::string& reason);
    void overrideItem(const std::string& itemId,
                      Participant* operatorParticipant,
                      int newValue,
                      const std::string& reason);

    // --- Batch Operations ---
    void approveContainer(const std::string& containerId, Participant* operatorParticipant);
    void rejectContainer(const std::string& containerId, const std::string& reason);
};