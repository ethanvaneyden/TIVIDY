// Tests that prove the design patterns are connected to each other:
// Facade -> Command -> Mediator -> Observer, Strategy -> Mediator,
// Visitor -> Composite, Abstract Factory -> Facade.

#include <gtest/gtest.h>

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ActionHistory.h"
#include "Container.h"
#include "EducationFacade.h"
#include "EducationWorkflowFactory.h"
#include "HospitalFacade.h"
#include "HospitalWorkflowFactory.h"
#include "Item.h"
#include "JsonSerializer.h"
#include "ParticipantRegistry.h"
#include "ProgressReportVisitor.h"
#include "Root.h"
#include "TividyWorkflow.h"
#include "json.h"

namespace
{
    bool has(const std::vector<std::string>& history, const std::string& text)
    {
        for (size_t i = 0; i < history.size(); ++i)
            if (history[i].find(text) != std::string::npos) return true;
        return false;
    }

    Container* firstContainer(Root* root)
    {
        return dynamic_cast<Container*>(root->getChildren().front());
    }

    // Completes every item with `mark` through the facade.
    void completeAll(ScenarioFacade& facade, const std::string& id, int mark)
    {
        Container* c = firstContainer(facade.getTree(id));
        const std::vector<Component*> kids = c->getChildren();
        for (size_t i = 0; i < kids.size(); ++i)
        {
            ASSERT_EQ(CommandResult::Success, facade.startItem(id, kids[i]->getId()));
            ASSERT_EQ(CommandResult::Success, facade.completeItem(id, kids[i]->getId(), mark));
        }
    }
}

class WiringTest : public ::testing::Test
{
protected:
    ParticipantRegistry participants; // destroyed last
    std::unique_ptr<EducationFacade> education;
    std::unique_ptr<HospitalFacade> hospital;

    void SetUp() override
    {
        education.reset(new EducationFacade(&participants));
        hospital.reset(new HospitalFacade(&participants));
    }
};

TEST_F(WiringTest, InstanceIdsCarryTheDisplayName)
{
    const std::string id = education->start("COS 214 Marking");
    EXPECT_EQ("edu_0_COS 214 Marking", id);
    EXPECT_EQ("edu_1_Other", education->start("Other"));
    EXPECT_EQ("hc_0_City", hospital->start("City"));
    EXPECT_EQ("COS 214 Marking", education->getTree(id)->getName());
}

TEST_F(WiringTest, NameThatClashesWithAGeneratedIdStillStarts)
{
    const std::string id = education->start("A1");
    ASSERT_FALSE(id.empty());
    EXPECT_NE(nullptr, education->getTree(id));
}

TEST_F(WiringTest, CommandsAreRecordedInTheAuditTrailInCauseAndEffectOrder)
{
    const std::string id = education->start("Audit");
    ASSERT_EQ(CommandResult::Success, education->startItem(id, "SUB-A"));
    const std::vector<std::string> history = education->getHistory(id);

    const std::vector<std::string>::const_iterator command =
        std::find(history.begin(), history.end(), "Started work on SUB-A");
    const std::vector<std::string>::const_iterator event =
        std::find(history.begin(), history.end(), "Marking started on SUB-A");
    ASSERT_NE(history.end(), command);
    ASSERT_NE(history.end(), event);
    EXPECT_LT(command, event);
    EXPECT_TRUE(has(history, "Opened A1 for work")); // the first start opened the assignment
}

TEST_F(WiringTest, RefusedActionsAreAuditedAsDeniedAndCarryAMessage)
{
    const std::string id = education->start("Denied");
    ASSERT_EQ(CommandResult::Success, education->startItem(id, "SUB-A"));
    EXPECT_EQ(CommandResult::InvalidTransition, education->startItem(id, "SUB-A"));

    EXPECT_NE(std::string::npos, education->lastMessage().find("Cannot start"));
    EXPECT_TRUE(has(education->getHistory(id), "Denied: Started work on SUB-A"));
    EXPECT_EQ(CommandResult::Success, education->startItem(id, "SUB-B"));
    EXPECT_TRUE(education->lastMessage().empty()); // cleared by success
}

