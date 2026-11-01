#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "EventBus.h"
#include "Observer.h"
#include "WorkflowEvent.h"

namespace
{
class RecordingObserver : public Observer
{
public:
    RecordingObserver(std::vector<WorkflowEvent>& received, int& destructions)
        : received(received), destructions(destructions)
    {
    }

    ~RecordingObserver() override
    {
        ++destructions;
    }

    void update(const WorkflowEvent& event) override
    {
        received.push_back(event);
    }

private:
    std::vector<WorkflowEvent>& received;
    int& destructions;
};
}

TEST(EventBusTest, PublishesEachEventToEverySubscribedObserver)
{
    EventBus bus;
    std::vector<WorkflowEvent> firstEvents;
    std::vector<WorkflowEvent> secondEvents;
    int destructions = 0;
    bus.subscribe(std::unique_ptr<Observer>(
        new RecordingObserver(firstEvents, destructions)));
    bus.subscribe(std::unique_ptr<Observer>(
        new RecordingObserver(secondEvents, destructions)));

    WorkflowEvent event;
    event.type = "ItemCompleted";
    event.recipientId = "item-1";
    event.payload = "85";
    bus.publish(event);

    ASSERT_EQ(firstEvents.size(), 1u);
    EXPECT_EQ(firstEvents.front().type, "ItemCompleted");
    EXPECT_EQ(firstEvents.front().recipientId, "item-1");
    EXPECT_EQ(firstEvents.front().payload, "85");
    ASSERT_EQ(secondEvents.size(), 1u);
    EXPECT_EQ(secondEvents.front().type, "ItemCompleted");
    EXPECT_EQ(secondEvents.front().recipientId, "item-1");
    EXPECT_EQ(secondEvents.front().payload, "85");
    EXPECT_EQ(bus.observerCount(), 2u);
    EXPECT_EQ(destructions, 0);
}

TEST(EventBusTest, OwnsObserversAndDestroysThemWithTheBus)
{
    std::vector<WorkflowEvent> received;
    int destructions = 0;
    {
        EventBus bus;
        bus.subscribe(std::unique_ptr<Observer>(
            new RecordingObserver(received, destructions)));
        EXPECT_EQ(bus.observerCount(), 1u);
        EXPECT_EQ(destructions, 0);
    }

    EXPECT_EQ(destructions, 1);
}

TEST(EventBusTest, IgnoresNullObserverSubscriptions)
{
    EventBus bus;
    bus.subscribe(std::unique_ptr<Observer>());

    EXPECT_EQ(bus.observerCount(), 0u);
    WorkflowEvent event;
    EXPECT_NO_THROW(bus.publish(event));
}