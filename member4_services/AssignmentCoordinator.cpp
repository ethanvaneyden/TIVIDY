#include "include/AssignmentCoordinator.h"

AssignmentCoordinator::AssignmentCoordinator() = default;

AssignmentCoordinator::~AssignmentCoordinator() = default;

void AssignmentCoordinator::notify(Colleague* sender, const std::string& event) {
    // Handle mediator notifications here.
    // The actual routing depends on the event names
    // and workflow behaviour agreed upon by the team.
    (void)sender;
    (void)event;
}

void AssignmentCoordinator::onSubmissionReceived(WorkItem* item) {
    // TODO: coordinate assignment when a submission is received.
    (void)item;
}

void AssignmentCoordinator::onMarkingCompleted(WorkItem* item) {
    // TODO: coordinate the next step after marking.
    (void)item;
}

void AssignmentCoordinator::onModerationCompleted(WorkItem* item) {
    // TODO: coordinate the next step after moderation.
    (void)item;
}