TEST_F(WiringTest, ObserverWritesTheReleaseNoticeWhenTheEducationAssignmentPublishes)
{
    const std::string id = education->start("Observer");
    completeAll(*education, id, 60);
    ASSERT_EQ(CommandResult::Success, education->approveContainer(id, "A1"));

    EXPECT_EQ("Publish", firstContainer(education->getTree(id))->getPhaseName());
    EXPECT_TRUE(has(education->getHistory(id), "Marks released for all students"));
}

TEST_F(WiringTest, ObserverWritesTheBillingNoticeWhenTheWardPublishes)
{
    const std::string id = hospital->start("Observer");
    completeAll(*hospital, id, 5);
    ASSERT_EQ(CommandResult::Success, hospital->approveContainer(id, "W1"));

    EXPECT_EQ("Publish", firstContainer(hospital->getTree(id))->getPhaseName());
    EXPECT_TRUE(has(hospital->getHistory(id), "Billing information generated"));
}

TEST_F(WiringTest, ChangingTheStrategyChangesWhoReceivesWork)
{
    const std::string id = education->start("Strategy");
    ASSERT_EQ(CommandResult::Success, education->setStrategy(id, "RoundRobin"));
    EXPECT_EQ("RoundRobin", education->getStrategyName(id));
    EXPECT_TRUE(has(education->getHistory(id), "strategy changed from Workload to RoundRobin"));

    ASSERT_EQ(CommandResult::Success, education->openContainer(id, "A1"));
    Container* a1 = firstContainer(education->getTree(id));
    const std::vector<Component*> kids = a1->getChildren();
    // Round robin over three TAs: submissions 0, 3 and 6 share the first marker.
    Item* s0 = dynamic_cast<Item*>(kids[0]);
    Item* s3 = dynamic_cast<Item*>(kids[3]);
    Item* s1 = dynamic_cast<Item*>(kids[1]);
    ASSERT_NE(nullptr, s0->getAssignee());
    EXPECT_EQ(s0->getAssignee(), s3->getAssignee());
    EXPECT_NE(s0->getAssignee(), s1->getAssignee());
}

TEST_F(WiringTest, AddContainerAndItemWorkForBothScenariosAndRespectTheIntakePhase)
{
    const std::string eid = education->start("Add");
    ASSERT_EQ(CommandResult::Success, education->addContainer(eid, "Test 1"));
    ASSERT_EQ(CommandResult::Success,
              education->addItem(eid, "A2", "Zed", std::map<std::string, std::string>()));
    EXPECT_EQ(1u, education->getTree(eid)->getChildren()[1]->getChildren().size());

    const std::string hid = hospital->start("Add");
    std::map<std::string, std::string> attrs;
    attrs["age"] = "40";
    attrs["diagnosis"] = "Flu";
    ASSERT_EQ(CommandResult::Success, hospital->addItem(hid, "W1", "New Patient", attrs));
    const nlohmann::json tree = nlohmann::json::parse(JsonSerializer::serializeTree(hospital->getTree(hid)));
    const nlohmann::json added = tree.at("children").at(0).at("children").back();
    EXPECT_EQ("New Patient", added.at("name"));
    EXPECT_EQ(40, added.at("age"));
    EXPECT_EQ("Flu", added.at("diagnosis"));

    // Once opened, the container no longer accepts work.
    ASSERT_EQ(CommandResult::Success, hospital->openContainer(hid, "W1"));
    EXPECT_EQ(CommandResult::InvalidTransition,
              hospital->addItem(hid, "W1", "Late", std::map<std::string, std::string>()));
    EXPECT_EQ(CommandResult::NotFound,
              hospital->addItem(hid, "nope", "X", std::map<std::string, std::string>()));
    EXPECT_EQ(CommandResult::Rejected,
              hospital->addItem(hid, "W1", "", std::map<std::string, std::string>()));
}

