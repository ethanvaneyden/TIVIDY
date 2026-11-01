#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "Container.h"
#include "Root.h"
#include "ScenarioFacade.h"
#include "TividyWorkflow.h"

TEST(WorkflowEngineTest, RoutesInstancesToTheirOwningScenario)
{
    TividyWorkflow workflow;
    const std::string educationId = workflow.startEducation("Course");
    const std::string hospitalId = workflow.startHealthcare("Hospital");

    ASSERT_FALSE(educationId.empty());
    ASSERT_FALSE(hospitalId.empty());
    EXPECT_NE(educationId, hospitalId);

    Root* educationRoot = workflow.getTree(educationId);
    Root* hospitalRoot = workflow.getTree(hospitalId);
    ASSERT_NE(educationRoot, nullptr);
    ASSERT_NE(hospitalRoot, nullptr);
    EXPECT_EQ(educationRoot->getType(), "module");
    EXPECT_EQ(hospitalRoot->getType(), "hospital");
    EXPECT_EQ(workflow.getTree("missing"), nullptr);
    EXPECT_TRUE(workflow.getHistory("missing").empty());
}

TEST(WorkflowEngineTest, MutationsAreScopedToTheTargetInstance)
{
    TividyWorkflow workflow;
    const std::string firstId = workflow.startHealthcare("First hospital");
    const std::string secondId = workflow.startHealthcare("Second hospital");
    ASSERT_FALSE(firstId.empty());
    ASSERT_FALSE(secondId.empty());

    Root* firstRoot = workflow.getTree(firstId);
    Root* secondRoot = workflow.getTree(secondId);
    ASSERT_NE(firstRoot, nullptr);
    ASSERT_NE(secondRoot, nullptr);
    ASSERT_EQ(firstRoot->getChildren().size(), 1u);
    ASSERT_EQ(secondRoot->getChildren().size(), 1u);

    EXPECT_EQ(workflow.admitPatient(firstId, "Extra ward"), CommandResult::Success);

    ASSERT_EQ(firstRoot->getChildren().size(), 2u);
    EXPECT_EQ(firstRoot->getChildren().back()->getName(), "Extra ward");
    ASSERT_EQ(secondRoot->getChildren().size(), 1u);
    EXPECT_NE(firstRoot, secondRoot);
}

TEST(WorkflowEngineTest, MissingInstanceMutationsReturnNotFound)
{
    TividyWorkflow workflow;

    EXPECT_EQ(workflow.submit("missing", "Assignment"), CommandResult::NotFound);
    EXPECT_EQ(workflow.admitPatient("missing", "Ward"), CommandResult::NotFound);
    EXPECT_EQ(workflow.approveContainer("missing", "container"), CommandResult::NotFound);
    EXPECT_EQ(workflow.overrideItem("missing", "item", 50, false), CommandResult::NotFound);
}