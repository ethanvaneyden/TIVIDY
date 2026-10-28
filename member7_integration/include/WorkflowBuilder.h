#pragma once

#include "Container.h"
#include "Item.h"

#include <memory>
#include <string>

/**
 * @brief Scenario-neutral factory interface for workflow components.
 *
 * Concrete builders create education- or healthcare-specific component
 * types. Tree assembly and ownership remain the responsibility of the
 * WorkflowInstance and its WorkRegistry.
 */
class WorkflowBuilder {
public:
    /**
     * @brief Destroys the polymorphic builder.
     */
    virtual ~WorkflowBuilder() = default;

    /**
     * @brief Creates a scenario-specific container.
     * @param containerId Unique ID for the container.
     * @param name Display name for the container.
     * @return Owning pointer to the new container.
     */
    virtual std::unique_ptr<Container> buildContainer(
        const std::string& containerId,
        const std::string& name) = 0;

    /**
     * @brief Creates a scenario-specific leaf item without subject metadata.
     * @param itemId Unique ID for the item.
     * @param name Display name for the item.
     * @return Owning pointer to the new item.
     */
    virtual std::unique_ptr<Item> buildItem(
        const std::string& itemId,
        const std::string& name) = 0;

    /**
     * @brief Creates a scenario-specific leaf item associated with a subject.
     * @param itemId Unique ID for the item.
     * @param name Display name for the item.
     * @param subjectId Student or patient ID, depending on the scenario.
     * @return Owning pointer to the new item.
     *
     * The default implementation supports builders that do not need
     * scenario-specific subject metadata.
     */
    virtual std::unique_ptr<Item> buildItem(
        const std::string& itemId,
        const std::string& name,
        const std::string& subjectId) {
        (void)subjectId;
        return buildItem(itemId, name);
    }
};
