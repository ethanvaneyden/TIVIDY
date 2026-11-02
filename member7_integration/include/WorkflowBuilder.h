#pragma once

#include "Root.h"

#include <map>
#include <string>

/**
 * @brief Scenario-neutral factory interface for workflow components.
 *
 * GoF Builder - Builder interface. Concrete builders (education / hospital)
 * create and register scenario-specific components step by step. The
 * WorkRegistry owns the component tree.
 */
class WorkRegistry;

class WorkflowBuilder
{
public:
    /**
     * @brief Destroys the polymorphic builder.
     */
    virtual ~WorkflowBuilder() = default;

    /**
     * @brief Set the registry that will own every node built from now on.
     *
     * Lets a factory create a builder without knowing the registry yet.
     * Builders that need no registry may ignore it.
     */
    virtual void setRegistry(WorkRegistry& registry) { (void)registry; }

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

    /**
     * @brief Build a leaf item that carries scenario data.
     *
     * Recognised keys are scenario specific ("subjectId", "diagnosis", "age"
     * ...). Builders ignore keys they do not understand, so callers stay
     * generic. The default implementation ignores the attributes.
     */
    virtual void buildItem(const std::string& itemId,
                           const std::string& name,
                           const std::map<std::string, std::string>& attributes)
    {
        (void)attributes;
        buildItem(itemId, name);
    }

    /** @return the root built so far (owned by the registry), or nullptr. */
    virtual Root* getRoot() const = 0;
};
