#pragma once


#include "Participant.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <map>

/**
 * @brief Owns the shared staff pool.
 * 
 * Used by the AssignmentEngine (to find workers) and the Chain of Responsibility 
 * (to find approvers). Returns non-owning pointers.
 */
class ParticipantRegistry {
private:
    /// @brief The actual storage mapping ID to owned Participant.
    std::unordered_map<std::string, std::unique_ptr<Participant>> participants;
    /// @brief Next role-specific ordinal used to generate participant IDs.
    std::map<std::string, unsigned> nextIds;

public:
    /// @brief Constructs an empty registry.
    ParticipantRegistry();

    /// @brief Destroys the registry and all owned participants.
    ~ParticipantRegistry();

    // Prevent copying to avoid accidental double-ownership of unique_ptrs
    ParticipantRegistry(const ParticipantRegistry&) = delete;
    ParticipantRegistry& operator=(const ParticipantRegistry&) = delete;

    /**
     * @brief Takes ownership of a participant and registers it.
     * @param p The unique_ptr transferring ownership.
     * @return Non-owning raw pointer to the registered participant.
     * @throws std::invalid_argument If p is null or a duplicate ID exists.
     */
    Participant* add(std::unique_ptr<Participant> p);

    /**
     * @brief Creates and registers a participant with a role-based ID.
     * @param name Participant display name.
     * @param role Participant role.
     * @return Non-owning pointer to the registered participant.
     * @throws std::invalid_argument If the name or role is empty.
     */
    Participant* add(const std::string& name, const std::string& role);

    /**
     * @brief Looks up a participant by ID.
     * @param id The participant's unique ID.
     * @return Non-owning pointer, or nullptr if not found.
     */
    Participant* get(const std::string& id) const;

    /**
     * @brief Removes a participant from the registry.
     * @param id The participant's unique ID.
     * @throws std::logic_error If the participant has pending tasks.
     */
    void remove(const std::string& id);

    /**
     * @brief Checks if an ID exists in the registry.
     */
    bool contains(const std::string& id) const;

    /**
     * @brief Returns a list of all participants matching a specific role.
     * @param role The role to filter by (e.g., "TA").
     * @return A vector of non-owning pointers.
     */
    std::vector<Participant*> getByRole(const std::string& role) const;

    /// @brief Returns the total number of registered participants.
    size_t size() const;

    /**
     * @brief Returns IDs for all registered participants.
     * @return The IDs in unspecified order.
     */
    std::vector<std::string> getAllIds() const;
};