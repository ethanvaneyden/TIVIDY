/**
 * @file Iterator.h
 * @brief STUB — replaced by Member 3's full implementation.
 */

#pragma once

class Component;

class Iterator
{
public:
    virtual ~Iterator() = default;

    virtual Component *first() = 0;
    virtual Component *next() = 0;
    virtual bool isDone() = 0;
    virtual Component *current() = 0;
};
