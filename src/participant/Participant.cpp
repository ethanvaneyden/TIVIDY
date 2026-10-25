#include "participant/Participant.h"

Participant::Participant(std::string id, std::string name, std::string role) : id(id), name(name), role(role), pendingTasks(0)
{
}

const std::string &Participant::getId() const
{
    return id;
}

const std::string &Participant::getName() const
{
    return name;
}

const std::string &Participant::getRole() const
{
    return role;
}

void Participant::setName(const std::string &name)
{
    this->name = name;
}

int Participant::getPendingTasks()
{
    return pendingTasks;
}

void Participant::incrementPendingTasks()
{
    pendingTasks++;
}

void Participant::decrementPendingTasks()
{
    if(pendingTasks < 1) {
        return;
    }
    pendingTasks--;
}
