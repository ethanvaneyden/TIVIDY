
#include "Item.h"
#include "CreatedState.h"
#include "ItemState.h"
#include "InvalidTransition.h"
#include "DepthFirstIterator.h"
#include "StateFilterIterator.h"
#include "Participant.h"
#include "WorkflowMediator.h"
#include "Container.h"
#include <utility>
#include <stdexcept>

Item::Item(std::string id, std::string type, std::string name)
    : Component(std::move(id), std::move(type), std::move(name)),
      taskPending(false),
      assignee(nullptr),
      reviewer(nullptr),
      reviewMethod(""),
      result(0),
      resultRecorded(false),
      rejectionReason("")
{
    state.reset(new CreatedState());
}

Item::~Item()
{
    if (taskPending && assignee != nullptr)
        assignee->decrementPendingTasks();
    if (assignee != nullptr)
        assignee->releaseItemUse();
    if (reviewer != nullptr && reviewer != assignee)
        reviewer->releaseItemUse();
}

Iterator* Item::createIterator() {
    return new DepthFirstIterator(this);
}

Iterator* Item::createIterator(const std::string& state) {
    return new StateFilterIterator(this, state);
}

bool Item::isCompleted() const {
    return state ? state->isCompleted() : false;
}

// ==========================================
// STATE TRANSITIONS
// ==========================================

void Item::assign(Participant* p) {
    if (!p) throw std::invalid_argument("Cannot assign a null participant");
    requirePhase("Active");
    if (!state) throw std::logic_error("Item has no lifecycle state");

    auto next = state->assign(*this, p);
    if (!next) throw std::logic_error("Item state returned no next state");
    p->incrementPendingTasks();
    try
    {
        setAssignee(p);
    }
    catch (...)
    {
        p->decrementPendingTasks();
        throw;
    }
    taskPending = true;
    setState(std::move(next));
    if (mediator) mediator->onItemAssigned(this, p);
}

void Item::start() {
    requirePhase("Active");
    if (!assignee) throw std::logic_error("Cannot start an item without an assignee");
    if (!state) throw std::logic_error("Item has no lifecycle state");

    auto next = state->start(*this);
    if (!next) throw std::logic_error("Item state returned no next state");
    setState(std::move(next));
    if (mediator) mediator->onItemStarted(this);
}

void Item::complete(int value) {
    requirePhase("Active");
    if (!state) throw std::logic_error("Item has no lifecycle state");

    auto next = state->complete(*this, value);
    if (!next) throw std::logic_error("Item state returned no next state");
    setState(std::move(next));
    setResult(value);
    if (taskPending)
    {
        assignee->decrementPendingTasks();
        taskPending = false;
    }
    if (mediator) mediator->onItemCompleted(this, value);
}

void Item::reject(const std::string& reason) {
    if (reason.empty()) throw std::invalid_argument("Rejection reason cannot be empty");
    requirePhase("Review");
    if (!state) throw std::logic_error("Item has no lifecycle state");

    auto next = state->reject(*this, reason);
    if (!next) throw std::logic_error("Item state returned no next state");
    setState(std::move(next));
    setRejectionReason(reason);
    // A rejected result no longer carries its previous approval.
    setReviewer(nullptr, "");
    if (mediator) mediator->onItemRejected(this, reason);
}

void Item::resolveReview(Participant* reviewer, int value, const std::string& reason) {
    if (!reviewer) throw std::invalid_argument("Reviewer cannot be null");
    requirePhase("Review");
    if (!state) throw std::logic_error("Item has no lifecycle state");

    auto next = state->resolveReview(*this, reviewer, value);
    if (!next) throw std::logic_error("Item state returned no next state");
    setState(std::move(next));
    setResult(value);
    setReviewer(reviewer, "Personal"); 
    if (mediator) mediator->onItemOverridden(this, reviewer, value, reason);
}

// ==========================================
// STATE MANAGEMENT
// ==========================================

void Item::setState(std::unique_ptr<ItemState> s) {
    if (!s) throw std::invalid_argument("Cannot set a null state");
    state = std::move(s);
}

std::string Item::getStateName() const {
    return state ? state->name() : "Unknown";
}

// ==========================================
// DATA ACCESSORS
// ==========================================

Participant* Item::getAssignee() const { return assignee; }
Participant* Item::getReviewer() const { return reviewer; }
const std::string& Item::getReviewMethod() const { return reviewMethod; }
bool Item::hasResult() const { return resultRecorded; }
const std::string& Item::getRejectionReason() const { return rejectionReason; }
bool Item::isReviewed() const { return isCompleted() && reviewer != nullptr; }

int Item::getResult() const {
    if (!resultRecorded) throw std::logic_error("No result recorded");
    return result;
}

// ==========================================
// SETTERS
// ==========================================

void Item::setAssignee(Participant* p)
{
    if (assignee == p)
        return;

    if (p != nullptr && p != reviewer)
        p->retainItemUse();
    if (assignee != nullptr && assignee != reviewer)
        assignee->releaseItemUse();
    assignee = p;
}

void Item::setReviewer(Participant* p, const std::string& method)
{
    std::string nextMethod(method);
    if (reviewer != p && p != nullptr && p != assignee)
        p->retainItemUse();
    if (reviewer != nullptr && reviewer != assignee && reviewer != p)
        reviewer->releaseItemUse();
    reviewer = p;
    reviewMethod.swap(nextMethod);
}
void Item::recordReview(Participant* p, const std::string& method) {
    if (!p) throw std::invalid_argument("Reviewer cannot be null");
    if (method.empty()) throw std::invalid_argument("Review method cannot be empty");
    requirePhase("Review");
    if (!isCompleted()) throw InvalidTransition("Only completed items can be reviewed");
    setReviewer(p, method);
}
void Item::setResult(int value) { 
    result = value; 
    resultRecorded = true; 
}
void Item::setRejectionReason(const std::string& reason) { 
    rejectionReason = reason; 
}

// ==========================================
// PHASE ENFORCEMENT
// ==========================================

void Item::requirePhase(const std::string& expected) const {
    Component* parentComponent = getParent();
    if (!parentComponent)
        throw InvalidTransition("Item must belong to a container before lifecycle operations");
    auto* container = dynamic_cast<const Container*>(parentComponent);
    if (!container)
        throw InvalidTransition("Item parent is not a workflow container");
    const std::string currentPhase = container->getPhaseName();
    if (currentPhase != expected) {
        throw InvalidTransition("Invalid workflow phase: expected '" +
            expected + "' but was '" + currentPhase + "'");
    }
}
