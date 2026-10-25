#ifndef PARTICIPANT_H
#define PARTICIPANT_H
#include <string>

class Participant
{
private:
    std::string id;
    std::string name;
    std::string role;
    int pendingTasks;

public:
    Participant(std::string id, std::string name, std::string role);

    const std::string &getId() const;
    const std::string &getName() const;
    const std::string &getRole() const;

    void setName(const std::string &name);
    int getPendingTasks();
    void incrementPendingTasks();
    void decrementPendingTasks();
};

#endif