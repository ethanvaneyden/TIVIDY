#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "Container.h"
#include "HospitalAdmissionBuilder.h"
#include "HospitalAdmissionCoordinator.h"
#include "HospitalAdmissionDirector.h"
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
            new hospital::HospitalAdmissionCoordinator(participants)),
        std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy()),
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
                         std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy()),
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
    ASSERT_EQ(first->getHistory().size(), 1u);
    EXPECT_NE(first->getHistory().front().find("TRT-1"), std::string::npos);
    EXPECT_TRUE(second->getHistory().empty());
}