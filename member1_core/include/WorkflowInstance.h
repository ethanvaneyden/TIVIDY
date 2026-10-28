#pragma once
#include <memory>
#include <string>
#include <vector>

class Container;
class WorkflowMediator;
class AssignmentStrategy;
class ParticipantRegistry;
class WorkflowBuilder;
class WorkRegistry;

/**
 * @brief Represents one configured workflow instance.
 *
 * The interface groups workflow identity, component tree access, history, and
 * operations for adding work and handling item/container outcomes. Its current
 * implementation is a stub; these operations are not yet implemented.
 */
class WorkflowInstance
{
private:
    std::string instanceId;
    std::string definitionType;

    std::unique_ptr<WorkRegistry> workRegistry;
    std::unique_ptr<WorkflowMediator> coordinator;
    std::unique_ptr<AssignmentStrategy> strategy;
    std::unique_ptr<WorkflowBuilder> builder;
    ParticipantRegistry *participants;
    Container *root;

public:
    /**
     * @brief Constructs a workflow instance from its collaborating services.
     * @param instanceId Unique identifier for this workflow instance.
     * @param definitionType Type of workflow definition being run.
     * @param workRegistry Registry for workflow components.
     * @param root Root container for the workflow tree.
     * @param coordinator Mediator coordinating workflow actions.
     * @param strategy Assignment strategy.
     * @param builder Builder associated with the workflow definition.
     * @param participants Registry of workflow participants.
     */
    WorkflowInstance(
        std::string instanceId,
        std::string definitionType,
        std::unique_ptr<WorkRegistry> workRegistry,
        Container *root,
        std::unique_ptr<WorkflowMediator> coordinator,
        std::unique_ptr<AssignmentStrategy> strategy,
        std::unique_ptr<WorkflowBuilder> builder,
        ParticipantRegistry *participants);

    /// @brief Destroys the workflow instance and its owned collaborators.
    ~WorkflowInstance();

    /// @brief Copying workflow instances is disabled.
    WorkflowInstance(const WorkflowInstance &) = delete;

    /// @brief Copy assignment is disabled.
    WorkflowInstance &operator=(const WorkflowInstance &) = delete;

    /// @brief Returns this workflow instance's identifier.
    const std::string &getInstanceId() const;

    /// @brief Returns the workflow definition type.
    const std::string &getDefinitionType() const;

    /// @brief Returns the root container of the workflow.
    Container *getRoot() const;

    /// @brief Returns the workflow's recorded history entries.
    std::vector<std::string> getHistory() const;

    /**
     * @brief Adds an item to a workflow container.
     * @param itemId Identifier for the new item.
     * @param name Display name for the item.
     * @param subjectId Identifier of the related subject.
     */
    void addItem(const std::string &itemId, const std::string &name, const std::string &subjectId);

    /**
     * @brief Adds a container to the workflow.
     * @param containerId Identifier for the new container.
     * @param name Display name for the container.
     */
    void addContainer(
        const std::string &containerId,
        const std::string &name);

    /**
     * @brief Records completion of an item with its result.
     * @param itemId Identifier of the item to complete.
     * @param value Result value.
     */
    void completeItem(const std::string &itemId, int value);

    /**
     * @brief Approves a container's completed work.
     * @param containerId Identifier of the container to approve.
     */
    void approveContainer(const std::string &containerId);

    /**
     * @brief Rejects a container's completed work.
     * @param containerId Identifier of the container to reject.
     * @param reason Explanation for the rejection.
     */
    void rejectContainer(const std::string &containerId, const std::string &reason);
};
