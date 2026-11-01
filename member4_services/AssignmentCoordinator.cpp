#include "include/AssignmentCoordinator.h"

#include <utility>

#include "../member1_core/include/Item.h"
#include "../member1_core/include/Container.h"
#include "../member1_core/include/Participant.h"

#include "../member3_routing/include/RoundRobinStrategy.h"
#include "../member3_routing/include/AssignmentStrategy.h"
#include "include/AdvancePhaseCommand.h"
#include "include/ApproveContainerCommand.h"

#include "include/AssignItemCommand.h"
#include "include/OverrideItemCommand.h"
#include "include/RecordResultCommand.h"
#include "include/RejectItemCommand.h"
#include "include/WorkCommand.h"
#include "include/WorkflowEvent.h"


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


// =====================================================
// ITEM EVENTS
// =====================================================

void AssignmentCoordinator::onItemAssigned(
    Item* item,
    Participant* assignee
)
{
    if (item == nullptr || assignee == nullptr || eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "ItemAssigned";
    event.recipientId = item->getId();
    event.payload = assignee->getId();

    eventBus->publish(event);
}


void AssignmentCoordinator::onItemStarted(Item* item)
{
    if (item == nullptr || eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "ItemStarted";
    event.recipientId = item->getId();

    eventBus->publish(event);
}


void AssignmentCoordinator::onItemCompleted(
    Item* item,
    int value
)
{
    if (item == nullptr || eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "ItemCompleted";
    event.recipientId = item->getId();
    event.payload = std::to_string(value);

    eventBus->publish(event);
}


void AssignmentCoordinator::onItemRejected(
    Item* item,
    const std::string& reason
)
{
    if (item == nullptr || eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "ItemRejected";
    event.recipientId = item->getId();
    event.payload = reason;

    eventBus->publish(event);
}


void AssignmentCoordinator::onItemOverridden(
    Item* item,
    Participant* reviewer,
    int newValue,
    const std::string& reason
)
{
    if (item == nullptr || reviewer == nullptr || eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "ItemOverridden";
    event.recipientId = item->getId();
    event.payload =
        reviewer->getId() + ": " +
        std::to_string(newValue) + " - " + reason;

    eventBus->publish(event);
}


// =====================================================
// PHASE EVENTS
// =====================================================

void AssignmentCoordinator::onPhaseAdvanced(Container* container)
{
    if (container == nullptr || eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "PhaseAdvanced";
    event.recipientId = container->getId();

    eventBus->publish(event);
}


// =====================================================
// ASSIGNMENT AND BATCH EVENTS
// =====================================================

void AssignmentCoordinator::onItemAdded(Item* item)
{
    if (item == nullptr || eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "ItemAdded";
    event.recipientId = item->getId();

    eventBus->publish(event);
}


void AssignmentCoordinator::onAssignmentFailed(
    Item* item,
    const std::string& reason
)
{
    if (eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "AssignmentFailed";

    if (item != nullptr) {
        event.recipientId = item->getId();
    }

    event.payload = reason;

    eventBus->publish(event);
}


void AssignmentCoordinator::onContainerApprovalRequested(
    Container* container,
    Participant* operatorParticipant
)
{
    if (container == nullptr || eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "ContainerApprovalRequested";
    event.recipientId = container->getId();

    if (operatorParticipant != nullptr) {
        event.payload = operatorParticipant->getId();
    }

    eventBus->publish(event);
}


void AssignmentCoordinator::onContainerRejected(
    Container* container,
    const std::string& reason
)
{
    if (container == nullptr || eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "ContainerRejected";
    event.recipientId = container->getId();
    event.payload = reason;

    eventBus->publish(event);
}


void AssignmentCoordinator::onApprovalResolved(
    Item* item,
    bool approved
)
{
    if (item == nullptr || eventBus == nullptr) {
        return;
    }

    WorkflowEvent event;
    event.type = "ApprovalResolved";
    event.recipientId = item->getId();
    event.payload = approved ? "Approved" : "Not approved";

    eventBus->publish(event);
}


// =====================================================
// COORDINATOR OPERATIONS
// =====================================================
void AssignmentCoordinator::onSubmissionReceived(Item* item)
{
    if (item == nullptr || history == nullptr ||
        assignmentEngine == nullptr ||
        participantRegistry == nullptr ||
        eventBus == nullptr)
    {
        return;
    }

    std::vector<Participant*> tas =
        participantRegistry->getByRole("TA");

    if (tas.empty())
    {
        onAssignmentFailed(item, "No available TA found");
        return;
    }

    Participant* selected = assignmentEngine->assign(*item, tas);

    if (selected == nullptr)
    {
        onAssignmentFailed(item, "Could not assign a TA");
        return;
    }

    std::unique_ptr<WorkCommand> command(
        new AssignItemCommand(item, selected)
    );

    history->execute(std::move(command));

    WorkflowEvent event;
    event.type = "SubmissionReceived";
    event.recipientId = item->getId();

    eventBus->publish(event);
}


void AssignmentCoordinator::onMarkingCompleted(Item* item)
{
    if (item == nullptr || eventBus == nullptr)
        return;

    WorkflowEvent event;
    event.type = "MarkingCompleted";
    event.recipientId = item->getId();

    if (item->hasResult())
        event.payload = std::to_string(item->getResult());

    eventBus->publish(event);
}



void AssignmentCoordinator::onModerationCompleted(Item* item)
{
    if (item == nullptr || eventBus == nullptr)
        return;

    WorkflowEvent event;
    event.type = "ModerationCompleted";
    event.recipientId = item->getId();
    event.payload = item->isReviewed()
        ? "Approved"
        : "Not yet approved";

    eventBus->publish(event);
}


// =====================================================
// ASSIGNMENT STRATEGY
// =====================================================

void AssignmentCoordinator::setAssignmentStrategy(
    std::unique_ptr<AssignmentStrategy> strategy
)
{
    if (assignmentEngine != nullptr && strategy != nullptr) {
        assignmentEngine->setStrategy(std::move(strategy));
    }
}


// =====================================================
// HISTORY
// =====================================================

std::vector<std::string>
AssignmentCoordinator::getHistoryAsStrings() const
{
    if (history == nullptr) {
        return std::vector<std::string>();
    }

    return history->getHistoryAsStrings();
}

void AssignmentCoordinator::recordResult(Item* item, int value)
{
    if (item == nullptr || history == nullptr) {
        return;
    }

    std::unique_ptr<WorkCommand> command(
        new RecordResultCommand(item, value)
    );

    history->execute(std::move(command));
}

void AssignmentCoordinator::rejectItem(
    Item* item,
    const std::string& reason
)
{
    if (item == nullptr || history == nullptr) {
        return;
    }

    std::unique_ptr<WorkCommand> command(
        new RejectItemCommand(item, reason)
    );

    history->execute(std::move(command));
}

void AssignmentCoordinator::overrideItem(
    Item* item,
    Participant* reviewer,
    int newValue,
    const std::string& reason
)
{
    if (item == nullptr || reviewer == nullptr || history == nullptr) {
        return;
    }

    std::unique_ptr<WorkCommand> command(
        new OverrideItemCommand(
            item,
            reviewer,
            newValue,
            reason
        )
    );

    history->execute(std::move(command));
}

void AssignmentCoordinator::advanceContainerPhase(Container* container)
{
    if (container == nullptr || history == nullptr) {
        return;
    }

    std::unique_ptr<WorkCommand> command(
        new AdvancePhaseCommand(container)
    );

    history->execute(std::move(command));
}

void AssignmentCoordinator::approveContainer(Container* container)
{
    if (container == nullptr ||
        history == nullptr ||
        !container->isReadyToPublish()) {
        return;
        }

    std::unique_ptr<WorkCommand> command(
        new ApproveContainerCommand(container)
    );

    history->execute(std::move(command));
}