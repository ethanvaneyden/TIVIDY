#pragma once

#include "DepthFirstIterator.h"
#include "Component.h"
#include "Item.h"

class StateFilterIterator : public DepthFirstIterator
{
private:
    std::string state;
    Component *FindTarget();

public:
    StateFilterIterator(Component *root, const std::string &state);
    virtual Component *first();
    virtual Component *next();
};
