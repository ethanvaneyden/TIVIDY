#include "ParticipantRegistry.h"
#include <utility>
#include <stdexcept>
#include <limits>

Participant *ParticipantRegistry::add(std::string name, std::string role)
{
    if (name.empty())
        throw std::invalid_argument("Name is required");
    if (role.empty())
        throw std::invalid_argument("Role is required");

    int counter = roleCounters[role];
    std::string id;
    do
    {
        if (counter == std::numeric_limits<int>::max())
            throw std::overflow_error("Participant ID counter exhausted");
        ++counter;
        id = role + std::to_string(counter);
    } while (participants.count(id) > 0);

    std::unique_ptr<Participant> participant(new Participant(id, name, role));

    Participant *raw = participant.get();
    auto inserted = participants.emplace(id, std::move(participant));
    if (!inserted.second)
        throw std::logic_error("Unable to register participant with a unique ID");
    roleCounters[role] = counter;
    return raw;
}

Participant *ParticipantRegistry::get(const std::string &id) const
{
    auto pair = participants.find(id);
    if (pair == participants.end())
    {
        return nullptr;
    }
    return pair->second.get();
}

void ParticipantRegistry::remove(const std::string &id)
{
    auto participant = participants.find(id);
    if (participant == participants.end())
        return;
    if (participant->second->hasItemReferences())
        throw std::logic_error("Cannot remove a participant referenced by workflow items");
    participants.erase(participant);
}

bool ParticipantRegistry::contains(const std::string &id) const
{
    return participants.count(id) > 0;
}

std::vector<Participant *> ParticipantRegistry::getByRole(const std::string role) const
{
    std::vector<Participant *> p = {};
    for (const auto &pair : participants)
    {
        if (pair.second->getRole() == role)
        {
            p.push_back((pair.second.get()));
        }
    }
    return p;
}

size_t ParticipantRegistry::size() const
{
    return participants.size();
}

std::vector<std::string> ParticipantRegistry::getAllIds() const
{
    std::vector<std::string> ids = {};
    for (const auto &pair : participants)
    {
        ids.push_back(pair.first);
    }
    return ids;
}
