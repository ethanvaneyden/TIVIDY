/**
 * @file ItemState.h
 * @brief Shared, scenario-independent item lifecycle.
*/

#pragma once

#include <string>

class Item;

class ItemState
{
    public:
        ItemState() = default;
        virtual ~ItemState() = default;

        virtual void assign(Item& item);
        virtual void start(Item& item);
        virtual void complete(Item& item, int value);
        virtual void reject(Item& item, const std::string& reason);
        virtual std::string name() const = 0;
        virtual bool isCompleted() const { return false; }
        virtual bool isRejected() const { return false; }
};
