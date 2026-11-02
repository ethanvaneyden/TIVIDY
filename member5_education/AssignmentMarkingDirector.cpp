#include "AssignmentMarkingDirector.h"

#include "AssignmentMarkingBuilder.h"
#include "EducationSeed.h"
#include "WorkflowBuilder.h"

#include <map>
#include <stdexcept>

namespace education
{

AssignmentMarkingDirector::AssignmentMarkingDirector(WorkflowBuilder& workflowBuilder)
    : builder(workflowBuilder)
{
}

AssignmentMarkingDirector::~AssignmentMarkingDirector() {}

const char* AssignmentMarkingDirector::moduleName()
{
    return "COS 214";
}

const char* AssignmentMarkingDirector::assignmentId()
{
    return "A1";
}

void AssignmentMarkingDirector::buildEmptyWorkflow()
{
    builder.buildRoot(moduleName());
    builder.buildContainer(assignmentId(), "Assignment 1");
}

void AssignmentMarkingDirector::buildDemoWorkflow()
{
    buildDemo(builder, moduleName());
}

void AssignmentMarkingDirector::buildDemo(WorkflowBuilder& target, const std::string& rootName)
{
    target.buildRoot(rootName);
    target.buildContainer(assignmentId(), "Assignment 1");

    // The student number travels as a generic attribute, so any
    // WorkflowBuilder can be driven and no type check is needed.
    const std::vector<DemoSubmission> submissions = EducationSeed::demoSubmissions();
    for (std::vector<DemoSubmission>::const_iterator s = submissions.begin();
         s != submissions.end(); ++s)
    {
        std::map<std::string, std::string> attributes;
        attributes["studentId"] = s->studentId;
        target.buildItem(s->itemId, s->studentName, attributes);
    }
}

Root* AssignmentMarkingDirector::buildDemoTree(WorkflowBuilder& target,
                                               const std::string& rootName)
{
    buildDemo(target, rootName);
    return target.getRoot();
}

Root* AssignmentMarkingDirector::buildTree(
    WorkflowBuilder& target,
    const std::string& rootName,
    const std::vector<std::pair<std::string, std::string>>& containers,
    const std::vector<std::tuple<std::string, std::string, std::string>>& items)
{
    // Check first, so a bad specification builds nothing.
    for (std::vector<std::tuple<std::string, std::string, std::string>>::const_iterator i = items.begin();
         i != items.end(); ++i)
    {
        bool known = false;
        for (std::vector<std::pair<std::string, std::string>>::const_iterator c = containers.begin();
             c != containers.end(); ++c)
        {
            if (c->first == std::get<0>(*i))
            {
                known = true;
                break;
            }
        }
        if (!known)
        {
            throw std::invalid_argument("Item " + std::get<1>(*i) +
                                        " names an unknown container: " + std::get<0>(*i));
        }
    }

    target.buildRoot(rootName);
    for (std::vector<std::pair<std::string, std::string>>::const_iterator c = containers.begin();
         c != containers.end(); ++c)
    {
        target.buildContainer(c->first, c->second);

        // The container just built is the builder's current one.
        for (std::vector<std::tuple<std::string, std::string, std::string>>::const_iterator i = items.begin();
             i != items.end(); ++i)
        {
            if (std::get<0>(*i) == c->first)
            {
                target.buildItem(std::get<1>(*i), std::get<2>(*i));
            }
        }
    }
    return target.getRoot();
}

} // namespace education
