/**
 * @file ActionHistory.h
 * @brief Command invoker that executes work commands and keeps the audit trail.
 *
 * GoF Command - Invoker. Every command is recorded *before* it runs so the
 * trail reads in cause-and-effect order ("Recorded result: 76" followed by the
 * events it caused). If a command throws, it is removed again and a
 * "Denied: ..." line is recorded instead, so invalid actions are visible in
 * the audit history as well as to the caller.
 */
#ifndef TIVIDY_ACTIONHISTORY_H
#define TIVIDY_ACTIONHISTORY_H

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "WorkCommand.h"

class ActionHistory {
private:
    std::vector<std::unique_ptr<WorkCommand>> history;

public:
    /**
     * @brief Records and executes a command; takes ownership.
     * @param command Command to run. A null command is ignored.
     * @throws Whatever the command throws, after recording a "Denied" entry.
     */
    void execute(std::unique_ptr<WorkCommand> command);

    /** @brief Records a plain audit line without running any action. */
    void log(const std::string& message);

    /** @brief Descriptions of every recorded command, oldest first. */
    std::vector<std::string> getHistoryAsStrings() const;

    /** @brief Number of recorded entries. */
    std::size_t size() const;
};

#endif // TIVIDY_ACTIONHISTORY_H
