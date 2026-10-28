#ifndef PARTICIPANTREGISTRY_H
#define PARTICIPANTREGISTRY_H

#include "Participant.h"

#include <map>
#include <string>
#include <memory>
#include <vector>

/**
 * @brief Owns and provides lookup for registered workflow participants.
 *
 * Participant identifiers are generated from each role and a role-specific
 * increasing counter. Pointers returned by this registry are non-owning.
 * Participants referenced by items cannot be removed; destroying the registry
 * detaches its participants from any items that still refer to them.
 */
class ParticipantRegistry
{
private:
    std::map<std::string, std::unique_ptr<Participant>> participants;
    std::map<std::string, int> roleCounters;

public:
    /// @brief Constructs an empty participant registry.
    ParticipantRegistry() = default;

    /// @brief Destroys the registry and its owned participants.
    ~ParticipantRegistry() = default;

    /**
     * @brief Creates and registers a participant.
     * @param name Non-empty participant name.
     * @param role Non-empty workflow role.
     * @return Non-owning pointer to the newly registered participant.
     * @throws std::invalid_argument If name or role is empty.
     */
    Participant *add(std::string name, std::string role);

    /**
     * @brief Looks up a participant by identifier.
     * @param id Participant identifier.
     * @return Non-owning pointer to the participant, or nullptr if not found.
     */
    Participant *get(const std::string &id) const;

    /**
     * @brief Removes an unreferenced participant by identifier.
     * @param id Participant identifier.
     * @throws std::logic_error If workflow items still refer to the participant.
     *
     * Has no effect if the identifier is not registered.
     */
    void remove(const std::string &id);

    /// @brief Checks whether an identifier is registered.
    bool contains(const std::string &id) const;

    /**
     * @brief Returns all registered participants with the specified role.
     * @param role Role to match.
     * @return Non-owning participant pointers in identifier order.
     */
    std::vector<Participant *> getByRole(const std::string role) const;

    /// @brief Returns the number of registered participants.
    size_t size() const;

    /// @brief Returns all registered identifiers in map order.
    std::vector<std::string> getAllIds() const;
};

#endif