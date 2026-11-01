/**
 * @file AssignmentMarkingDirector.h
 * @brief Director for the Education scenario: the "recipes" for a workflow.
 *
 * Drives any WorkflowBuilder (normally AssignmentMarkingBuilder) through a
 * fixed sequence of build steps.
 */

#pragma once

#include <string>

class WorkflowBuilder;

namespace education
{

class AssignmentMarkingDirector
{
    private:
        WorkflowBuilder& builder;

    public:
        explicit AssignmentMarkingDirector(WorkflowBuilder& workflowBuilder);

        /** @brief A module with one assignment and no submissions yet. */
        void buildEmptyWorkflow();

        /**
         * @brief A module with one assignment and the demo submissions from
         *        EducationSeed (one for every mark band).
         */
        void buildDemoWorkflow();

        static const char* moduleName();
        static const char* assignmentId();
};

}

