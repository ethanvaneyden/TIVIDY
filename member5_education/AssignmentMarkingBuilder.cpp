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

Container* AssignmentMarkingBuilder::findAssignment(const std::string& containerId) const
{
    if (registry == nullptr || root == nullptr)
    {
        throw std::logic_error("buildRoot must be called before choosing an assignment");
    }

    Container* assignment = dynamic_cast<Container*>(registry->get(containerId));
    if (assignment == nullptr || assignment->getParent() != root)
    {
        throw std::invalid_argument("Unknown assignment: " + containerId);
    }
    return assignment;
}

void AssignmentMarkingBuilder::attachSubmission(Container* target,
                                                const std::string& itemId,
                                                const std::string& name,
                                                const std::string& studentId)
{
    std::unique_ptr<Submission> node(new Submission(itemId, name, studentId));
    Submission* raw = node.get();
    registry->add(std::move(node));
    try
    {
        target->add(raw);
    }
    catch (...)
    {
        registry->remove(itemId); // do not leave an orphan in the registry
        throw;
    }
}

void AssignmentMarkingBuilder::buildItem(const std::string& itemId, const std::string& name)
{
    buildSubmission(itemId, name, itemId);
}

void AssignmentMarkingBuilder::buildSubmission(const std::string& itemId,
                                               const std::string& name,
                                               const std::string& studentId)
{
    if (currentContainer == nullptr)
    {
        throw std::logic_error("buildItem called before buildContainer");
    }
    attachSubmission(currentContainer, itemId, name, studentId);
}

void AssignmentMarkingBuilder::buildSubmission(const std::string& parentContainerId,
                                               const std::string& itemId,
                                               const std::string& name,
                                               const std::string& studentId)
{
    Container* target = findAssignment(parentContainerId);
    attachSubmission(target, itemId, name, studentId);
    currentContainer = target; // only once the submission is attached
}

void AssignmentMarkingBuilder::selectContainer(const std::string& containerId)
{
    currentContainer = findAssignment(containerId);
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
