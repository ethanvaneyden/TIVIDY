#include <gtest/gtest.h>

#include "Container.h"
#include "EducationMediator.h"
#include "EducationPolicies.h"
#include "AssignmentMarkingBuilder.h"
#include "JsonSerializer.h"
#include "Participant.h"
#include "ParticipantRegistry.h"
#include "Root.h"
#include "Submission.h"

namespace
{

void moveToReview(Container& container, education::Submission& submission,
                  Participant& assignee, int mark)
{
    container.add(&submission);
    container.advancePhase();
    submission.assign(&assignee);
    submission.start();
    submission.complete(mark);
    container.advancePhase();
}

} // namespace

TEST(EducationSubmissionTest, ValidatesAndStoresMarksOnTheItem)
{
    education::Submission submission("S1", "Student");
    EXPECT_FALSE(submission.hasMark());
    EXPECT_THROW(submission.complete(101), std::invalid_argument);

    EXPECT_EQ(submission.getType(), "submission");
}

TEST(EducationSubmissionTest, AssignmentBuilderCreatesOwnedSubmissionNodes)
{
    education::AssignmentMarkingBuilder builder;
    builder.buildRoot("Module");
    builder.buildContainer("A1", "Assignment");
    builder.buildItem("S1", "Student");
    std::unique_ptr<Root> root = builder.getResult();

    ASSERT_EQ(root->getChildren().size(), 1u);
    Container* assignment = dynamic_cast<Container*>(root->getChildren()[0]);
    ASSERT_NE(assignment, nullptr);
    ASSERT_EQ(assignment->getChildren().size(), 1u);
    EXPECT_NE(dynamic_cast<education::Submission*>(assignment->getChildren()[0]), nullptr);
}

TEST(EducationSubmissionTest, ChainApprovesUsingTheSubmissionAndPublishes)
{
    ParticipantRegistry participants;
    Participant* ta = participants.add("Taylor", education::EducationRoles::ta());
    participants.add("Taylor Two", education::EducationRoles::ta());
    participants.add("Lecturer", education::EducationRoles::lecturer());
    Container assignment("A1", "assignment", "Assignment");
    education::Submission submission("S1", "Student");
    education::EducationMediator mediator(participants);
    assignment.setMediator(&mediator);

    moveToReview(assignment, submission, *ta, 65);
    education::EducationMediator::BatchResult result = mediator.approveBatch(assignment);

    ASSERT_EQ(result.approvedIds.size(), 1u);
    EXPECT_EQ(submission.getMark(), 65);
    EXPECT_TRUE(submission.isReviewed());
    EXPECT_EQ(submission.getReviewer()->getRole(), education::EducationRoles::ta());
    EXPECT_TRUE(mediator.isReadyToPublish(assignment));
    EXPECT_TRUE(assignment.tryPublish());
    EXPECT_EQ(assignment.getPhaseName(), "Publish");
}

TEST(EducationSubmissionTest, SerializerExposesSubmissionTypeAndMark)
{
    ParticipantRegistry participants;
    Participant* assignee = participants.add("Taylor", education::EducationRoles::ta());
    Container assignment("A1", "assignment", "Assignment");
    education::Submission submission("S1", "Student");
    moveToReview(assignment, submission, *assignee, 65);

    const std::string json = JsonSerializer::serializeItem(&submission);
    EXPECT_NE(json.find("\"type\":\"submission\""), std::string::npos);
    EXPECT_NE(json.find("\"mark\":65"), std::string::npos);
}

TEST(EducationSubmissionTest, LecturerResolvesRejectedSubmissionWithoutSidecarState)
{
    ParticipantRegistry participants;
    Participant* ta = participants.add("Taylor", education::EducationRoles::ta());
    Participant* lecturer =
        participants.add("Lecturer", education::EducationRoles::lecturer());
    Container assignment("A1", "assignment", "Assignment");
    education::Submission submission("S1", "Student");
    education::EducationMediator mediator(participants);

    moveToReview(assignment, submission, *ta, 35);
    education::EducationMediator::BatchResult result = mediator.approveBatch(assignment);
    ASSERT_EQ(result.rejectedIds.size(), 1u);
    EXPECT_FALSE(mediator.isReadyToPublish(assignment));

    EXPECT_TRUE(mediator.resolveRejected(submission, true, 0, lecturer));
    EXPECT_EQ(submission.getMark(), 35);
    EXPECT_EQ(submission.getReviewer(), lecturer);
    EXPECT_EQ(submission.getReviewMethod(), "Personal");
    EXPECT_TRUE(mediator.isReadyToPublish(assignment));
}