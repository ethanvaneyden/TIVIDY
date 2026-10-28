/**
 * @file Item.cpp
 * @brief Implementation of the Item class.
 * @author Member 1 (Core Infrastructure)
 * @date October 2026
 */

#include "Item.h"
#include "ItemState.h"
#include "Participant.h"
#include "WorkflowMediator.h"
#include "Container.h"
#include <stdexcept>
#include <utility>

Item::Item(std::string id, std::string type, std::string name, Component* parent)
    : Component(std::move(id), std::move(type), std::move(name), parent),
      assignee(nullptr),
      reviewer(nullptr),
      reviewMethod(""),
      result(0),
      resultRecorded(false),
      rejectionReason("")
{
    // Note: The initial state (e.g., AvailableState) is set by the 
    // concrete subclass (Submission/Treatment) in their constructors.
}

Item::~Item() = default;

Iterator* Item::createIterator() {
    return nullptr;
}

bool Item::isCompleted() const {
    return state != nullptr && state->isCompleted();
}

// ==========================================
// STATE TRANSITIONS (Delegates to ItemState)
// ==========================================

void Item::assign(Participant* p) {
    if (!p) throw std::invalid_argument("Cannot assign a null participant");

    requirePhase("Active");

    if (!state) throw std::logic_error("Item has no lifecycle state");
    auto next = state->assign(*this, p);
    setState(std::move(next));
    setAssignee(p);
    if (mediator) mediator->onItemAssigned(this, p);
}

void Item::start() {
    requirePhase("Active");

    if (!assignee) {
        throw std::logic_error("Cannot start an item without an assignee");
    }

    if (!state) throw std::logic_error("Item has no lifecycle state");
    auto next = state->start(*this);
    setState(std::move(next));
    if (mediator) mediator->onItemStarted(this);
}

void Item::complete(int value) {
    requirePhase("Active");

    if (!state) throw std::logic_error("Item has no lifecycle state");
    auto next = state->complete(*this, value);
    setState(std::move(next));
    setResult(value);
    if (mediator) mediator->onItemCompleted(this, value);
}

void Item::reject(const std::string& reason) {
    if (reason.empty()) throw std::invalid_argument("Rejection reason cannot be empty");

    requirePhase("Review");

    if (!state) throw std::logic_error("Item has no lifecycle state");
    auto next = state->reject(*this, reason);
    setState(std::move(next));
    setRejectionReason(reason);

    if (mediator) mediator->onItemRejected(this, reason);
}

void Item::resolveReview(
    Participant* reviewer,
    int value,
    const std::string& reason) {
    if (!reviewer) throw std::invalid_argument("Reviewer cannot be null");

    requirePhase("Review");

    if (!state) throw std::logic_error("Item has no lifecycle state");
    auto next = state->resolveReview(*this, reviewer, value);
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
    if (!resultRecorded) {
        throw std::logic_error("No result recorded for this item yet");
    }
    return result;
}

// ==========================================
// SETTERS (Called by State classes & Mediator)
// ==========================================

void Item::setAssignee(Participant* p) {
    // If there was a previous assignee, you might need to notify them here
    assignee = p;
}

void Item::setReviewer(Participant* p, const std::string& method) {
    reviewer = p;
    reviewMethod = method;
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
    if (!parent) return; // Standalone item (e.g., during testing), bypass check

    auto* container = dynamic_cast<const Container*>(parent);
    if (container) {
        std::string currentPhase = container->getPhaseName();
        if (currentPhase != expected) {
            throw std::logic_error("Invalid workflow phase: expected '" + 
                expected + "' but was '" + currentPhase + "'");
        }
    }
}