#include "AssignmentMarkingBuilder.h"

#include "Container.h"
#include "Root.h"
#include "Submission.h"
#include "WorkRegistry.h"

#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace education
{

AssignmentMarkingBuilder::AssignmentMarkingBuilder()
    : registry(nullptr), root(nullptr), currentContainer(nullptr), runtimeMode(false)
{
}

AssignmentMarkingBuilder::AssignmentMarkingBuilder(WorkRegistry& workRegistry)
    : registry(&workRegistry), root(nullptr), currentContainer(nullptr), runtimeMode(false)
{
}

AssignmentMarkingBuilder::~AssignmentMarkingBuilder() {}

void AssignmentMarkingBuilder::setRegistry(WorkRegistry& workRegistry)
{
    registry = &workRegistry;
    root = nullptr;
    currentContainer = nullptr;
    runtimeMode = false;
}

void AssignmentMarkingBuilder::adoptRootFromRegistry()
{
    if (registry == nullptr || root != nullptr)
    {
        return;
    }
    std::vector<std::string> ids = registry->getAllIds();
    for (std::vector<std::string>::const_iterator id = ids.begin(); id != ids.end(); ++id)
    {
        Root* found = dynamic_cast<Root*>(registry->get(*id));
        if (found != nullptr)
        {
            root = found;
            runtimeMode = true;
            return;
        }
    }
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
    runtimeMode = false;
}

void AssignmentMarkingBuilder::buildContainer(const std::string& containerId,
                                              const std::string& name)
{
    adoptRootFromRegistry();
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
        registry->remove(containerId);
        throw;
    }
    currentContainer = raw;
}

Container* AssignmentMarkingBuilder::findAssignment(const std::string& containerId) const
{
    const_cast<AssignmentMarkingBuilder*>(this)->adoptRootFromRegistry();
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
        registry->remove(itemId);
        throw;
    }
}

void AssignmentMarkingBuilder::buildItem(const std::string& itemId, const std::string& name)
{
    adoptRootFromRegistry();
    if (runtimeMode)
    {
        // Runtime use: register only, WorkflowInstance::addItem attaches it.
        if (registry == nullptr)
        {
            throw std::logic_error("buildItem called before setRegistry");
        }
        registry->add(std::unique_ptr<Component>(new Submission(itemId, name, itemId)));
        return;
    }
    buildSubmission(itemId, name, itemId);
}

void AssignmentMarkingBuilder::buildItem(const std::string& itemId,
                                         const std::string& name,
                                         const std::map<std::string, std::string>& attributes)
{
    adoptRootFromRegistry();

    std::string studentId = itemId;
    std::map<std::string, std::string>::const_iterator found = attributes.find("studentId");
    if (found == attributes.end())
    {
        found = attributes.find("subjectId");
    }
    if (found != attributes.end() && !found->second.empty())
    {
        studentId = found->second;
    }

    std::unique_ptr<Submission> node(new Submission(itemId, name, studentId));
    Submission* raw = node.get();
    for (std::map<std::string, std::string>::const_iterator a = attributes.begin();
         a != attributes.end(); ++a)
    {
        if (a->first != "studentId" && a->first != "subjectId")
        {
            raw->setAttribute(a->first, a->second);
        }
    }

    if (runtimeMode)
    {
        // Runtime use: register only, WorkflowInstance::addItem attaches it.
        if (registry == nullptr)
        {
            throw std::logic_error("buildItem called before setRegistry");
        }
        registry->add(std::move(node));
        return;
    }
    if (currentContainer == nullptr)
    {
        throw std::logic_error("buildItem called before buildContainer");
    }
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
