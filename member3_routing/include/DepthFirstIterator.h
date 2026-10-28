#pragma once

#include "Iterator.h"
#include "Component.h"
/**
 * @file DepthFirstIterator.h
 * @brief Pre-order depth-first iterator over the Root -> Container -> Item tree.
 */

/**
 * @class DepthFirstIterator
 * @brief Visits a node, then each of its children in order, then moves to the next sibling.
 *
 * For the tree Root -> (C1 -> A, B), (C2 -> C) the visiting order is
 * Root, C1, A, B, C2, C. next() goes down to the first child if there is one,
 * otherwise across to the next sibling, otherwise up through getParent() until
 * an ancestor has a next sibling. The climb stops at the starting node, so an
 * iterator started on a Container never leaves that subtree.
 *
 * @note Relies on Container::add() calling setParent() on every child, and on
 *       leaves returning an empty vector from getChildren().
 */
class DepthFirstIterator : public Iterator
{
private:
    /** @brief Node the traversal starts from; the climb in next() never goes above it. */
    Component *root;
    /** @brief Node the iterator is currently on, or nullptr when the traversal is done. */
    Component *curr;

public:
    /**
     * @brief Creates an iterator positioned on the given node.
     * @param root Node to start from (may be a Root, Container or Item).
     */
    DepthFirstIterator(Component *root);
    /**
     * @brief Moves back to the starting node.
     * @return The starting node.
     */
    virtual Component *first();
    /**
     * @brief Moves to the next node in depth-first order.
     * @return The next node, or nullptr when the whole subtree has been visited.
     */
    virtual Component *next();
    /** @brief Returns true once next() has run off the end of the tree. */
    virtual bool isDone();
    /** @brief Returns the current node, or nullptr if done. */
    virtual Component *current();
};
