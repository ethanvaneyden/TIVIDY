#pragma once

#include "Component.h"
#include <memory>
#include <string>

class ItemState;
class Participant;
class Container;

/**
 * @brief A leaf workflow component with an execution and review lifecycle.
 * 
 * This class acts as the **Context** in the State pattern and the **Leaf** 
 * in the Composite pattern.
 * 
 * It owns its lifecycle state (via std::unique_ptr) but does NOT own its 
 * assignee or reviewer (those are owned by the ParticipantRegistry).
 * 
 * Operations like start() and complete() delegate to the current ItemState. 
 * If an operation is invalid for the current state, the state class will throw 
 * a std::logic_error.
 */
class Item : public Component {
private:
    /**
     * @brief Installs a state returned by the current state transition.
     * @param next Owning pointer to the next state.
     * @throws std::invalid_argument If next is null.
     */
    void setState(std::unique_ptr<ItemState> next);

protected:
    /// @brief Owned lifecycle state for this item (e.g., Available, Assigned).
    std::unique_ptr<ItemState> state;
    
    /// @brief Non-owning participant assigned to perform the work.
    Participant* assignee;
    
    /// @brief Non-owning participant who reviewed or resolved the result.
    Participant* reviewer;
    
    /// @brief Review method ("Batch" or "Personal"), or empty if not reviewed.
    std::string reviewMethod;

    /// @brief Recorded result value (mark or severity).
    int result;
    
    /// @brief Flag indicating if a result has been recorded.
    bool resultRecorded;
    
    /// @brief Reason retained when a result is rejected or overridden.
    std::string rejectionReason;

public:
    /**
     * @brief Constructs an item in its initial state (usually Available).
     * @param id Unique identifier for this component.
     * @param type Item type (e.g., "Submission", "Treatment").
     * @param name Display name for the UI.
     * @param parent Optional non-owning reference to the parent Container.
     */
    Item(std::string id, std::string type, std::string name, Component* parent = nullptr);

    /**
     * @brief Destroys the item and its owned state object.
     */
    virtual ~Item();

    /**
     * @brief Creates an iterator for traversing this leaf.
     * @return A SingleItemIterator.
     */
    Iterator* createIterator() override;
     Iterator* createIterator(const std::string& state) override;

    /**
     * @brief Checks whether the current lifecycle state is "Completed".
     */
    bool isCompleted() const override;

    // ==========================================
    // STATE TRANSITIONS (Delegates to ItemState)
    // ==========================================

    /**
     * @brief Assigns this item to a participant.
     * @param p Non-null participant to assign.
     * @throws std::invalid_argument If p is null.
     * @throws std::logic_error If the current state does not allow assignment.
     */
    void assign(Participant* p);

    /**
     * @brief Starts work on this item.
     * @throws std::logic_error If the current state does not allow starting.
     */
    void start();

    /**
     * @brief Records completion with a result value.
     * @param value Result value to record (mark or severity).
     * @throws std::logic_error If the current state does not allow completion.
     */
    void complete(int value);

    /**
     * @brief Rejects a result during the Review phase.
     * @param reason Explanation for the rejection.
     * @throws std::logic_error If the current state does not allow rejection.
     */
    void reject(const std::string& reason);

    /**
     * @brief Resolves a rejected result through personal review.
     * @param reviewer Non-null participant resolving the rejection.
     * @param value Replacement result value.
     * @throws std::invalid_argument If reviewer is null.
     * @throws std::logic_error If there is no rejected result to resolve.
     */
    void resolveReview(
        Participant* reviewer,
        int value,
        const std::string& reason = std::string());

    // ==========================================
    // STATE MANAGEMENT
    // ==========================================

    /**
     * @brief Returns the name of the current lifecycle state.
     */
    std::string getStateName() const;

    // ==========================================
    // DATA ACCESSORS
    // ==========================================

    /// @brief Returns the non-owning assignee, or nullptr if none is set.
    Participant* getAssignee() const;

    /// @brief Returns the non-owning reviewer, or nullptr if not reviewed.
    Participant* getReviewer() const;

    /// @brief Returns the review method, or an empty string if not reviewed.
    const std::string& getReviewMethod() const;

    /// @brief Checks whether a result value has been recorded.
    bool hasResult() const;

    /**
     * @brief Returns the recorded result.
     * @throws std::logic_error If no result has been recorded yet.
     */
    int getResult() const;

    /// @brief Returns the rejection reason, or an empty string if never rejected.
    const std::string& getRejectionReason() const;

    /// @brief Checks whether the item is completed and has a recorded review.
    bool isReviewed() const;

    // ==========================================
    // SETTERS (Called by State classes & Mediator)
    // ==========================================

    /**
     * @brief Sets the assignee and updates the participant's item references.
     * @param p Non-owning pointer to the assigned participant.
     */
    void setAssignee(Participant* p);

    /**
     * @brief Sets the reviewer and records how they reviewed it.
     * @param p Non-owning pointer to the reviewer.
     * @param method "Batch" or "Personal".
     */
    void setReviewer(Participant* p, const std::string& method);

    /**
     * @brief Records an approved review without changing the item state.
     * @param p Non-owning reviewer pointer.
     * @param method Review method, such as "Batch".
     * @throws std::invalid_argument If p is null or method is empty.
     * @throws InvalidTransition If this item is not completed.
     */
    void recordReview(Participant* p, const std::string& method);

    /**
     * @brief Sets the result value and marks it as recorded.
     * @param value The mark or severity.
     */
    void setResult(int value);

    /**
     * @brief Stores the reason why the item was rejected.
     * @param reason The rejection explanation.
     */
    void setRejectionReason(const std::string& reason);

    // ==========================================
    // PHASE ENFORCEMENT
    // ==========================================

    /**
     * @brief Ensures the parent container is in the expected phase.
     * @param expected The required phase name (e.g., "Active" or "Review").
     * @throws std::logic_error If the parent container is not in that phase.
     */
    void requirePhase(const std::string& expected) const;
};