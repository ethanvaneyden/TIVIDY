/**
 * @file ItemState.h
 * @brief Shared, scenario-independent item lifecycle.
*/

#pragma once

#include <memory>
#include <string>

class Item;
class Participant;

class ItemState
{
public:
    virtual ~ItemState() = default;

    virtual std::unique_ptr<ItemState> assign(Item& item, Participant* participant) = 0;
    virtual std::unique_ptr<ItemState> start(Item& item) = 0;
    virtual std::unique_ptr<ItemState> complete(Item& item, int value) = 0;
    virtual std::unique_ptr<ItemState> reject(Item& item, const std::string& reason) = 0;
    virtual std::unique_ptr<ItemState> resolveReview(
        Item& item,
        Participant* reviewer,
        int newValue) = 0;

    virtual std::string name() const = 0;
    virtual bool isCompleted() const { return false; }
    virtual bool isRejected() const { return false; }
};

class InvalidTransitionState : public ItemState
{
public:
    std::unique_ptr<ItemState> assign(Item& item, Participant* participant) override;
    std::unique_ptr<ItemState> start(Item& item) override;
    std::unique_ptr<ItemState> complete(Item& item, int value) override;
    std::unique_ptr<ItemState> reject(Item& item, const std::string& reason) override;
    std::unique_ptr<ItemState> resolveReview(
        Item& item,
        Participant* reviewer,
        int newValue) override;
};
