/**
 * @file Participant.cpp
 * @brief Implementation of the Participant class.
 * @author Member 1
 */

#include "Participant.h"
#include <utility>

Participant::Participant(std::string id, std::string name, std::string role)
    : id(std::move(id)), 
      name(std::move(name)), 
      role(std::move(role)), 
      pendingTasks(0) {}

const std::string& Participant::getId() const { 
    return id; 
}

const std::string& Participant::getName() const { 
    return name; 
}

const std::string& Participant::getRole() const { 
    return role; 
}

int Participant::getPendingTasks() const { 
    return pendingTasks; 
}

void Participant::incrementPendingTasks() { 
    ++pendingTasks; 
}

void Participant::decrementPendingTasks() { 
    if (pendingTasks > 0) {
        --pendingTasks; 
    }
}