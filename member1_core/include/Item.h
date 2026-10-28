#ifndef ITEM_H
#define ITEM_H
#include "Component.h"
#include <memory>

class ItemState;
class Participant;
class CreatedState;
class AssignedState;
class InProgressState;
class CompletedState;
class RejectedState;

class Item : public Component {
protected:
    std::unique_ptr<ItemState> state;
    Participant* assignee;
    Participant* reviewer;
    std::string reviewMethod; // Batch / Personal

private:
    int result;
    bool resultRecorded;
    std::string rejectionReason;

    enum class Action { Assign, Start, Complete, Reject };
    void transition(Action action, int value = 0, const std::string& reason = "");
    void requirePhase(const std::string& expected) const;
    void notifyChange(const std::string& event);
    void setState(std::unique_ptr<ItemState> s);
    void setAssignee(Participant* p);
    void setReviewer(Participant* p);
    void participantDestroyed(Participant* p);

    friend class CreatedState;
    friend class AssignedState;
    friend class InProgressState;
    friend class CompletedState;
    friend class RejectedState;
    friend class Participant;

public:
    Item(std::string id, std::string type, std::string name, Component* parent = nullptr);
    virtual ~Item();

    Iterator* createIterator() override;
    bool isCompleted() const override;

    void assign(Participant* p);
    void start();
    void complete(int value);
    void reject(const std::string& reason);

    std::string getStateName() const;

    Participant* getAssignee() const;

    Participant* getReviewer() const;
    const std::string& getReviewMethod() const;
    void recordReview(Participant* p, const std::string& method);

    bool hasResult() const;
    int getResult() const;
    bool isReviewed() const;
    const std::string& getRejectionReason() const;
    // A rejected result is resolved by a human, retaining the original assignee.
    void resolveReview(Participant* reviewer, int value);

};

#endif
