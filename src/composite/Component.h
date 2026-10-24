#include <string>
#include <vector>

#include "mediator/Colleague.h"

class Iterator;
class Command;

class Component : public Colleague
{
private:
    std::string id;
    std::string type;
    std::string name;
    Component *parent;

public:
    Component(std::string id, std::string type, std::string name, Component *parent);
    ~Component() = default;

    virtual void add(Component *c);
    virtual void remove(const std::string &id);
    virtual std::vector<Component *> getChildren();
    virtual Iterator *createIterator() = 0;

    const std::string &getId() const;
    const std::string &getType() const;
    const std::string &getName() const;

    Component *getParent() const;
    void setParent(Component *p);
};