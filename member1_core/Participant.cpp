/**
 * @file Participant.cpp
 * @brief Implementation of the Participant class.
 * @author Member 1
 */

#include "Participant.h"
#include <limits>
#include <stdexcept>
#include <utility>

Participant::Participant(std::string id, std::string name, std::string role)
    : id(std::move(id)), 
      name(std::move(name)), 
      role(std::move(role)), 
      pendingTasks(0),
      itemUses(0) {}

const std::string& Participant::getId() const { 
    return id; 
}

const std::string& Participant::getName() const { 
    return name; 
}

const std::string& Participant::getRole() const { 
    return role; 
}

void Participant::setName(const std::string& newName) {
    name = newName;
}

int Participant::getPendingTasks() const { 
    return pendingTasks; 
}

void Participant::incrementPendingTasks() { 
    if (pendingTasks == std::numeric_limits<int>::max())
        throw std::overflow_error("Participant pending-task count overflow");
    ++pendingTasks; 
}

void Participant::decrementPendingTasks() { 
    if (pendingTasks > 0) {
        --pendingTasks; 
    }
}

bool Participant::hasItemUses() const
{
    return itemUses != 0;
}

void Participant::retainItemUse()
{
    if (itemUses == std::numeric_limits<std::size_t>::max())
        throw std::overflow_error("Participant item-use count overflow");
    ++itemUses;
}

void Participant::releaseItemUse()
{
    if (itemUses > 0)
    {
        --itemUses;
    }
}