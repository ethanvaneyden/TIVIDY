#include "Participant.h"
#include "Item.h"

Participant::Participant(std::string id, std::string name, std::string role)
    : id(id), name(name), role(role), pendingTasks(0)
{
}

Participant::~Participant()
{
    for (const auto& reference : itemReferences)
        reference.first->participantDestroyed(this);
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

void Participant::addItemReference(Item* item)
{
    ++itemReferences[item];
}

void Participant::removeItemReference(Item* item)
{
    auto reference = itemReferences.find(item);
    if (reference == itemReferences.end())
        return;
    if (--reference->second == 0)
        itemReferences.erase(reference);
}

bool Participant::hasItemReferences() const
{
    return !itemReferences.empty();
}
