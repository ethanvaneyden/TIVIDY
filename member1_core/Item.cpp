#include "Item.h"
#include "ItemState.h"
#include "Participant.h"
#include "DepthFirstIterator.h"
#include "CreatedState.h"
#include "Container.h"
#include "InvalidTransition.h"
#include "LifecycleEvents.h"
#include "WorkflowMediator.h"

#include <stdexcept>
#include <utility>

Item::Item(std::string id, std::string type, std::string name, Component* parent)
    : Component(id, type, name, parent), state(new CreatedState()),
      assignee(nullptr), reviewer(nullptr), result(0), resultRecorded(false)
{
}

// ItemState is complete here, so callers can safely destroy Item from its header.
Item::~Item()
{
    setAssignee(nullptr);
    setReviewer(nullptr);
}

Iterator* Item::createIterator()
{
    return new DepthFirstIterator(this);
}

bool Item::isCompleted() const { return state->isCompleted(); }
std::string Item::getStateName() const { return state->name(); }

void Item::setState(std::unique_ptr<ItemState> next)
{
    if (!next) throw std::invalid_argument("An item state cannot be null");
    state = std::move(next);
}

void Item::transition(Action action, int value, const std::string& reason)
{
    // Keep the executing state alive until its method returns, even if that
    // method installs a replacement state.
    std::unique_ptr<ItemState> current = std::move(state);
    try
    {
        switch (action)
        {
        case Action::Assign: current->assign(*this); break;
        case Action::Start: current->start(*this); break;
        case Action::Complete: current->complete(*this, value); break;
        case Action::Reject: current->reject(*this, reason); break;
        }
    }
    catch (...)
    {
        if (!state) state = std::move(current);
        throw;
    }
    if (!state) state = std::move(current);
}

void Item::requirePhase(const std::string& expected) const
{
    if (!parent) return; // Detached items support isolated lifecycle tests.
    const Container* container = dynamic_cast<const Container*>(parent);
    if (!container || container->getPhaseName() != expected)
        throw InvalidTransition("This item operation requires the " + expected + " phase");
}

void Item::notifyChange(const std::string& event)
{
    WorkflowMediator* target = getMediator();
    if (!target && parent) target = parent->getMediator();
    if (target) target->notify(this, event);
}

void Item::assign(Participant* p)
{
    if (!p) throw std::invalid_argument("An assignee is required");
    requirePhase("Active");
    transition(Action::Assign);
    setAssignee(p);
    notifyChange(LifecycleEvents::ItemAssigned);
}

void Item::start()
{
    requirePhase("Active");
    if (!assignee) throw InvalidTransition("An item must have an assignee before starting");
    transition(Action::Start);
    notifyChange(LifecycleEvents::ItemStarted);
}

void Item::complete(int value)
{
    requirePhase("Active");
    if (state->isRejected())
        throw InvalidTransition("Rejected results require personal review");
    transition(Action::Complete, value);
    result = value;
    resultRecorded = true;
    notifyChange(LifecycleEvents::ItemCompleted);
}

void Item::reject(const std::string& reason)
{
    if (reason.empty()) throw std::invalid_argument("A rejection reason is required");
    requirePhase("Review");
    transition(Action::Reject, 0, reason);
    rejectionReason = reason;
    setReviewer(nullptr);
    reviewMethod.clear();
    notifyChange(LifecycleEvents::ItemRejected);
}

Participant* Item::getAssignee() const { return assignee; }

void Item::setAssignee(Participant* p)
{
    if (assignee == p) return;
    if (p) p->addItemReference(this);
    if (assignee) assignee->removeItemReference(this);
    assignee = p;
}

void Item::setReviewer(Participant* p)
{
    if (reviewer == p) return;
    if (p) p->addItemReference(this);
    if (reviewer) reviewer->removeItemReference(this);
    reviewer = p;
}

void Item::participantDestroyed(Participant* p)
{
    if (assignee == p)
        assignee = nullptr;
    if (reviewer == p)
    {
        reviewer = nullptr;
        reviewMethod.clear();
    }
}

Participant* Item::getReviewer() const { return reviewer; }
const std::string& Item::getReviewMethod() const { return reviewMethod; }

void Item::recordReview(Participant* p, const std::string& method)
{
    if (!p) throw std::invalid_argument("A reviewer is required");
    if (method != "Batch" && method != "Personal")
        throw std::invalid_argument("Review method must be Batch or Personal");
    requirePhase("Review");
    if (!isCompleted() || !resultRecorded)
        throw InvalidTransition("Only completed results can be approved; use resolveReview for rejected results");
    setReviewer(p);
    reviewMethod = method;
    notifyChange(LifecycleEvents::ItemReviewed);
}

void Item::resolveReview(Participant* p, int value)
{
    if (!p) throw std::invalid_argument("A reviewer is required");
    requirePhase("Review");
    if (!state->isRejected() || !resultRecorded)
        throw InvalidTransition("Personal resolution requires a rejected result");
    transition(Action::Complete, value);
    result = value;
    setReviewer(p);
    reviewMethod = "Personal";
    // Keep rejectionReason for the audit/history collaborator.
    notifyChange(LifecycleEvents::ItemReviewResolved);
}

bool Item::hasResult() const { return resultRecorded; }

int Item::getResult() const
{
    if (!resultRecorded) throw InvalidTransition("This item has no result yet");
    return result;
}

bool Item::isReviewed() const
{
    return isCompleted() && reviewer && !reviewMethod.empty();
}

const std::string& Item::getRejectionReason() const { return rejectionReason; }
