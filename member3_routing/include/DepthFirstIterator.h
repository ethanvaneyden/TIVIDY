#include "WorkIterator.h"
#include "Component.h"

class DepthFirstIterator : WorkIterator
{
private:
    Component *root;
    Component *current;

public:
    DepthFirstIterator(Component *root);
    virtual bool hasNext() const;
    virtual Component *first();
    virtual Component *next();
    virtual Component *isDone();
    virtual Component *current();
};