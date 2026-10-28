#ifndef DEPTHFIRSTITERATOR_H
#define DEPTHFIRSTITERATOR_H

#include "Iterator.h"
#include "Component.h"

class DepthFirstIterator : public Iterator
{
private:
    Component *root;
    Component *curr;

public:
    DepthFirstIterator(Component *root);

    virtual Component *first();
    virtual Component *next();
    virtual bool isDone();
    virtual Component *current();
};

#endif