#ifndef PARTICIPANTREGISTRY_H
#define PARTICIPANTREGISTRY_H

#include "participant/Participant.h"

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
    void remove(const std::string &id);
    bool contains(const std::string &id) const;
    std::vector<Participant *> getByRole(const std::string role) const;
    size_t size() const;
    std::vector<std::string> getAllIds() const;
};

#endif