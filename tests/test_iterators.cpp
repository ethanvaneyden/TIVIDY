#include <gtest/gtest.h>

#include <string>
#include <vector>
#include "Container.h"
#include "Item.h"
#include "DepthFirstIterator.h"

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