#pragma once

#include "DepthFirstIterator.h"
#include "Component.h"
#include "Item.h"
/**
 * @file StateFilterIterator.h
 * @brief Depth-first iterator that only stops on items in a chosen state.
 */

/**
 * @class StateFilterIterator
 * @brief Walks the tree depth-first and yields only items whose state name matches.
 *
 * Reuses DepthFirstIterator for the walk and adds a skip loop on top, so the
 * traversal code is not duplicated. The state is chosen at construction, so one
 * class covers every item state, for example "Available" (to find work to
 * assign) or "Rejected" (the operator's review queue).
 *
 * Containers and the root inherit the default Component::getStateName(), which
 * returns an empty string, so only items can ever match.
 *
 * @code
 * StateFilterIterator rejected(root, "Rejected");
 * for (rejected.first(); !rejected.isDone(); rejected.next())
 *     review(rejected.current());
 * @endcode
 *
 * @note The state string must match what ItemState::getName() returns exactly,
 *       including case. A misspelt state simply produces an empty traversal.
 */
class StateFilterIterator : public DepthFirstIterator
{
private:
    /** @brief The item state name to match. */
    std::string state;
    /**
     * @brief Advances (using DepthFirstIterator::next) until the current node matches.
     * @return The matching node, or nullptr if no further match exists.
     */
    Component *FindTarget();

public:
    /**
     * @brief Creates a filtering iterator.
     * @param root        Node to start the traversal from.
     * @param targetstate Item state name to look for, e.g. "Available".
     */
    StateFilterIterator(Component *root, const std::string &state);
    /**
     * @brief Resets to the start and moves to the first matching item.
     * @return The first matching item, or nullptr if there is none.
     */
    virtual Component *first();
    /**
     * @brief Moves to the next matching item.
     * @return The next matching item, or nullptr when no more match.
     */
    virtual Component *next();
};
