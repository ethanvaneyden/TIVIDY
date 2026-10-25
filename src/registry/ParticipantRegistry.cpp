#include "registry/ParticipantRegistry.h"
#include <utility>
#include <stdexcept>

Participant *ParticipantRegistry::add(std::string name, std::string role)
{
    if (name.empty())
        throw std::invalid_argument("Name is required");
    if (role.empty())
        throw std::invalid_argument("Role is required");

    int counter = ++roleCounters[role];
    std::string id = role + std::to_string(counter);

    std::unique_ptr<Participant> participant(new Participant(id, name, role));

    Participant *raw = participant.get();
    participants.emplace(id, std::move(participant));
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
    participants.erase(id);
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
