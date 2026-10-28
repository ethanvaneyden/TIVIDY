#ifndef PARTICIPANT_H
#define PARTICIPANT_H
#include <cstddef>
#include <map>
#include <string>

class Item;
class ParticipantRegistry;

/**
 * @brief Represents a person who participates in workflow tasks.
 *
 * A participant has an identifier, name, role, and pending-task count.
 */
class Participant
{
private:
    std::string id;
    std::string name;
    std::string role;
    int pendingTasks;
    std::map<Item*, std::size_t> itemReferences;

    void addItemReference(Item* item);
    void removeItemReference(Item* item);
    bool hasItemReferences() const;

    friend class Item;
    friend class ParticipantRegistry;

public:
    /**
     * @brief Constructs a participant with no pending tasks.
     * @param id Participant identifier.
     * @param name Participant's display name.
     * @param role Participant's workflow role.
     */
    Participant(std::string id, std::string name, std::string role);

    /**
     * @brief Detaches any items that still refer to this participant.
     */
    ~Participant();

    /// @brief Returns the participant identifier.
    const std::string &getId() const;

    /// @brief Returns the participant's display name.
    const std::string &getName() const;

    /// @brief Returns the participant's workflow role.
    const std::string &getRole() const;

    /// @brief Updates the participant's display name.
    /// @param name New display name.
    void setName(const std::string &name);

    /// @brief Returns the number of tasks currently pending for this participant.
    int getPendingTasks();

    /// @brief Increments the pending-task count.
    void incrementPendingTasks();

    /**
     * @brief Decrements the pending-task count when it is positive.
     *
     * The count remains unchanged when it is already zero.
     */
    void decrementPendingTasks();
};

#endif