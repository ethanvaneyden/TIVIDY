#ifndef PARTICIPANTREGISTRY_H
#define PARTICIPANTREGISTRY_H

#include "Participant.h"

#include <map>
#include <string>
#include <memory>
#include <vector>

class ParticipantRegistry
{
private:
    std::map<std::string, std::unique_ptr<Participant>> participants;
    std::map<std::string, int> roleCounters;

public:
    ParticipantRegistry() = default;
    ~ParticipantRegistry() = default;

    Participant *add(std::string name, std::string role);
    Participant *get(const std::string &id) const;

    /**
     * @brief Removes an unreferenced participant by identifier.
     * @param id Participant identifier.
     * @throws std::logic_error If workflow items still refer to the participant.
     *
     * Has no effect if the identifier is not registered.
     */
    void remove(const std::string &id);
    bool contains(const std::string &id) const;
    std::vector<Participant *> getByRole(const std::string role) const;
    size_t size() const;
    std::vector<std::string> getAllIds() const;
};

#endif