// Participant.h
#pragma once

#include <string>

/**
 * @brief Passive data object representing a workflow participant.
 */
class Participant {
private:
    std::string id;
    std::string name;
    std::string role;
    int pendingTasks; ///< Used by LoadBalancedStrategy.

public:
    /** @brief Creates a participant with its registry-assigned identity. */
    Participant(std::string id, std::string name, std::string role);

    /** @brief Returns this participant's unique ID. */
    const std::string& getId() const;
    /** @brief Returns this participant's display name. */
    const std::string& getName() const;
    /** @brief Returns this participant's role. */
    const std::string& getRole() const;
    /** @brief Returns the count of work items currently assigned. */
    int getPendingTasks() const;
    /** @brief Updates the participant's display name. */
    void setName(const std::string& newName);
    /** @brief Increments the assigned-work count. */
    void incrementPendingTasks();
    /** @brief Decrements the assigned-work count, bounded at zero. */
    void decrementPendingTasks();
};