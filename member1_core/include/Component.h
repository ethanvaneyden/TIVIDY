#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <vector>
#include "Colleague.h"

class Iterator;

class Component : public Colleague
{
protected:
    std::string id;
    std::string type;
    std::string name;

    /// @brief Non owning reference
    Component *parent;

public:
    Component(std::string id, std::string type, std::string name, Component *parent = nullptr);
    virtual ~Component() = default;

    
    /// @brief Default no op add
    /// @param c 
    virtual void add(Component *c);

    /// @brief Default no op remove
    /// @param id 
    virtual void remove(const std::string &id);

    /// @brief Default no op get children
    /// @return vector of components
    virtual std::vector<Component *> getChildren() const;

    virtual Iterator *createIterator() = 0;
    virtual bool isCompleted() const = 0;

    const std::string &getId() const;
    const std::string &getType() const;
    const std::string &getName() const;

    virtual std::string getStateName() const { return ""; }

    Component *getParent() const;
    void setParent(Component *p);
};

#endif