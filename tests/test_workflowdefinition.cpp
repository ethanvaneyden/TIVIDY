#include <gtest/gtest.h>

#include "AssignmentMarkingBuilder.h"
#include "AssignmentStrategy.h"
#include "EducationWorkflowFactory.h"
#include "HospitalAdmissionBuilder.h"
#include "HospitalWorkflowFactory.h"
#include "Root.h"
#include "WorkRegistry.h"
#include "WorkflowBuilder.h"
#include "WorkloadStrategy.h"

TEST(WorkflowDefinitionTest, EducationFactoryCreatesItsBuilderAndStrategy)
{
    EducationWorkflowFactory factory;
    WorkRegistry registry;
    std::unique_ptr<WorkflowBuilder> builder = factory.createBuilder();
    std::unique_ptr<AssignmentStrategy> strategy = factory.createStrategy();

    ASSERT_NE(builder, nullptr);
    ASSERT_NE(strategy, nullptr);
    EXPECT_EQ(factory.scenarioName(), "education");
    EXPECT_EQ(strategy->name(), "Workload");

    education::AssignmentMarkingBuilder* educationBuilder =
        dynamic_cast<education::AssignmentMarkingBuilder*>(builder.get());
    ASSERT_NE(educationBuilder, nullptr);
    educationBuilder->setRegistry(registry);
    builder->buildRoot("Course");
    builder->buildContainer("A1", "Assignment 1");
    builder->buildItem("S1", "Student 1");

    ASSERT_NE(builder->getRoot(), nullptr);
    ASSERT_EQ(builder->getRoot()->getChildren().size(), 1u);
    ASSERT_EQ(builder->getRoot()->getChildren().front()->getChildren().size(), 1u);
    EXPECT_EQ(registry.size(), 3u);
}

TEST(WorkflowDefinitionTest, HospitalFactoryCreatesItsBuilderAndStrategy)
{
    HospitalWorkflowFactory factory;
    WorkRegistry registry;
    std::unique_ptr<WorkflowBuilder> builder = factory.createBuilder();
    std::unique_ptr<AssignmentStrategy> strategy = factory.createStrategy();

    ASSERT_NE(builder, nullptr);
    ASSERT_NE(strategy, nullptr);
    EXPECT_EQ(factory.scenarioName(), "healthcare");
    EXPECT_EQ(strategy->name(), "Workload");

    hospital::HospitalAdmissionBuilder* hospitalBuilder =
        dynamic_cast<hospital::HospitalAdmissionBuilder*>(builder.get());
    ASSERT_NE(hospitalBuilder, nullptr);
    hospitalBuilder->setRegistry(registry);
    builder->buildRoot("Hospital");
    builder->buildContainer("W1", "Ward 1");
    builder->buildItem("T1", "Patient 1");

    ASSERT_NE(builder->getRoot(), nullptr);
    ASSERT_EQ(builder->getRoot()->getChildren().size(), 1u);
    ASSERT_EQ(builder->getRoot()->getChildren().front()->getChildren().size(), 1u);
    EXPECT_EQ(registry.size(), 3u);
}

TEST(WorkflowDefinitionTest, FactoryCallsCreateIndependentStrategyInstances)
{
    EducationWorkflowFactory factory;
    std::unique_ptr<AssignmentStrategy> first = factory.createStrategy();
    std::unique_ptr<AssignmentStrategy> second = factory.createStrategy();

    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);
    EXPECT_NE(first.get(), second.get());
}