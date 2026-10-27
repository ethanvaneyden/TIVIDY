#ifndef DEPTHFIRSTITERATOR_H
#define DEPTHFIRSTITERATOR_H

#include "WorkIterator.h"
#include "Component.h"

class DepthFirstIterator : WorkIterator
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

#endif;