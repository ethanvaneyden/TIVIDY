#include "Component.h"
#include <memory>

class ItemState;
class Participant;

class Item : public Component
{
private:
    std::unique_ptr<ItemState> state;
    Participant *assignee;

public:
    Item();
    ~Item() = default;

    void add(Component *c) override;
    void remove(const std::string &id) override;
    std::vector<Component *> getChildren();
    Iterator *createIterator();

    std::string getStateName();
    void setState(std::unique_ptr<ItemState> state);
};