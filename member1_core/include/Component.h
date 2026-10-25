#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <vector>
#include "Colleague.h"

class Iterator;

class Component : public Colleague {
protected:
    std::string id;
    std::string type;
    std::string name;
    Component* parent;

public:
    Component(std::string id, std::string type, std::string name, Component* parent = nullptr);
    virtual ~Component() = default;

    virtual void add(Component* c);
    virtual void remove(const std::string& id);
    virtual std::vector<Component*> getChildren() const;

    virtual Iterator* createIterator() = 0;
    virtual bool isCompleted() const = 0;

    const std::string& getId() const;
    const std::string& getType() const;
    const std::string& getName() const;

    Component* getParent() const;
    void setParent(Component* p);
};

#endif