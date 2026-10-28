#include "AssignmentMarkingBuilder.h"

#include "Container.h"
#include "Item.h"
#include "Root.h"
#include "Submission.h"

#include <stdexcept>
#include <utility>

namespace education
{

namespace
{
    const char* const MODULE_TYPE = "module";
    const char* const ASSIGNMENT_TYPE = "assignment";
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
    std::unique_ptr<Container> container(new Container(containerId, ASSIGNMENT_TYPE, name));
    currentContainer = container.get();
    root->add(std::unique_ptr<Component>(container.release()));
}

void AssignmentMarkingBuilder::buildItem(const std::string& itemId,
                                         const std::string& name)
{
    if (currentContainer == nullptr)
    {
        throw std::logic_error("buildItem called before buildContainer");
    }
    currentContainer->add(std::unique_ptr<Component>(new Submission(itemId, name)));
}

std::unique_ptr<Root> AssignmentMarkingBuilder::getResult()
{
    currentContainer = nullptr;
    return std::move(root);
}

} // namespace education