TEST_F(WiringTest, StartingAnItemInANewContainerOpensThatContainer)
{
    const std::string id = education->start("Open");
    ASSERT_EQ(CommandResult::Success, education->addContainer(id, "Test 2"));
    ASSERT_EQ(CommandResult::Success,
              education->addItem(id, "A2", "Zed", std::map<std::string, std::string>()));
    // The first container is untouched, only A2 is opened.
    ASSERT_EQ(CommandResult::Success, education->startItem(id, "SUB-1"));
    Root* root = education->getTree(id);
    EXPECT_EQ("Intake", dynamic_cast<Container*>(root->getChildren()[0])->getPhaseName());
    EXPECT_EQ("Active", dynamic_cast<Container*>(root->getChildren()[1])->getPhaseName());
}

TEST_F(WiringTest, HospitalSeverityZeroTakesTheExceptionPath)
{
    const std::string id = hospital->start("Exception");
    ASSERT_EQ(CommandResult::Success, hospital->startItem(id, "TRT-1"));
    ASSERT_EQ(CommandResult::Success, hospital->completeItem(id, "TRT-1", 0));
    ASSERT_EQ(CommandResult::Success, hospital->startItem(id, "TRT-2"));
    ASSERT_EQ(CommandResult::Success, hospital->completeItem(id, "TRT-2", 5));
    ASSERT_EQ(CommandResult::Success, hospital->startItem(id, "TRT-3"));
    ASSERT_EQ(CommandResult::Success, hospital->completeItem(id, "TRT-3", 9));
    ASSERT_EQ(CommandResult::Success, hospital->approveContainer(id, "W1"));

    // 0 falls off the chain; the chief must settle it with 1-10.
    Item* rejected = dynamic_cast<Item*>(firstContainer(hospital->getTree(id))->getChildren()[0]);
    EXPECT_EQ("Rejected", rejected->getStateName());
    EXPECT_EQ(CommandResult::Rejected, hospital->overrideItem(id, "TRT-1", 0, true));
    EXPECT_EQ(CommandResult::Success, hospital->overrideItem(id, "TRT-1", 4, false));
    EXPECT_EQ("Publish", firstContainer(hospital->getTree(id))->getPhaseName());
}

TEST_F(WiringTest, OverrideIsLoggedOnce)
{
    const std::string id = education->start("Once");
    ASSERT_EQ(CommandResult::Success, education->startItem(id, "SUB-A"));
    ASSERT_EQ(CommandResult::Success, education->completeItem(id, "SUB-A", 20));
    for (const char* sub : {"SUB-B", "SUB-C", "SUB-D", "SUB-E", "SUB-F", "SUB-G", "SUB-H"})
    {
        ASSERT_EQ(CommandResult::Success, education->startItem(id, sub));
        ASSERT_EQ(CommandResult::Success, education->completeItem(id, sub, 60));
    }
    ASSERT_EQ(CommandResult::Success, education->approveContainer(id, "A1"));
    ASSERT_EQ(CommandResult::Success, education->overrideItem(id, "SUB-A", 55, false));

    int settled = 0;
    const std::vector<std::string> history = education->getHistory(id);
    for (size_t i = 0; i < history.size(); ++i)
        if (history[i].find("Submission SUB-A settled by") != std::string::npos) ++settled;
    EXPECT_EQ(1, settled);
}

