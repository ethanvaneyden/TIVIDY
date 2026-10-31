#include "AssignmentMarkingDirector.h"

#include "EducationSeed.h"
#include "WorkflowBuilder.h"

#include <vector>

namespace education
{

AssignmentMarkingDirector::AssignmentMarkingDirector(WorkflowBuilder& workflowBuilder)
    : builder(workflowBuilder)
{
}

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
    buildEmptyWorkflow();

    const std::vector<DemoSubmission> submissions = EducationSeed::demoSubmissions();
    for (std::vector<DemoSubmission>::const_iterator s = submissions.begin();
         s != submissions.end(); ++s)
    {
        builder.buildItem(s->itemId, s->studentName);
    }
}

}
