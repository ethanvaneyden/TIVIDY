#ifndef WORKITERATORITERATOR_H
#define WORKITERATORITERATOR_H
class Component;

class WorkIterator
{
public:
    virtual ~WorkIterator();

    virtual Component *first() = 0;
    virtual Component *next() = 0;
    virtual bool isDone() = 0;
    virtual Component *current() = 0;
};

#endif