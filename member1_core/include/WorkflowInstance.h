#pragma once

#include <memory>
#include <string>
#include <vector>

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

    // --- Owned Components ---
    std::unique_ptr<WorkRegistry> registry;
    std::unique_ptr<WorkflowMediator> coordinator;
    std::unique_ptr<AssignmentStrategy> strategy;
    std::unique_ptr<WorkflowBuilder> builder;

    // --- Non-Owning References ---
    ParticipantRegistry* participants; // Owned by the Facade
    Root* root;                         // Owned by the WorkRegistry

public:
    /**
     * @brief Creates an isolated workflow instance and wires its mediator.
     * @param instanceId Unique workflow instance ID.
     * @param definitionType Scenario identifier.
     * @param registry Registry owning the complete component tree.
     * @param root Non-owning pointer to the root held by registry.
     * @param coordinator Owned typed workflow mediator.
     * @param strategy Owned assignment strategy.
     * @param builder Owned scenario-specific builder.
     * @param participants Non-owning pointer to the facade-owned participant registry.
     * @throws std::invalid_argument If required dependencies are null or root is not registered.
     */
    WorkflowInstance(
        std::string instanceId,
        std::string definitionType,
        std::unique_ptr<WorkRegistry> registry,
        Root* root,
        std::unique_ptr<WorkflowMediator> coordinator,
        std::unique_ptr<AssignmentStrategy> strategy,
        std::unique_ptr<WorkflowBuilder> builder,
        ParticipantRegistry* participants
    );

    /**
     * @brief Destroys this instance and its owned services and registry.
     */
    ~WorkflowInstance();

    /**
     * @brief Copying is disabled to preserve single ownership of the engine.
     * @param other Instance that would otherwise be copied.
     * @throws Not applicable; this operation is deleted.
     */
    WorkflowInstance(const WorkflowInstance&) = delete;

    /**
     * @brief Copy assignment is disabled to preserve single ownership.
     * @param other Instance that would otherwise be assigned.
     * @return Not applicable; this operation is deleted.
     * @throws Not applicable; this operation is deleted.
     */
    WorkflowInstance& operator=(const WorkflowInstance&) = delete;

    /**
     * @brief Returns this instance's unique ID.
     * @return Instance ID.
     */
    const std::string& getInstanceId() const;

    /**
     * @brief Returns the scenario definition name.
     * @return Definition type.
     */
    const std::string& getDefinitionType() const;

    /**
     * @brief Returns the registry-owned root component.
     * @return Non-owning pointer to the root.
     */
    Root* getRoot() const;

    /**
     * @brief Returns the mediator's event history.
     * @return History entries in chronological order.
     */
    std::vector<std::string> getHistory() const;

    /**
     * @brief Builds and attaches a scenario-specific container.
     * @param containerId Unique container ID.
     * @param name Display name.
     * @return Non-owning pointer to the registry-owned container.
     * @throws std::logic_error If the builder does not register a container.
     * @throws std::invalid_argument If the ID is duplicated or the tree shape is invalid.
     */
    Container* addContainer(
        const std::string& containerId,
        const std::string& name);

    /**
     * @brief Builds, registers, and attaches a scenario-specific leaf item.
     * @param parentContainerId Parent container ID.
     * @param itemId Unique item ID.
     * @param name Display name.
     * @param subjectId Student or patient ID.
     * @throws std::logic_error If the builder does not register the item.
     * @throws std::invalid_argument If the ID or tree shape is invalid.
     */
    void addItem(const std::string& parentContainerId,
                 const std::string& itemId,
                 const std::string& name,
                 const std::string& subjectId);

    /**
     * @brief Looks up an item using the registry.
     * @param itemId Item ID.
     * @return Non-owning item pointer, or nullptr if missing or not an item.
     */
    Item* getItem(const std::string& itemId) const;

    /**
     * @brief Looks up a container using the registry.
     * @param containerId Container ID.
     * @return Non-owning container pointer, or nullptr if missing or not a container.
     */
    Container* getContainer(const std::string& containerId) const;

    /**
     * @brief Starts work on a registry-owned item.
     * @param itemId Item ID.
     * @throws std::runtime_error If no item with the ID exists.
     * @throws InvalidTransition If the item's state disallows starting.
     */
    void startItem(const std::string& itemId);

    /**
     * @brief Completes work on a registry-owned item.
     * @param itemId Item ID.
     * @param value Result value.
     * @throws std::runtime_error If no item with the ID exists.
     * @throws InvalidTransition If the item's state disallows completion.
     */
    void completeItem(const std::string& itemId, int value);

    /**
     * @brief Rejects a registry-owned item.
     * @param itemId Item ID.
     * @param reason Rejection explanation.
     * @throws std::runtime_error If no item with the ID exists.
     * @throws InvalidTransition If the item's state disallows rejection.
     */
    void rejectItem(const std::string& itemId, const std::string& reason);

    /**
     * @brief Resolves a rejected item on behalf of an operator.
     * @param itemId Item ID.
     * @param operatorParticipant Non-owning operator pointer.
     * @param newValue Replacement result.
     * @param reason Resolution explanation.
     * @throws std::runtime_error If no item with the ID exists.
     * @throws InvalidTransition If the item is not rejected.
     */
    void overrideItem(const std::string& itemId,
                      Participant* operatorParticipant,
                      int newValue,
                      const std::string& reason);

    /**
     * @brief Delegates container approval to the coordinator.
     * @param containerId Container ID.
     * @param operatorParticipant Non-owning operator pointer.
     * @throws std::runtime_error If no container with the ID exists.
     * @throws InvalidTransition If the approval request is invalid.
     */
    void approveContainer(const std::string& containerId, Participant* operatorParticipant);

    /**
     * @brief Delegates container rejection to the coordinator.
     * @param containerId Container ID.
     * @param reason Rejection explanation.
     * @throws std::runtime_error If no container with the ID exists.
     * @throws InvalidTransition If the rejection request is invalid.
     */
    void rejectContainer(const std::string& containerId, const std::string& reason);
};