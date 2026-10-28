/**
 * @file ParticipantRegistry.cpp
 * @brief Implementation of the ParticipantRegistry class.
 * @author Member 1
 */

#include "ParticipantRegistry.h"
#include <stdexcept>

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