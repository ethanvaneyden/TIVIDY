/**
 * @file HospitalAuditCommand.h
 * @brief Command that records a hospital action in the shared ActionHistory.
 */

#pragma once

#include "WorkCommand.h"

#include <string>

namespace hospital
{

/**
 * @class HospitalAuditCommand
 * @brief Audit entry for an action that has already been applied.
 *
 * The workflow change itself happens through the Item and Container state
 * machines. This command carries the description of that change so that
 * ActionHistory keeps it with every other recorded action; execute() has
 * nothing left to do.
 */
class HospitalAuditCommand : public WorkCommand
{
    private:
        std::string entry;

    public:
        explicit HospitalAuditCommand(const std::string& description) : entry(description) {}

        void execute() override {}

        std::string description() const override { return entry; }
};

}
