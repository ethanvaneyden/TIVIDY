#pragma once

#include "Root.h"

#include <string>

/**
 * @brief Scenario-neutral factory interface for workflow components.
 *
 * Concrete builders create and register education- or healthcare-specific
 * components. The WorkRegistry owns the component tree.
 */
class WorkflowBuilder
{
public:
    /**
     * @brief Destroys the polymorphic builder.
     */
    virtual ~WorkflowBuilder() = default;

    /** @brief Build and register the workflow root. */
    virtual void buildRoot(const std::string& rootName) = 0;

    /**
     * @brief Build and register a scenario-specific container.
     * @param containerId Unique ID for the container.
     * @param name Display name for the container.
     */
    virtual void buildContainer(const std::string& containerId,
                                const std::string& name) = 0;

    /**
     * @brief Build and register a scenario-specific leaf item.
     * @param itemId Unique ID for the item.
     * @param name Display name for the item.
     */
    virtual void buildItem(const std::string& itemId,
                           const std::string& name) = 0;

    /** @return the root built so far (owned by the registry), or nullptr. */
    virtual Root* getRoot() const = 0;
};
