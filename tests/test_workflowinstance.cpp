#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "Container.h"
#include "AssignmentStrategy.h"
#include "HospitalAdmissionBuilder.h"
#include "HospitalAdmissionCoordinator.h"
#include "HospitalAdmissionDirector.h"
#include "HospitalPolicies.h"
#include "HospitalSeed.h"
#include "InvalidTransition.h"
#include "Item.h"
#include "ParticipantRegistry.h"
#include "Root.h"
#include "WorkflowInstance.h"
#include "WorkRegistry.h"
#include "WorkloadStrategy.h"

namespace
{
class LastParticipantStrategy : public AssignmentStrategy
{
public:
    Participant* assign(const std::vector<Participant*>& pool) override
    {
        return pool.empty() ? nullptr : pool.back();
    }

    std::string name() const override { return "LastParticipant"; }
};

class TrackedStrategy : public WorkloadStrategy
{
public:
    explicit TrackedStrategy(bool& destroyed) : destroyed(destroyed) {}
    ~TrackedStrategy() override { destroyed = true; }

private:
    bool& destroyed;
};

class TrackedBuilder : public hospital::HospitalAdmissionBuilder
{
public:
    TrackedBuilder(WorkRegistry& registry, bool& destroyed)
        : HospitalAdmissionBuilder(registry), destroyed(destroyed)
    {
    }

    ~TrackedBuilder() override { destroyed = true; }

private:
    bool& destroyed;
};

class TrackedCoordinator : public hospital::HospitalAdmissionCoordinator
{
public:
    TrackedCoordinator(ParticipantRegistry& participants,
                       std::unique_ptr<AssignmentStrategy> strategy,
                       bool& destroyed)
        : HospitalAdmissionCoordinator(participants, std::move(strategy)),
          destroyed(destroyed)
    {
    }

    ~TrackedCoordinator() override { destroyed = true; }

private:
    bool& destroyed;
};

std::unique_ptr<WorkflowInstance> makeHospitalInstance(
    const std::string& id,
    ParticipantRegistry& participants)
{
    std::unique_ptr<WorkRegistry> registry(new WorkRegistry());
    std::unique_ptr<hospital::HospitalAdmissionBuilder> builder(
        new hospital::HospitalAdmissionBuilder(*registry));
    hospital::HospitalAdmissionDirector(*builder).buildEmptyWard();
    Root* root = builder->getRoot();

    return std::unique_ptr<WorkflowInstance>(new WorkflowInstance(
        id,
        "healthcare",
        std::move(registry),
        root,
        std::unique_ptr<WorkflowMediator>(
            new hospital::HospitalAdmissionCoordinator(
                participants, std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy()))),
        std::move(builder),
        &participants));
}
}

TEST(WorkflowInstanceTest, RejectsUnregisteredDescendants)
{
    ParticipantRegistry participants;
    Item unregisteredItem("item", "treatment", "Patient");
    std::unique_ptr<WorkRegistry> registry(new WorkRegistry());
    Root* root = static_cast<Root*>(
        registry->add(std::unique_ptr<Component>(new Root("root", "hospital", "Hospital"))));
    Container* ward = static_cast<Container*>(
        registry->add(std::unique_ptr<Component>(new Container("ward", "ward", "Ward"))));
    root->add(ward);
    ward->add(&unregisteredItem);

    std::unique_ptr<WorkflowBuilder> builder(
        new hospital::HospitalAdmissionBuilder(*registry));
    std::unique_ptr<WorkflowMediator> coordinator(
        new hospital::HospitalAdmissionCoordinator(participants));

    EXPECT_THROW(
        WorkflowInstance("instance",
                         "healthcare",
                         std::move(registry),
                         root,
                         std::move(coordinator),
                         std::move(builder),
                         &participants),
        std::invalid_argument);
}

