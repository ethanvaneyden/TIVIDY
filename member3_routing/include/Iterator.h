/**
 * @file Iterator.h
 * @brief STUB — replaced by Member 3's full implementation.
*/

#ifndef ITERATOR_H
#define ITERATOR_H

class Component;

class Iterator
{
    public:
        Iterator() = default;
        virtual ~Iterator() = default;

        virtual bool hasNext() const = 0;
        virtual Component* next() = 0;
};

#endif