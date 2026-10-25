#ifndef ITEM_H
#define ITEM_H
#include "Component.h"
#include <memory>

class ItemState;
class Participant;

class Item : public Component {
protected:
    std::unique_ptr<ItemState> state;
    Participant* assignee;
    Participant* reviewer;
    std::string reviewMethod; // Batch / Personal

public:
    Item(std::string id, std::string type, std::string name, Component* parent = nullptr);
    virtual ~Item() = default;

    Iterator* createIterator() override;
    bool isCompleted() const override;

    void assign(Participant* p);
    void start();
    void complete(int value);
    void reject(const std::string& reason);

    std::string getStateName() const;
    void setState(std::unique_ptr<ItemState> s);

    Participant* getAssignee() const;
    void setAssignee(Participant* p);

    Participant* getReviewer() const;
    const std::string& getReviewMethod() const;
    void recordReview(Participant* p, const std::string& method);

};

#endif