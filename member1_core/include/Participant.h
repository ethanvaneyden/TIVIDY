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
    Participant(std::string id, std::string name, std::string role)
        : id(std::move(id)), name(std::move(name)), role(std::move(role)), pendingTasks(0) {}

    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::string& getRole() const { return role; }
    int getPendingTasks() const { return pendingTasks; }
    void incrementPendingTasks() { ++pendingTasks; }
    void decrementPendingTasks() { if (pendingTasks > 0) --pendingTasks; }
};