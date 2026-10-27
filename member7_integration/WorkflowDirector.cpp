/**
 * @file WorkflowDirector.cpp
 * @brief Implementation of WorkflowDirector.
*/

#include "WorkflowDirector.h"
#include "WorkflowBuilder.h"
#include "Root.h"

#include <stdexcept>

/* [DEPENDS ON] Member 1: Root.h — exists but depends on Component.h,
 * which depends on Iterator.h (Member 3) and WorkflowMediator.h (Member 4).
 * This file compiles once those stubs exist.*/

std::unique_ptr<Root> WorkflowDirector::buildDemoTree(
    WorkflowBuilder& builder,
    const std::string& rootName)
{
    builder.buildRoot(rootName);
    builder.buildContainer("c1", "Sample container");
    builder.buildItem("i1", "Sample item");
    builder.buildItem("i2", "Another item");
    return builder.getResult();
}

std::unique_ptr<Root> WorkflowDirector::buildTree(
    WorkflowBuilder& builder,
    const std::string& rootName,
    const std::vector<std::pair<std::string, std::string>>& containers,
    const std::vector<std::tuple<std::string, std::string, std::string>>& items)
{
    builder.buildRoot(rootName);

    for(const auto& c : containers)
    {
        builder.buildContainer(c.first, c.second);
    }

    for(const auto& i : items)
    {
        /* Items are assumed to attach to whichever container the concrete
         * builder considers current. Concrete builders enforce the rule.*/
        builder.buildItem(std::get<0>(i) + ":" + std::get<1>(i), std::get<2>(i));
    }

    return builder.getResult();
}