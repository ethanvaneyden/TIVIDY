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
};

TEST(IteratorTest, DepthFirst)
{
    Container root("r", "Root", "Root");
    Container c1("c1", "Container", "C1", &root);
    Container c2("c2", "Container", "C2", &root);
    Item a("a", "Item", "A", &c1);
    Item b("b", "Item", "B", &c1);
    Item c("c", "Item", "C", &c2);

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