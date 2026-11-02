/**
 * @file Iterator.h
 * @brief Abstract external iterator for traversing the work composite tree.
 *
 * Part of the Iterator pattern. The client owns the loop and drives the
 * traversal with first(), next(), isDone() and current(), so the traversal
 * logic is kept out of Component, Container and Item.
 *
 * @code
 * Iterator *it = root->createIterator();
 * for (it->first(); !it->isDone(); it->next())
 *     use(it->current());
 * delete it;
 * @endcode
 */

/**
 * @class Iterator
 * @brief Interface that every traversal of the composite tree implements.
 */
#pragma once

class Component;

class Iterator
{
public:
    /** @brief Virtual destructor so iterators can be deleted through a base pointer. */
    virtual ~Iterator() = default;
    /**
     * @brief Resets the traversal to its starting position.
     * @return The first element, or nullptr if there is none.
     */
    virtual Component *first() = 0;
    /**
     * @brief Advances the traversal by one element.
     * @return The new current element, or nullptr when the traversal has ended.
     */
    virtual Component *next() = 0;
    /**
     * @brief Checks whether the traversal has moved past the last element.
     * @return true if there is no current element, false otherwise.
     */
    virtual bool isDone() = 0;
    /**
     * @brief Gets the element the iterator is currently on.
     * @return The current element, or nullptr if the traversal is done.
     */
    virtual Component *current() = 0;
};
