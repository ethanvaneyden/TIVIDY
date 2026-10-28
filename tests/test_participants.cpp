#include <gtest/gtest.h>

#include <algorithm>
#include <stdexcept>
#include <set>
#include <string>
#include <vector>

#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Container.h"
#include "Item.h"

TEST(ParticipantTest, StoresIdentityAndAllowsNameUpdates)
{
    Participant participant("marker1", "Alex", "Marker");

    EXPECT_EQ(participant.getId(), "marker1");
    EXPECT_EQ(participant.getName(), "Alex");
    EXPECT_EQ(participant.getRole(), "Marker");

    participant.setName("Alex Smith");
    EXPECT_EQ(participant.getName(), "Alex Smith");
}

TEST(ParticipantTest, TracksPendingTasksWithoutGoingBelowZero)
{
    Participant participant("marker1", "Alex", "Marker");

    EXPECT_EQ(participant.getPendingTasks(), 0);

    participant.decrementPendingTasks();
    EXPECT_EQ(participant.getPendingTasks(), 0);

    participant.incrementPendingTasks();
    participant.incrementPendingTasks();
    EXPECT_EQ(participant.getPendingTasks(), 2);

    participant.decrementPendingTasks();
    EXPECT_EQ(participant.getPendingTasks(), 1);
}

TEST(ParticipantRegistryTest, AssignsIdsIncrementingPerRole)
{
    ParticipantRegistry registry;

    Participant *firstMarker = registry.add("Alex", "Marker");
    Participant *firstReviewer = registry.add("Blair", "Reviewer");
    Participant *secondMarker = registry.add("Casey", "Marker");

    ASSERT_NE(firstMarker, nullptr);
    ASSERT_NE(firstReviewer, nullptr);
    ASSERT_NE(secondMarker, nullptr);
    EXPECT_EQ(firstMarker->getId(), "Marker1");
    EXPECT_EQ(firstReviewer->getId(), "Reviewer1");
    EXPECT_EQ(secondMarker->getId(), "Marker2");
}

TEST(ParticipantRegistryTest, FindsParticipantsAndReportsMembership)
{
    ParticipantRegistry registry;
    Participant *participant = registry.add("Alex", "Marker");

    EXPECT_EQ(registry.get("Marker1"), participant);
    EXPECT_TRUE(registry.contains("Marker1"));
    EXPECT_EQ(registry.get("missing"), nullptr);
    EXPECT_FALSE(registry.contains("missing"));
}

TEST(ParticipantRegistryTest, FiltersParticipantsByRole)
{
    ParticipantRegistry registry;
    Participant *marker = registry.add("Alex", "Marker");
    registry.add("Blair", "Reviewer");
    Participant *secondMarker = registry.add("Casey", "Marker");

    const std::vector<Participant *> markers = registry.getByRole("Marker");
    ASSERT_EQ(markers.size(), 2u);
    EXPECT_NE(std::find(markers.begin(), markers.end(), marker), markers.end());
    EXPECT_NE(std::find(markers.begin(), markers.end(), secondMarker), markers.end());
    EXPECT_TRUE(registry.getByRole("Unknown").empty());
}

TEST(ParticipantRegistryTest, RemovesParticipantsAndIgnoresUnknownIds)
{
    ParticipantRegistry registry;
    registry.add("Alex", "Marker");

    registry.remove("Marker1");
    EXPECT_EQ(registry.size(), 0u);
    EXPECT_FALSE(registry.contains("Marker1"));

    registry.remove("missing");
    EXPECT_EQ(registry.size(), 0u);
}

TEST(ParticipantRegistryTest, ReturnsAllIds)
{
    ParticipantRegistry registry;
    registry.add("Alex", "Marker");
    registry.add("Blair", "Reviewer");

    const std::vector<std::string> ids = registry.getAllIds();
    EXPECT_EQ(std::set<std::string>(ids.begin(), ids.end()),
              (std::set<std::string>{"Marker1", "Reviewer1"}));
}

TEST(ParticipantRegistryTest, RejectsEmptyNameOrRole)
{
    ParticipantRegistry registry;

    EXPECT_THROW(registry.add("", "Marker"), std::invalid_argument);
    EXPECT_THROW(registry.add("Alex", ""), std::invalid_argument);
    EXPECT_EQ(registry.size(), 0u);

    Participant *participant = registry.add("Alex", "Marker");
    ASSERT_NE(participant, nullptr);
    EXPECT_EQ(participant->getId(), "Marker1");
}

TEST(ParticipantRegistryTest, AvoidsCollisionsBetweenRoleGeneratedIds)
{
    ParticipantRegistry registry;
    for (int i = 0; i < 11; ++i)
        registry.add("Person", "A");

    Participant *participant = registry.add("Other", "A1");

    ASSERT_NE(participant, nullptr);
    EXPECT_EQ(participant->getId(), "A12");
    EXPECT_EQ(registry.get("A11")->getRole(), "A");
    EXPECT_EQ(registry.get("A12"), participant);
}

TEST(ParticipantRegistryTest, PreventsRemovalWhileItemReferencesParticipant)
{
    ParticipantRegistry registry;
    Participant *participant = registry.add("Alex", "Marker");

    {
        Container container("container", "group", "Container");
        Item item("item", "work", "Item");
        container.add(&item);
        container.advancePhase();
        item.assign(participant);
        item.start();
        item.complete(10);
        container.advancePhase();
        item.recordReview(participant, "Personal");

        EXPECT_THROW(registry.remove(participant->getId()), std::logic_error);
        EXPECT_TRUE(registry.contains(participant->getId()));
        EXPECT_EQ(item.getAssignee(), participant);
        EXPECT_EQ(item.getReviewer(), participant);
    }

    registry.remove("Marker1");
    EXPECT_FALSE(registry.contains("Marker1"));
}

TEST(ParticipantRegistryTest, ClearsItemReferencesWhenRegistryIsDestroyed)
{
    Container container("container", "group", "Container");
    Item item("item", "work", "Item");

    container.add(&item);
    container.advancePhase();

    {
        ParticipantRegistry registry;
        Participant *participant = registry.add("Alex", "Marker");
        item.assign(participant);
        item.start();
        item.complete(10);
        container.advancePhase();
        item.recordReview(participant, "Personal");

        ASSERT_EQ(item.getAssignee(), participant);
        ASSERT_EQ(item.getReviewer(), participant);
    }

    EXPECT_EQ(item.getAssignee(), nullptr);
    EXPECT_EQ(item.getReviewer(), nullptr);
    EXPECT_TRUE(item.getReviewMethod().empty());
    EXPECT_FALSE(item.isReviewed());
}