TEST_F(WiringTest, VisitorsRenderTheSameTreeThroughAccept)
{
    const std::string id = hospital->start("Visit");
    ASSERT_EQ(CommandResult::Success, hospital->startItem(id, "TRT-1"));
    ASSERT_EQ(CommandResult::Success, hospital->completeItem(id, "TRT-1", 2));

    ProgressReportVisitor report;
    hospital->getTree(id)->accept(report);
    EXPECT_EQ(3, report.totalItems());
    EXPECT_EQ(1, report.completedItems());
    EXPECT_EQ(33, report.percentComplete());
    EXPECT_EQ(1, report.stateCounts().at("Completed"));
    ASSERT_EQ(1u, report.containers().size());
    EXPECT_EQ("Active", report.containers()[0].phase);

    const nlohmann::json progress = nlohmann::json::parse(JsonSerializer::serializeProgress(hospital->getTree(id)));
    EXPECT_EQ(3, progress.at("totalItems"));
    EXPECT_EQ("W1", progress.at("containers").at(0).at("id"));

    const nlohmann::json tree = nlohmann::json::parse(JsonSerializer::serializeTree(hospital->getTree(id)));
    EXPECT_EQ("Active", tree.at("children").at(0).at("phase"));
    EXPECT_EQ(2, tree.at("children").at(0).at("children").at(0).at("result"));
}

TEST_F(WiringTest, InstancesAreIsolatedAndCanBeRemoved)
{
    const std::string a = education->start("One");
    const std::string b = education->start("Two");
    ASSERT_EQ(CommandResult::Success, education->startItem(a, "SUB-A"));

    Item* inA = dynamic_cast<Item*>(firstContainer(education->getTree(a))->getChildren()[0]);
    Item* inB = dynamic_cast<Item*>(firstContainer(education->getTree(b))->getChildren()[0]);
    EXPECT_EQ("InProgress", inA->getStateName());
    EXPECT_EQ("Available", inB->getStateName());

    EXPECT_EQ(CommandResult::Success, education->removeInstance(a));
    EXPECT_EQ(nullptr, education->getTree(a));
    EXPECT_EQ(CommandResult::NotFound, education->removeInstance(a));
    EXPECT_NE(nullptr, education->getTree(b));
}

TEST(FactoryWiringTest, FactoriesDescribeTheirScenarioWithoutTheEngineHardCodingIt)
{
    EducationWorkflowFactory edu;
    HospitalWorkflowFactory hosp;
    EXPECT_EQ("edu", edu.instancePrefix());
    EXPECT_EQ("hc", hosp.instancePrefix());
    EXPECT_TRUE(edu.isEnterableValue(100));
    EXPECT_FALSE(edu.isEnterableValue(101));
    EXPECT_TRUE(hosp.isEnterableValue(0));
    EXPECT_FALSE(hosp.isSettleableValue(0));
    EXPECT_EQ("Lecturer", edu.operatorRole());
    EXPECT_EQ("ChiefOfMedicine", hosp.operatorRole());
}

TEST(TividyWorkflowWiringTest, ScenarioOwnershipIsEnforced)
{
    TividyWorkflow workflow;
    const std::string e = workflow.startEducation("E");
    EXPECT_TRUE(workflow.ownsInstance("education", e));
    EXPECT_FALSE(workflow.ownsInstance("healthcare", e));
    EXPECT_EQ(CommandResult::NotFound, workflow.removeInstance("missing"));
    EXPECT_NE(std::string::npos, workflow.lastMessage().find("missing"));
    EXPECT_EQ(CommandResult::Success, workflow.removeInstance(e));
    EXPECT_FALSE(workflow.ownsInstance("education", e));
}

TEST(ActionHistoryWiringTest, FailedCommandsAreReplacedByADeniedEntry)
{
    class Failing : public WorkCommand
    {
    public:
        void execute() override { throw std::runtime_error("boom"); }
        std::string description() const override { return "do it"; }
    };
    ActionHistory history;
    EXPECT_THROW(history.execute(std::unique_ptr<WorkCommand>(new Failing())), std::runtime_error);
    ASSERT_EQ(1u, history.size());
    EXPECT_EQ("Denied: do it (boom)", history.getHistoryAsStrings()[0]);
}
