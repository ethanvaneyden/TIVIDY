#include "AssignmentMarkingBuilder.h"

#include "Container.h"
#include "Root.h"
#include "Submission.h"
#include "WorkRegistry.h"

#include <stdexcept>
#include <utility>

namespace education
{

AssignmentMarkingBuilder::AssignmentMarkingBuilder()
    : registry(nullptr), root(nullptr), currentContainer(nullptr)
{
}

AssignmentMarkingBuilder::AssignmentMarkingBuilder(WorkRegistry& workRegistry)
    : registry(&workRegistry), root(nullptr), currentContainer(nullptr)
{
}

AssignmentMarkingBuilder::~AssignmentMarkingBuilder() {}

void AssignmentMarkingBuilder::setRegistry(WorkRegistry& workRegistry)
{
    registry = &workRegistry;
    root = nullptr;
    currentContainer = nullptr;
}

void AssignmentMarkingBuilder::buildRoot(const std::string& rootName)
{
    if (registry == nullptr)
    {
        throw std::logic_error("buildRoot called before setRegistry");
    }
    std::unique_ptr<Root> node(new Root(rootName, "module", rootName));
    Root* raw = node.get();
    registry->add(std::move(node)); // throws on a duplicate id
    root = raw;
    currentContainer = nullptr;
}

void AssignmentMarkingBuilder::buildContainer(const std::string& containerId,
                                              const std::string& name)
{
    if (root == nullptr)
    {
        throw std::logic_error("buildContainer called before buildRoot");
    }

    std::unique_ptr<Container> node(new Container(containerId, "assignment", name));
    Container* raw = node.get();
    registry->add(std::move(node));
    try
    {
        root->add(raw);
    }
    catch (...)
    {
        registry->remove(containerId); // do not leave an orphan in the registry
        throw;
    }
    currentContainer = raw;
}

void AssignmentMarkingBuilder::buildItem(const std::string& itemId, const std::string& name)
{
    if (currentContainer == nullptr)
    {
        throw std::logic_error("buildItem called before buildContainer");
    }

    std::unique_ptr<Submission> node(new Submission(itemId, name));
    Submission* raw = node.get();
    registry->add(std::move(node));
    try
    {
        currentContainer->add(raw);
    }
    catch (...)
    {
        registry->remove(itemId);
        throw;
    }
}

std::unique_ptr<Root> AssignmentMarkingBuilder::getResult()
{
    // Notes: the registry owns the root, so it cannot be handed out
    // as a unique_ptr. Maybe change the interface (for example Root* getResult())?.
    throw std::logic_error("The registry owns the root; use getRoot()");
}

Root* AssignmentMarkingBuilder::getRoot() const
{
    return root;
}

Container* AssignmentMarkingBuilder::getCurrentContainer() const
{
    return currentContainer;
}

}