TEST(WorkflowInstanceTest, InstancesKeepTheirWorkTreesAndHistorySeparate)
{
    ParticipantRegistry participants;
    hospital::HospitalSeed::seedParticipants(participants);
    std::unique_ptr<WorkflowInstance> first = makeHospitalInstance("first", participants);
    std::unique_ptr<WorkflowInstance> second = makeHospitalInstance("second", participants);

    ASSERT_NE(first->getRoot(), second->getRoot());
    first->addItem("W1", "TRT-1", "Patient A", "P-1");

    ASSERT_NE(first->getItem("TRT-1"), nullptr);
    EXPECT_EQ(second->getItem("TRT-1"), nullptr);
    EXPECT_EQ(first->getContainer("W1")->getChildren().size(), 1u);
    EXPECT_TRUE(second->getContainer("W1")->getChildren().empty());
    ASSERT_NE(first->getRoot()->getMediator(), nullptr);
    EXPECT_EQ(first->getContainer("W1")->getMediator(), first->getRoot()->getMediator());
    EXPECT_EQ(first->getItem("TRT-1")->getMediator(), first->getRoot()->getMediator());
    EXPECT_NE(second->getRoot()->getMediator(), first->getRoot()->getMediator());
    ASSERT_EQ(first->getHistory().size(), 1u);
    EXPECT_NE(first->getHistory().front().find("TRT-1"), std::string::npos);
    EXPECT_TRUE(second->getHistory().empty());
}

TEST(WorkflowInstanceTest, OwnsItsCoordinatorStrategyAndBuilder)
{
    ParticipantRegistry participants;
    std::unique_ptr<WorkRegistry> registry(new WorkRegistry());
    bool coordinatorDestroyed = false;
    bool strategyDestroyed = false;
    bool builderDestroyed = false;
    std::unique_ptr<TrackedBuilder> builder(
        new TrackedBuilder(*registry, builderDestroyed));
    builder->buildRoot("Hospital");
    builder->buildContainer("W1", "Ward");
    Root* root = builder->getRoot();

    {
        WorkflowInstance instance(
            "tracked",
            "healthcare",
            std::move(registry),
            root,
            std::unique_ptr<WorkflowMediator>(new TrackedCoordinator(
                participants,
                std::unique_ptr<AssignmentStrategy>(new TrackedStrategy(strategyDestroyed)),
                coordinatorDestroyed)),
            std::move(builder),
            &participants);

        EXPECT_FALSE(coordinatorDestroyed);
        EXPECT_FALSE(strategyDestroyed);
        EXPECT_FALSE(builderDestroyed);
    }

    EXPECT_TRUE(coordinatorDestroyed);
    EXPECT_TRUE(strategyDestroyed);
    EXPECT_TRUE(builderDestroyed);
}

TEST(WorkflowInstanceTest, ReplacesCoordinatorStrategyAtRuntime)
{
    ParticipantRegistry participants;
    hospital::HospitalSeed::seedParticipants(participants);
    const std::vector<Participant*> doctors =
        participants.getByRole(hospital::HospitalRoles::doctor());
    ASSERT_GE(doctors.size(), 2u);

    std::unique_ptr<WorkRegistry> registry(new WorkRegistry());
    std::unique_ptr<hospital::HospitalAdmissionBuilder> builder(
        new hospital::HospitalAdmissionBuilder(*registry));
    hospital::HospitalAdmissionDirector(*builder).buildEmptyWard();
    Root* root = builder->getRoot();

    std::unique_ptr<hospital::HospitalAdmissionCoordinator> coordinator(
        new hospital::HospitalAdmissionCoordinator(participants));
    hospital::HospitalAdmissionCoordinator* coordinatorView = coordinator.get();
    WorkflowInstance instance(
        "runtime-strategy",
        "healthcare",
        std::move(registry),
        root,
        std::move(coordinator),
        std::move(builder),
        &participants);

    instance.addItem("W1", "TRT-A", "Patient A", "P-A");
    instance.addItem("W1", "TRT-B", "Patient B", "P-B");
    EXPECT_THROW(instance.setAssignmentStrategy(nullptr), std::invalid_argument);
    EXPECT_EQ(instance.getAssignmentStrategyName(), "Workload");
    instance.setAssignmentStrategy(std::unique_ptr<AssignmentStrategy>(
        new LastParticipantStrategy()));
    EXPECT_EQ(instance.getAssignmentStrategyName(), "LastParticipant");

    EXPECT_EQ(coordinatorView->openWard(*instance.getContainer("W1")), 2);
    EXPECT_EQ(instance.getItem("TRT-A")->getAssignee(), doctors.back());
    EXPECT_EQ(instance.getItem("TRT-B")->getAssignee(), doctors.back());
}