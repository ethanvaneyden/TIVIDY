#include <gtest/gtest.h>

#include <string>
#include <vector>
#include "Component.h"
#include "DepthFirstIterator.h"
#include "StateFilterIterator.h"

// Fake node
class TestNode : public Component
{
private:
    std::vector<Component *> kids;
    std::string stateName;

public:
    TestNode(std::string name, std::string state = "")
        : Component(name, "test", name), stateName(state) {}

    void add(Component *c) override
    {
        kids.push_back(c);
        c->setParent(this);
    }
    std::vector<Component *> getChildren() const override { return kids; }
    std::string getStateName() const override { return stateName; }
    Iterator *createIterator() override { return nullptr; }
    bool isCompleted() const override { return false; }
};

TEST(Iterator, DepthFirst)
{
    TestNode root("Root"), c1("C1"), c2("C2"), a("A"), b("B"), c("C");
    root.add(&c1);
    root.add(&c2);
    c1.add(&a);
    c1.add(&b);
    c2.add(&c);

    DepthFirstIterator it(&root);
    std::vector<std::string> order;
    for (it.first(); !it.isDone(); it.next())
        order.push_back(it.current()->getName());

    EXPECT_EQ(order, (std::vector<std::string>{"Root", "C1", "A", "B", "C2", "C"}));
}