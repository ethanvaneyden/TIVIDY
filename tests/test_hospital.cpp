/**
 * @file test_hospital.cpp
 * @brief Unit tests for Member 6's Hospital scenario.
 */

#include <gtest/gtest.h>

#include "AssignmentStrategy.h"
#include "Container.h"
#include "HospitalAdmissionBuilder.h"
#include "HospitalAdmissionCoordinator.h"
#include "HospitalAdmissionDirector.h"
#include "HospitalApprovalChain.h"
#include "HospitalPolicies.h"
#include "HospitalSeed.h"
#include "InvalidTransition.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Root.h"
#include "Treatment.h"
#include "WorkRegistry.h"
#include "WorkflowInstance.h"
#include "WorkloadStrategy.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

using namespace hospital;

namespace
{

bool historyContains(const std::vector<std::string>& history, const std::string& text)
{
    for (std::size_t i = 0; i < history.size(); ++i)
    {
        if (history[i].find(text) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

class HospitalFixture : public ::testing::Test
{
    protected:
        ParticipantRegistry people;
        WorkRegistry registry;
        HospitalAdmissionBuilder builder;
        std::unique_ptr<HospitalAdmissionCoordinator> coordinator;
        Root* root;
        Container* ward;

        HospitalFixture() : builder(registry), root(nullptr), ward(nullptr) {}

        void SetUp()
        {
            HospitalSeed::seedParticipants(people);
            coordinator.reset(new HospitalAdmissionCoordinator(people));
        }

        void buildWard(int treatments)
        {
            builder.buildRoot("City Hospital");
            builder.buildContainer("W1", "General Ward");
            for (int i = 1; i <= treatments; ++i)
            {
                const std::string n = std::to_string(i);
                builder.buildTreatment("T" + n, "Patient " + n, "P-" + n, "Diagnosis " + n);
            }
            root = builder.getRoot();
            ward = builder.getCurrentContainer();
            root->setMediator(coordinator.get());
        }

        Treatment* treatment(int i)
        {
            return dynamic_cast<Treatment*>(registry.get("T" + std::to_string(i)));
        }

        void treatAll(const std::vector<int>& severities)
        {
            for (std::size_t i = 0; i < severities.size(); ++i)
            {
                Treatment* t = treatment(static_cast<int>(i) + 1);
                t->start();
                t->complete(severities[i]);
            }
        }

        void reachReview(const std::vector<int>& severities)
        {
            buildWard(static_cast<int>(severities.size()));
            coordinator->openWard(*ward);
            treatAll(severities);
        }

        Participant* firstWithRole(const std::string& role)
        {
            return people.getByRole(role).front();
        }
};

}

// ---------------- Treatment ----------------

TEST(HospitalTreatment, StartsAvailableWithoutSeverity)
{
    Treatment t("TRT-1", "Karabo", "P-1", "Flu");
    EXPECT_EQ("Available", t.getStateName());
    EXPECT_FALSE(t.hasSeverity());
    EXPECT_EQ(Treatment::NO_SEVERITY, t.getSeverity());
    EXPECT_EQ("treatment", t.getType());
}

TEST(HospitalTreatment, HoldsPatientData)
{
    Treatment t("TRT-1", "Karabo", "P-1", "Flu");
    EXPECT_EQ("Karabo", t.getPatientName());
    EXPECT_EQ("P-1", t.getPatientId());
    EXPECT_EQ("Flu", t.getDiagnosis());
    t.setDiagnosis("Pneumonia");
    t.setSeverity(6);
    EXPECT_EQ("Pneumonia", t.getDiagnosis());
    EXPECT_TRUE(t.hasSeverity());
    EXPECT_EQ(6, t.getSeverity());
}

// ---------------- Builder and director ----------------

TEST(HospitalBuilder, BuildsHospitalWardAndTreatments)
{
    WorkRegistry registry;
    HospitalAdmissionBuilder builder(registry);
    builder.buildRoot("City Hospital");
    builder.buildContainer("W1", "General Ward");
    builder.buildTreatment("T1", "Anele", "P-1", "Asthma");

    ASSERT_NE(nullptr, builder.getRoot());
    EXPECT_EQ("hospital", builder.getRoot()->getType());
    ASSERT_NE(nullptr, builder.getCurrentContainer());
    EXPECT_EQ("ward", builder.getCurrentContainer()->getType());
    EXPECT_EQ(1u, builder.getCurrentContainer()->getChildren().size());
    EXPECT_EQ(3u, registry.size());
}

TEST(HospitalBuilder, BuildItemUsesItemIdAsPatientId)
{
    WorkRegistry registry;
    HospitalAdmissionBuilder builder(registry);
    builder.buildRoot("City Hospital");
    builder.buildContainer("W1", "General Ward");
    builder.buildItem("P-77", "Thabo");

    Treatment* t = dynamic_cast<Treatment*>(registry.get("P-77"));
    ASSERT_NE(nullptr, t);
    EXPECT_EQ("P-77", t->getPatientId());
    EXPECT_EQ("Thabo", t->getPatientName());
}

TEST(HospitalBuilder, RefusesOutOfOrderAndDuplicateBuilds)
{
    HospitalAdmissionBuilder unset;
    EXPECT_THROW(unset.buildRoot("H"), std::logic_error);

    WorkRegistry registry;
    HospitalAdmissionBuilder builder(registry);
    EXPECT_THROW(builder.buildContainer("W1", "Ward"), std::logic_error);
    builder.buildRoot("H");
    EXPECT_THROW(builder.buildItem("T1", "Nobody"), std::logic_error);
    builder.buildContainer("W1", "Ward");
    builder.buildItem("T1", "Patient");
    EXPECT_THROW(builder.buildItem("T1", "Patient"), std::invalid_argument);
    EXPECT_EQ(1u, builder.getCurrentContainer()->getChildren().size());
}

TEST(HospitalDirector, DemoWardHasThreeTreatmentsWithDiagnoses)
{
    WorkRegistry registry;
    HospitalAdmissionBuilder builder(registry);
    HospitalAdmissionDirector director(builder);
    director.buildDemoWard();

    Container* ward = builder.getCurrentContainer();
    ASSERT_NE(nullptr, ward);
    EXPECT_EQ(HospitalAdmissionDirector::wardId(), ward->getId());
    ASSERT_EQ(3u, ward->getChildren().size());
    Treatment* first = dynamic_cast<Treatment*>(ward->getChildren().front());
    ASSERT_NE(nullptr, first);
    EXPECT_FALSE(first->getDiagnosis().empty());
}

TEST(HospitalDirector, EmptyWardHasNoTreatments)
{
    WorkRegistry registry;
    HospitalAdmissionBuilder builder(registry);
    HospitalAdmissionDirector director(builder);
    director.buildEmptyWard();
    EXPECT_TRUE(builder.getCurrentContainer()->getChildren().empty());
    EXPECT_EQ("Intake", builder.getCurrentContainer()->getPhaseName());
}

// ---------------- Policies ----------------

TEST(HospitalPolicies, ApprovalBandsIncludingBoundaries)
{
    EXPECT_EQ(HospitalRoles::triageNurse(), HospitalPolicies::approverRoleFor(1));
    EXPECT_EQ(HospitalRoles::triageNurse(), HospitalPolicies::approverRoleFor(3));
    EXPECT_EQ(HospitalRoles::admittingDoctor(), HospitalPolicies::approverRoleFor(4));
    EXPECT_EQ(HospitalRoles::admittingDoctor(), HospitalPolicies::approverRoleFor(6));
    EXPECT_EQ(HospitalRoles::chiefOfMedicine(), HospitalPolicies::approverRoleFor(7));
    EXPECT_EQ(HospitalRoles::chiefOfMedicine(), HospitalPolicies::approverRoleFor(10));
}

TEST(HospitalPolicies, InvalidSeverityHasNoApprover)
{
    EXPECT_FALSE(HospitalPolicies::isValidSeverity(0));
    EXPECT_FALSE(HospitalPolicies::isValidSeverity(11));
    EXPECT_FALSE(HospitalPolicies::isValidSeverity(-1));
    EXPECT_TRUE(HospitalPolicies::approverRoleFor(0).empty());
    EXPECT_TRUE(HospitalPolicies::approverRoleFor(11).empty());
}

TEST(HospitalPolicies, ApproverIsNeverTheAssignee)
{
    ParticipantRegistry people;
    Participant* only = people.add("Dr Solo", HospitalRoles::admittingDoctor());
    EXPECT_EQ(nullptr, HospitalPolicies::selectApprover(5, only, people));

    Participant* other = people.add("Dr Other", HospitalRoles::admittingDoctor());
    EXPECT_EQ(other, HospitalPolicies::selectApprover(5, only, people));
}

TEST(HospitalPolicies, ApproverWithFewestPendingTasksWins)
{
    ParticipantRegistry people;
    Participant* busy = people.add("Busy", HospitalRoles::triageNurse());
    Participant* idle = people.add("Idle", HospitalRoles::triageNurse());
    busy->incrementPendingTasks();
    EXPECT_EQ(idle, HospitalPolicies::selectApprover(2, nullptr, people));
}

// ---------------- Approval chain ----------------

TEST_F(HospitalFixture, ChainRoutesEachSeverityToItsBand)
{
    reachReview({2, 5, 9});
    ASSERT_EQ("Review", ward->getPhaseName());

    HospitalApprovalChain chain(people);
    EXPECT_TRUE(chain.approve(*treatment(1), 2));
    EXPECT_TRUE(chain.approve(*treatment(2), 5));
    EXPECT_TRUE(chain.approve(*treatment(3), 9));

    EXPECT_EQ(HospitalRoles::triageNurse(), treatment(1)->getReviewer()->getRole());
    EXPECT_EQ(HospitalRoles::admittingDoctor(), treatment(2)->getReviewer()->getRole());
    EXPECT_EQ(HospitalRoles::chiefOfMedicine(), treatment(3)->getReviewer()->getRole());
    EXPECT_EQ("Batch", treatment(1)->getReviewMethod());
}

TEST_F(HospitalFixture, ChainDoesNotApproveSeverityZero)
{
    reachReview({0});
    HospitalApprovalChain chain(people);
    EXPECT_FALSE(chain.approve(*treatment(1), 0));
    EXPECT_FALSE(treatment(1)->isReviewed());
}

TEST(HospitalApprovalChainStandalone, FailsWhenNoApproverExistsForBand)
{
    ParticipantRegistry people;
    people.add("Prof Botha", HospitalRoles::chiefOfMedicine());
    HospitalApprovalChain chain(people);
    Treatment t("T1", "Patient", "P-1");
    EXPECT_FALSE(chain.approve(t, 2));
}

// ---------------- Coordinator: opening the ward ----------------

TEST_F(HospitalFixture, OpeningWardAssignsEveryTreatmentToADoctor)
{
    buildWard(3);
    EXPECT_EQ(3, coordinator->openWard(*ward));
    EXPECT_EQ("Active", ward->getPhaseName());
    for (int i = 1; i <= 3; ++i)
    {
        ASSERT_NE(nullptr, treatment(i)->getAssignee());
        EXPECT_EQ(HospitalRoles::doctor(), treatment(i)->getAssignee()->getRole());
        EXPECT_EQ("Assigned", treatment(i)->getStateName());
    }
}

TEST_F(HospitalFixture, WorkloadStrategySpreadsTreatmentsAcrossDoctors)
{
    buildWard(3);
    coordinator->openWard(*ward);
    std::vector<std::string> ids;
    for (int i = 1; i <= 3; ++i)
    {
        ids.push_back(treatment(i)->getAssignee()->getId());
    }
    std::sort(ids.begin(), ids.end());
    EXPECT_EQ(3u, std::unique(ids.begin(), ids.end()) - ids.begin());
}

TEST_F(HospitalFixture, OpeningEmptyWardIsInvalid)
{
    builder.buildRoot("City Hospital");
    builder.buildContainer("W1", "General Ward");
    ward = builder.getCurrentContainer();
    EXPECT_THROW(coordinator->openWard(*ward), InvalidTransition);
    EXPECT_EQ("Intake", ward->getPhaseName());
}

TEST_F(HospitalFixture, OpeningWardTwiceIsInvalid)
{
    buildWard(1);
    coordinator->openWard(*ward);
    EXPECT_THROW(coordinator->openWard(*ward), InvalidTransition);
}

TEST(HospitalCoordinatorNoDoctors, TreatmentsStayAvailableAndFailureIsLogged)
{
    ParticipantRegistry people;
    people.add("Prof Botha", HospitalRoles::chiefOfMedicine());
    WorkRegistry registry;
    HospitalAdmissionBuilder builder(registry);
    HospitalAdmissionCoordinator coordinator(people);
    builder.buildRoot("H");
    builder.buildContainer("W1", "Ward");
    builder.buildTreatment("T1", "Patient", "P-1", "Flu");
    builder.getRoot()->setMediator(&coordinator);

    EXPECT_EQ(0, coordinator.openWard(*builder.getCurrentContainer()));
    EXPECT_EQ("Available", registry.get("T1") == nullptr ? "" :
              dynamic_cast<Item*>(registry.get("T1"))->getStateName());
    EXPECT_TRUE(historyContains(coordinator.getHistoryAsStrings(), "No doctor available for T1"));
}

// ---------------- Coordinator: treatment lifecycle ----------------

TEST_F(HospitalFixture, CompletingUnassignedTreatmentIsInvalid)
{
    buildWard(1);
    EXPECT_THROW(treatment(1)->complete(5), InvalidTransition);
}

TEST_F(HospitalFixture, StartingBeforeWardIsActiveIsInvalid)
{
    buildWard(1);
    EXPECT_THROW(treatment(1)->start(), std::logic_error);
}

TEST_F(HospitalFixture, CompletionRecordsSeverityAndLogsIt)
{
    buildWard(2);
    coordinator->openWard(*ward);
    treatment(1)->start();
    treatment(1)->complete(4);
    EXPECT_EQ(4, treatment(1)->getSeverity());
    EXPECT_EQ("Active", ward->getPhaseName());
    EXPECT_TRUE(historyContains(coordinator->getHistoryAsStrings(), "T1 completed with severity 4"));
}

TEST_F(HospitalFixture, WardMovesToReviewWhenEveryTreatmentIsDone)
{
    reachReview({2, 5});
    EXPECT_EQ("Review", ward->getPhaseName());
    EXPECT_TRUE(historyContains(coordinator->getHistoryAsStrings(), "moved to Review"));
}

TEST_F(HospitalFixture, InvalidSeverityIsFlaggedInHistory)
{
    reachReview({0});
    EXPECT_TRUE(historyContains(coordinator->getHistoryAsStrings(), "invalid severity"));
}

// ---------------- Coordinator: batch approval ----------------

TEST_F(HospitalFixture, BatchApprovalOutsideReviewIsInvalid)
{
    buildWard(1);
    EXPECT_THROW(coordinator->approveBatch(*ward), InvalidTransition);
    coordinator->openWard(*ward);
    EXPECT_THROW(coordinator->approveBatch(*ward), InvalidTransition);
}

TEST_F(HospitalFixture, BatchApprovesValidSeveritiesAndPublishesWard)
{
    reachReview({2, 5, 9});
    ward->getChildren();
    coordinator->onContainerApprovalRequested(ward, firstWithRole(HospitalRoles::chiefOfMedicine()));

    const HospitalAdmissionCoordinator::BatchResult& result = coordinator->getLastBatch();
    EXPECT_EQ(3u, result.approvedIds.size());
    EXPECT_TRUE(result.rejectedIds.empty());
    EXPECT_EQ("Publish", ward->getPhaseName());
}

TEST_F(HospitalFixture, BatchRejectsSeverityTheChainCannotApprove)
{
    reachReview({2, 0});
    coordinator->onContainerApprovalRequested(ward, firstWithRole(HospitalRoles::chiefOfMedicine()));

    const HospitalAdmissionCoordinator::BatchResult& result = coordinator->getLastBatch();
    ASSERT_EQ(1u, result.approvedIds.size());
    ASSERT_EQ(1u, result.rejectedIds.size());
    EXPECT_EQ("T2", result.rejectedIds[0]);
    EXPECT_EQ("Rejected", treatment(2)->getStateName());
    EXPECT_NE(std::string(), treatment(2)->getRejectionReason());
    EXPECT_EQ("Review", ward->getPhaseName());
    EXPECT_FALSE(coordinator->isReadyToPublish(*ward));
}

TEST_F(HospitalFixture, SecondBatchSkipsAlreadyReviewedAndRejected)
{
    reachReview({2, 0});
    coordinator->onContainerApprovalRequested(ward, nullptr);
    coordinator->onContainerApprovalRequested(ward, nullptr);
    const HospitalAdmissionCoordinator::BatchResult& result = coordinator->getLastBatch();
    EXPECT_TRUE(result.approvedIds.empty());
    EXPECT_EQ(2u, result.skippedIds.size());
}

TEST_F(HospitalFixture, ApprovalRequestOutsideReviewIsLoggedNotExecuted)
{
    buildWard(1);
    EXPECT_NO_THROW(coordinator->onContainerApprovalRequested(ward, nullptr));
    EXPECT_TRUE(historyContains(coordinator->getHistoryAsStrings(), "ignored"));
    EXPECT_EQ("Intake", ward->getPhaseName());
}

// ---------------- Coordinator: Chief of Medicine override ----------------

TEST_F(HospitalFixture, ChiefResolvesRejectedTreatmentAndWardPublishes)
{
    reachReview({2, 0});
    coordinator->onContainerApprovalRequested(ward, nullptr);

    Participant* chief = firstWithRole(HospitalRoles::chiefOfMedicine());
    EXPECT_TRUE(coordinator->resolveRejected(*treatment(2), false, 8, chief));

    EXPECT_EQ(8, treatment(2)->getSeverity());
    EXPECT_EQ(chief, treatment(2)->getReviewer());
    EXPECT_EQ("Personal", treatment(2)->getReviewMethod());
    EXPECT_EQ("Publish", ward->getPhaseName());
    EXPECT_TRUE(historyContains(coordinator->getHistoryAsStrings(), "settled by Prof Botha"));
}

TEST_F(HospitalFixture, OnlyChiefMayResolveARejectedTreatment)
{
    reachReview({0});
    coordinator->onContainerApprovalRequested(ward, nullptr);
    Participant* nurse = firstWithRole(HospitalRoles::triageNurse());
    EXPECT_FALSE(coordinator->resolveRejected(*treatment(1), false, 5, nurse));
    EXPECT_FALSE(coordinator->resolveRejected(*treatment(1), false, 5, nullptr));
    EXPECT_EQ("Rejected", treatment(1)->getStateName());
}

TEST_F(HospitalFixture, ChiefCannotSettleWithInvalidSeverity)
{
    reachReview({0});
    coordinator->onContainerApprovalRequested(ward, nullptr);
    Participant* chief = firstWithRole(HospitalRoles::chiefOfMedicine());
    EXPECT_FALSE(coordinator->resolveRejected(*treatment(1), false, 11, chief));
    EXPECT_FALSE(coordinator->resolveRejected(*treatment(1), true, 0, chief));
    EXPECT_EQ("Rejected", treatment(1)->getStateName());
}

TEST_F(HospitalFixture, NonRejectedTreatmentCannotBeResolved)
{
    reachReview({2});
    Participant* chief = firstWithRole(HospitalRoles::chiefOfMedicine());
    EXPECT_FALSE(coordinator->resolveRejected(*treatment(1), false, 5, chief));
}

// ---------------- Instance isolation and engine integration ----------------

TEST(HospitalIsolation, TwoWardsDoNotInterfere)
{
    ParticipantRegistry people;
    HospitalSeed::seedParticipants(people);

    WorkRegistry registryA;
    WorkRegistry registryB;
    HospitalAdmissionBuilder builderA(registryA);
    HospitalAdmissionBuilder builderB(registryB);
    HospitalAdmissionCoordinator coordinatorA(people);
    HospitalAdmissionCoordinator coordinatorB(people);

    HospitalAdmissionDirector(builderA).buildDemoWard();
    HospitalAdmissionDirector(builderB).buildDemoWard();
    builderA.getRoot()->setMediator(&coordinatorA);
    builderB.getRoot()->setMediator(&coordinatorB);

    coordinatorA.openWard(*builderA.getCurrentContainer());

    EXPECT_EQ("Active", builderA.getCurrentContainer()->getPhaseName());
    EXPECT_EQ("Intake", builderB.getCurrentContainer()->getPhaseName());
    EXPECT_EQ("Available", dynamic_cast<Item*>(registryB.get("TRT-1"))->getStateName());
    EXPECT_FALSE(coordinatorA.getHistoryAsStrings().empty());
    EXPECT_TRUE(coordinatorB.getHistoryAsStrings().empty());
}

TEST(HospitalInstance, RunsThroughTheGenericWorkflowInstance)
{
    ParticipantRegistry people;
    HospitalSeed::seedParticipants(people);

    std::unique_ptr<WorkRegistry> registry(new WorkRegistry());
    std::unique_ptr<HospitalAdmissionBuilder> builder(new HospitalAdmissionBuilder(*registry));
    HospitalAdmissionDirector(*builder).buildEmptyWard();
    Root* root = builder->getRoot();

    std::unique_ptr<HospitalAdmissionCoordinator> coordinator(new HospitalAdmissionCoordinator(people));
    HospitalAdmissionCoordinator* coordinatorView = coordinator.get();

    WorkflowInstance instance("hc-1", "healthcare", std::move(registry), root,
                              std::move(coordinator),
                              std::unique_ptr<AssignmentStrategy>(new WorkloadStrategy()),
                              std::move(builder), &people);

    instance.addItem("W1", "TRT-9", "Zanele", "P-9");
    ASSERT_NE(nullptr, instance.getItem("TRT-9"));
    EXPECT_EQ("Available", instance.getItem("TRT-9")->getStateName());
    EXPECT_TRUE(historyContains(instance.getHistory(), "TRT-9 added"));

    Container* ward = instance.getContainer("W1");
    ASSERT_NE(nullptr, ward);
    coordinatorView->openWard(*ward);

    instance.startItem("TRT-9");
    instance.completeItem("TRT-9", 5);
    EXPECT_EQ("Review", ward->getPhaseName());

    instance.approveContainer("W1", people.getByRole(HospitalRoles::chiefOfMedicine()).front());
    EXPECT_EQ("Publish", ward->getPhaseName());
    EXPECT_THROW(instance.completeItem("TRT-9", 5), InvalidTransition);
}
