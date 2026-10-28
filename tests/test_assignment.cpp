#include <gtest/gtest.h>

#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "Participant.h"
#include "RoundRobinStrategy.h"
#include "WorkloadStrategy.h"
#include "RandomStrategy.h"
#include "AssignmentEngine.h"
#include "Item.h"

class StrategyTest : public ::testing::Test
{
protected:
    Participant a{"P1", "Alice", "TA"};
    Participant b{"P2", "Bob", "TA"};
    Participant c{"P3", "Cara", "TA"};
    std::vector<Participant *> pool{&a, &b, &c};
};


TEST_F(StrategyTest, RoundRobinCyclesThroughPool)
{
    RoundRobinStrategy rr;
    EXPECT_EQ(rr.assign(pool), &a);
    EXPECT_EQ(rr.assign(pool), &b);
    EXPECT_EQ(rr.assign(pool), &c);
    EXPECT_EQ(rr.assign(pool), &a); 
}

TEST_F(StrategyTest, RoundRobinEmptyPoolReturnsNull)
{
    RoundRobinStrategy rr;
    EXPECT_EQ(rr.assign({}), nullptr);
}

TEST_F(StrategyTest, RoundRobinSurvivesPoolChange)
{
    RoundRobinStrategy rr;
    rr.assign(pool); 
    rr.assign(pool); 
    std::vector<Participant *> smaller{&a, &b};
    EXPECT_EQ(rr.assign(smaller), &a); 
}

// ---------- Workload ----------
TEST_F(StrategyTest, WorkloadPicksFewestPending)
{
    a.incrementPendingTasks();
    a.incrementPendingTasks();
    b.incrementPendingTasks();
    WorkloadStrategy w;
    EXPECT_EQ(w.assign(pool), &c); // c has 0
}

TEST_F(StrategyTest, WorkloadTieGoesToFirst)
{
    WorkloadStrategy w;
    EXPECT_EQ(w.assign(pool), &a); // all at 0
}

TEST_F(StrategyTest, WorkloadEmptyPoolReturnsNull)
{
    WorkloadStrategy w;
    EXPECT_EQ(w.assign({}), nullptr);
}

// ---------- Random ----------
TEST_F(StrategyTest, RandomAlwaysReturnsPoolMember)
{
    RandomStrategy r;
    for (int i = 0; i < 50; ++i)
    {
        Participant *p = r.assign(pool);
        EXPECT_TRUE(p == &a || p == &b || p == &c);
    }
}

TEST_F(StrategyTest, RandomEmptyPoolReturnsNull)
{
    RandomStrategy r;
    EXPECT_EQ(r.assign({}), nullptr);
}

// ---------- Engine ----------
TEST_F(StrategyTest, EngineStripsNullsFromPool)
{
    AssignmentEngine engine(std::make_unique<WorkloadStrategy>());
    Item item("I1", "Submission", "Test item");
    std::vector<Participant *> dirty{nullptr, &a, nullptr};
    EXPECT_EQ(engine.assign(item, dirty), &a);
}

TEST_F(StrategyTest, EngineReturnsNullForEmptyOrAllNullPool)
{
    AssignmentEngine engine(std::make_unique<RoundRobinStrategy>());
    Item item("I1", "Submission", "Test item");
    EXPECT_EQ(engine.assign(item, {}), nullptr);
    EXPECT_EQ(engine.assign(item, {nullptr, nullptr}), nullptr);
}

TEST_F(StrategyTest, EngineSetStrategySwapsBehaviour)
{
    AssignmentEngine engine(std::make_unique<RoundRobinStrategy>());
    Item item("I1", "Submission", "Test item");
    EXPECT_EQ(engine.strategyName(), "RoundRobin");

    a.incrementPendingTasks(); // a is now busy
    engine.setStrategy(std::make_unique<WorkloadStrategy>());
    EXPECT_EQ(engine.strategyName(), "Workload");
    EXPECT_EQ(engine.assign(item, pool), &b); // fewest pending, first on tie
}

TEST_F(StrategyTest, EngineDoesNotChangeTheItem)
{
    AssignmentEngine engine(std::make_unique<RoundRobinStrategy>());
    Item item("I1", "Submission", "Test item");
    engine.assign(item, pool);
    EXPECT_EQ(item.getAssignee(), nullptr); // the command does item.assign()
}