/**
 * @file ParticipantRegistry.cpp
 * @brief Implementation of the ParticipantRegistry class.
 * @author Member 1
 */

#include "ParticipantRegistry.h"
#include <stdexcept>
#include <utility>

ParticipantRegistry::ParticipantRegistry() = default;
ParticipantRegistry::~ParticipantRegistry() = default;

Participant* ParticipantRegistry::add(std::unique_ptr<Participant> p) {
    if (!p) {
        throw std::invalid_argument("Cannot add null participant to registry");
    }
    std::string id = p->getId();
    if (participants.count(id)) {
        throw std::invalid_argument("Participant with ID " + id + " already exists");
    }
    
    Participant* raw = p.get();
    participants.emplace(id, std::move(p));
    return raw;
}

Participant* ParticipantRegistry::add(const std::string& name, const std::string& role) {
    if (name.empty() || role.empty())
        throw std::invalid_argument("Participant name and role must not be empty");

    unsigned& nextId = nextIds[role];
    if (nextId == 0) nextId = 1;
    std::string id;
    do {
        id = role + std::to_string(nextId++);
    } while (participants.find(id) != participants.end());

    return add(std::unique_ptr<Participant>(new Participant(id, name, role)));
}

Participant* ParticipantRegistry::get(const std::string& id) const {
    auto it = participants.find(id);
    return it != participants.end() ? it->second.get() : nullptr;
}

void ParticipantRegistry::remove(const std::string& id) {
    auto it = participants.find(id);
    if (it == participants.end()) return;

    // Safety check: Do not remove someone who is actively working
    if (it->second->getPendingTasks() > 0) {
        throw std::logic_error("Cannot remove participant " + id + " because they have pending tasks.");
    }
    if (it->second->hasItemUses())
    {
        throw std::logic_error("Cannot remove participant " + id +
                               " while workflow items still use them.");
    }
    participants.erase(it);
}

bool ParticipantRegistry::contains(const std::string& id) const {
    return participants.count(id) > 0;
}

std::vector<Participant*> ParticipantRegistry::getByRole(const std::string& role) const {
    std::vector<Participant*> result;
    for (const auto& pair : participants) {
        if (pair.second->getRole() == role) {
            result.push_back(pair.second.get());
        }
    }
    return result;
}

size_t ParticipantRegistry::size() const {
    return participants.size();
}

std::vector<std::string> ParticipantRegistry::getAllIds() const {
    std::vector<std::string> ids;
    ids.reserve(participants.size());
    for (const auto& entry : participants)
        ids.push_back(entry.first);
    return ids;
}