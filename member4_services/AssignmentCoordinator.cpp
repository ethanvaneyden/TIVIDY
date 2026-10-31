#include "include/AssignmentCoordinator.h"

#include <utility>
#include <vector>

#include "../member1_core/include/Item.h"
#include "../member1_core/include/Participant.h"
#include "include/AssignItemCommand.h"
#include "../member3_routing/include/RoundRobinStrategy.h"
#include "include/WorkflowEvent.h"
#include "../member3_routing/include/AssignmentStrategy.h"

AssignmentCoordinator::AssignmentCoordinator(
    WorkRegistry* registry,
    ParticipantRegistry* participantRegistry
)
    : assignmentEngine(
        new AssignmentEngine(
            std::unique_ptr<AssignmentStrategy>(
                new RoundRobinStrategy()
            )
        )
      ),
      history(new ActionHistory()),
      eventBus(new EventBus()),
      registry(registry),
      participantRegistry(participantRegistry)
{
}


AssignmentCoordinator::~AssignmentCoordinator() = default;


void AssignmentCoordinator::notify(
    Colleague* sender,
    const std::string& event
)
{
    if (sender == nullptr) {
        return;
    }

    Item* item = dynamic_cast<Item*>(sender);

    if (item == nullptr) {
        return;
    }

    if (event == "SubmissionReceived") {
        onSubmissionReceived(item);
    }
    else if (event == "MarkingCompleted") {
        onMarkingCompleted(item);
    }
    else if (event == "ModerationCompleted") {
        onModerationCompleted(item);
    }
}


void AssignmentCoordinator::onSubmissionReceived(Item* item)
{
    if (item == nullptr || participantRegistry == nullptr) {
        return;
    }

    // Retrieve the available teaching assistants.
    std::vector<Participant*> participants =
        participantRegistry->getByRole("TA");

    if (participants.empty()) {
        return;
    }

    // Let the assignment engine select a participant.
    Participant* selectedParticipant = assignmentEngine->assign(*item, participants);

    if (selectedParticipant == nullptr) {
        return;
    }

    // Delegate the assignment through a command.
    std::unique_ptr<WorkCommand> command(
        new AssignItemCommand(item, selectedParticipant)
    );

    history->execute(std::move(command));

    // Publish the assignment event.
    WorkflowEvent workflowEvent;
    workflowEvent.type = "ItemAssigned";
    workflowEvent.recipientId = item->getId();
    workflowEvent.payload = selectedParticipant->getId();

    eventBus->publish(workflowEvent);
}


void AssignmentCoordinator::onMarkingCompleted(Item* item)
{
    if (item == nullptr || !item->hasResult()) {
        return;
    }

    // The marking participant has completed the item.
    // The approval chain will handle the approval decision.
}


void AssignmentCoordinator::onModerationCompleted(Item* item)
{
    if (item == nullptr) {
        return;
    }

    // The moderator has completed the review.
    // The coordinator will handle the resulting workflow transition.
}


void AssignmentCoordinator::setAssignmentStrategy(std::unique_ptr<AssignmentStrategy> strategy)
{
    if (strategy != nullptr) {
        assignmentEngine->setStrategy(std::move(strategy));
    }
}

std::vector<std::string> AssignmentCoordinator::getHistory() const
{
    return history->getHistoryAsStrings();
}
