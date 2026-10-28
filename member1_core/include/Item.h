#ifndef ITEM_H
#define ITEM_H
#include "Component.h"
#include <memory>

class ItemState;
class Participant;

/**
 * @brief A leaf workflow component with an execution and review lifecycle.
 *
 * An item tracks its current lifecycle state, assignee, reviewer, result, and
 * any rejection reason. When attached to a container, execution operations
 * must occur in the appropriate workflow phase. The item does not own its
 * parent, assignee, or reviewer.
 */
class Item : public Component {
protected:
    /// @brief Owned lifecycle state for this item.
    std::unique_ptr<ItemState> state;
    /// @brief Non-owning participant assigned to perform the work.
    Participant* assignee;
    /// @brief Non-owning participant who reviewed or resolved the result.
    Participant* reviewer;
    /// @brief Review method ("Batch" or "Personal"), or empty if not reviewed.
    std::string reviewMethod;

private:
    /// @brief Recorded result value; meaningful only when resultRecorded is true.
    int result;
    /// @brief Whether a result has been recorded.
    bool resultRecorded;
    /// @brief Reason retained when a result is rejected, including after resolution.
    std::string rejectionReason;

    /// @brief Lifecycle operation dispatched to the current state.
    enum class Action { Assign, Start, Complete, Reject };

    /**
     * @brief Dispatches an operation to the current state.
     * @param action Operation to perform.
     * @param value Result value for completion, if applicable.
     * @param reason Rejection reason, if applicable.
     */
    void transition(Action action, int value = 0, const std::string& reason = "");

    /**
     * @brief Ensures the parent container is in the expected phase.
     * @param expected Required phase name.
     * @throws InvalidTransition If an attached item is not in that phase.
     */
    void requirePhase(const std::string& expected) const;

    /// @brief Notifies the associated mediator that this item changed.
    void notifyChange(const std::string& event);

public:
    /**
     * @brief Constructs an item in its initial Available state.
     * @param id Unique identifier for this component.
     * @param type Item type.
     * @param name Display name.
     * @param parent Optional non-owning reference to the parent component.
     */
    Item(std::string id, std::string type, std::string name, Component* parent = nullptr);

    /// @brief Destroys the item and its owned lifecycle state.
    virtual ~Item();

    /**
     * @brief Creates an iterator for traversing this item.
     * @return An iterator, or nullptr when traversal is unavailable.
     */
    Iterator* createIterator() override;

    /// @brief Checks whether the current lifecycle state is completed.
    bool isCompleted() const override;

    /**
     * @brief Assigns this item to a participant.
     * @param p Non-null participant to assign.
     * @throws std::invalid_argument If p is null.
     * @throws InvalidTransition If the attached item's container is not Active
     *         or the current state does not allow assignment.
     */
    void assign(Participant* p);

    /**
     * @brief Starts work on this item.
     * @throws InvalidTransition If the attached item's container is not Active,
     *         no assignee is set, or the current state disallows starting.
     */
    void start();

    /**
     * @brief Records completion with a result value.
     * @param value Result value to record.
     * @throws InvalidTransition If the attached item's container is not Active,
     *         the result was rejected, or the current state disallows completion.
     */
    void complete(int value);

    /**
     * @brief Rejects a result during the Review phase.
     * @param reason Non-empty explanation for the rejection.
     * @throws std::invalid_argument If reason is empty.
     * @throws InvalidTransition If the attached item's container is not Review
     *         or the current state does not allow rejection.
     */
    void reject(const std::string& reason);

    /// @brief Returns the name of the current lifecycle state.
    std::string getStateName() const;

    /**
     * @brief Replaces the item's lifecycle state.
     * @param s New non-null state to own.
     * @throws std::invalid_argument If s is null.
     */
    void setState(std::unique_ptr<ItemState> s);

    /// @brief Returns the non-owning assignee, or nullptr if none is set.
    Participant* getAssignee() const;

    /**
     * @brief Sets the assignee directly for internal collaborators.
     * @param p Non-owning assignee pointer; may be nullptr.
     *
     * User-facing assignment should use assign().
     */
    void setAssignee(Participant* p);

    /// @brief Returns the non-owning reviewer, or nullptr if not reviewed.
    Participant* getReviewer() const;

    /// @brief Returns the review method, or an empty string if not reviewed.
    const std::string& getReviewMethod() const;

    /**
     * @brief Records approval of a completed result.
     * @param p Non-null participant who reviewed the item.
     * @param method Review method; must be "Batch" or "Personal".
     * @throws std::invalid_argument If p is null or method is unsupported.
     * @throws InvalidTransition If the attached item's container is not Review
     *         or the item has no completed result to approve.
     */
    void recordReview(Participant* p, const std::string& method);

    /// @brief Checks whether a result value has been recorded.
    bool hasResult() const;

    /**
     * @brief Returns the recorded result.
     * @return The result value.
     * @throws InvalidTransition If no result has been recorded.
     */
    int getResult() const;

    /// @brief Checks whether the item is completed and has a recorded review.
    bool isReviewed() const;

    /// @brief Returns the rejection reason, or an empty string if never rejected.
    const std::string& getRejectionReason() const;

    /**
     * @brief Resolves a rejected result through personal review.
     * @param reviewer Non-null participant resolving the rejection.
     * @param value Replacement result value.
     * @throws std::invalid_argument If reviewer is null.
     * @throws InvalidTransition If the attached item's container is not Review
     *         or there is no rejected result to resolve.
     *
     * The original assignee and rejection reason are retained.
     */
    void resolveReview(Participant* reviewer, int value);

};

#endif
