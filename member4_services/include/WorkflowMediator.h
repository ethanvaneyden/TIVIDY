#pragma once

/**
 * @file WorkflowMediator.h
 * @brief STUB — replaced by Member 4's full implementation.
*/

#include <string>
#include "Colleague.h"

class WorkflowMediator
{
    public:
        WorkflowMediator();
        virtual ~WorkflowMediator();

        //this function should be pure virtual i assume
        //stub for now
        // virtual void notify(Colleague* sender, const std::string& event) = 0;
        virtual void notify(Colleague* sender, const std::string& event);
};
