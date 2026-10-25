#ifndef ROOT_H
#define ROOT_H
#include "composite/Component.h"
#include <vector>
#include <memory>


class Root : public Component {
private:
    std::vector<Component*> children;

public:
    Root(std::string id, std::string type, std::string name, Component* parent = nullptr);
    virtual ~Root() = default;

    void add(Component* c) override;
    void remove(const std::string& id) override;
    std::vector<Component*> getChildren() const override;
    Iterator* createIterator() override;
};

#endif