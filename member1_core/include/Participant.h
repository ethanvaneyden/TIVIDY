#ifndef PARTICIPANT_H
#define PARTICIPANT_H
#include <cstddef>
#include <map>
#include <string>

class Item;
class ParticipantRegistry;

class Participant
{
private:
    std::string id;
    std::string name;
    std::string role;
    int pendingTasks;
    std::map<Item*, std::size_t> itemReferences;

    void addItemReference(Item* item);
    void removeItemReference(Item* item);
    bool hasItemReferences() const;

    friend class Item;
    friend class ParticipantRegistry;

public:
    Participant(std::string id, std::string name, std::string role);

    /**
     * @brief Detaches any items that still refer to this participant.
     */
    ~Participant();

    const std::string &getId() const;
    const std::string &getName() const;
    const std::string &getRole() const;

    void setName(const std::string &name);
    int getPendingTasks();
    void incrementPendingTasks();
    void decrementPendingTasks();
};

#endif