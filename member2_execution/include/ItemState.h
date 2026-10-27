/**
 * @file ItemState.h
 * @brief STUB — replaced by Member 2's full implementation.
*/

#ifndef ITEMSTATE_H
#define ITEMSTATE_H

#include <string>

class Item;

class ItemState
{
    public:
        ItemState() = default;
        virtual ~ItemState() = default;

        virtual void assign(Item& item) = 0;
        virtual void start(Item& item) = 0;
        virtual void complete(Item& item, int value) = 0;
        virtual void reject(Item& item, const std::string& reason) = 0;
        virtual std::string name() const = 0;
};

#endif