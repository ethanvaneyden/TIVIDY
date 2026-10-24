#ifndef PARTICIPANTREGISTRY_H
#define PARTICIPANTREGISTRY_H

#include "participant/Participant.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

class ParticipantRegistry
{
private:
    std::unordered_map<std::string, std::unique_ptr<Participant>> participants;

public:
    ParticipantRegistry() = default;
    ~ParticipantRegistry() = default;

    Participant *add(std::unique_ptr<Participant> participant);
    Participant *get(const std::string &id) const;
    void remove(const std::string &id);
    bool contains(const std::string &id) const;
    std::vector<Participant*> getByRole(const std::string role) const;
    size_t size() const;
    std::vector<std::string> getAllIds() const;
};

#endif