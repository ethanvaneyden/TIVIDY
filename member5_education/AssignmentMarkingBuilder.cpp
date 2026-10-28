#include "AssignmentMarkingBuilder.h"

#include "Container.h"
#include "Item.h"
#include "Root.h"

#include <stdexcept>
#include <utility>

namespace education
{

namespace
{
    const char* const MODULE_TYPE = "module";
    const char* const ASSIGNMENT_TYPE = "assignment";
    const char* const SUBMISSION_TYPE = "submission";
}

AssignmentMarkingBuilder::AssignmentMarkingBuilder() : currentContainer(nullptr) {}

AssignmentMarkingBuilder::~AssignmentMarkingBuilder() {}

void AssignmentMarkingBuilder::buildRoot(const std::string& rootName)
{
    root.reset(new Root(rootName, MODULE_TYPE, rootName));
    currentContainer = nullptr;
}

void AssignmentMarkingBuilder::buildContainer(const std::string& containerId,
                                              const std::string& name)
{
    if (!root)
    {
        throw std::logic_error("buildContainer called before buildRoot");
    }
    // TODO(Member 1): ownership of the new node is not defined yet.
    Container* container = new Container(containerId, ASSIGNMENT_TYPE, name);
    root->add(container);
    currentContainer = container;
}

void AssignmentMarkingBuilder::buildItem(const std::string& itemId,
                                         const std::string& name)
{
    if (currentContainer == nullptr)
    {
        throw std::logic_error("buildItem called before buildContainer");
    }
    // TODO(Member 1): ownership of the new node is not defined yet.
    currentContainer->add(new Item(itemId, SUBMISSION_TYPE, name));
}

std::unique_ptr<Root> AssignmentMarkingBuilder::getResult()
{
    currentContainer = nullptr;
    return std::move(root);
}

} // namespace education
