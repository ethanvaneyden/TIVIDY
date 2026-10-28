/**
 * @file WorkflowEngine.h
 * @brief STUB — replaced by Member 2's full implementation.
*/

#pragma once

#include <memory>
#include <string>

class WorkflowInstance;
class WorkflowBuilder;

class WorkflowEngine
{
    public:
        WorkflowEngine() = default;
        virtual ~WorkflowEngine() = default;

        virtual std::unique_ptr<WorkflowInstance> createInstance(
            const std::string& definitionType,
            const std::string& name,
            std::unique_ptr<WorkflowBuilder> builder) = 0;
};
