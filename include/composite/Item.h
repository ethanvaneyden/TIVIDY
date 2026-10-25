#ifndef ITEM_H
#define ITEM_H
#include "composite/Component.h"
#include <memory>

class ItemState;
class Participant;

class Item : public Component {
protected:
    std::unique_ptr<ItemState> state;
    Participant* assignee;
    Participant* reviewer;
    int rejectionCount;

public:
    Item(std::string id, std::string type, std::string name, Component* parent = nullptr);
    ~Item() override;

    Iterator* createIterator() override;
    bool isCompleted() const override;

    void assign(const std::string& who);
    void start();
    void complete(int value);
    void reject(const std::string& reason);
    void escalate();
    void fail(const std::string& reason);
    void cancel();

    std::string getStateName() const;
    void setState(std::unique_ptr<ItemState> s);

    Participant* getAssignee() const;
    void setAssignee(Participant* p);

    int getRejectionCount() const;
    void incrementRejectionCount();
};

#endif