#ifndef ASSIGNMENTENGINE_H
#define ASSIGNMENTENGINE_H

#include <memory>
#include <string>
#include <vector>
#include "AssignmentStrategy.h"

class Item;
class Iterator;
class Participant;

class AssignmentEngine
{
private:
    std::unique_ptr<AssignmentStrategy> strategy;

public:
    explicit AssignmentEngine(std::unique_ptr<AssignmentStrategy> strategy);

    void setStrategy(std::unique_ptr<AssignmentStrategy> s);
    std::string strategyName() const;

    Participant *assign(Item &item, const std::vector<Participant *> &pool);

    int assignAvailable(Iterator &it, const std::vector<Participant *> &pool);
};

#endif