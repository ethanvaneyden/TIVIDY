#ifndef ASSIGNEDSTATE_H
#define ASSIGNEDSTATE_H

#include "ItemState.h"

class AssignedState : public ItemState
{
public:
    void start(Item& item) override;
    std::string name() const override;
};

#endif
