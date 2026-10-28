#pragma once

#include <memory>
#include <string>
#include <vector>
#include "AssignmentStrategy.h"

class Item;

class Participant;

/**
 * @class AssignmentEngine
 * @brief Strategy Context: holds the active AssignmentStrategy and delegates to it.
 *
 * The engine never knows which concrete strategy it holds. It only CHOOSES a
 * worker; it never changes the item. The AssignItem command takes the returned
 * worker and calls Item::assign(), which does the state change and pending count.
 *
 * The strategy is chosen from outside: by the scenario's factory at
 * construction, or at runtime through setStrategy().
 */
class AssignmentEngine
{
private:
    std::unique_ptr<AssignmentStrategy> strategy;

public:
    /**
     * @brief Creates an engine with a starting strategy.
     * @param strategy The algorithm to use. Ownership moves to the engine.
     */
    explicit AssignmentEngine(std::unique_ptr<AssignmentStrategy> strategy);

    /**
     * @brief Replaces the active strategy at runtime.
     * @param s The new algorithm. Ownership moves to the engine; the old one is deleted.
     * @warning Do not pass nullptr; the engine then has no strategy and assign() returns nullptr.
     */
    void setStrategy(std::unique_ptr<AssignmentStrategy> s);
    /// @brief Name of the active strategy, or "None" if there is none.
    std::string strategyName() const;

    /**
     * @brief Chooses a worker for an item. Does not modify the item.
     *
     * Removes nullptrs from the pool, then asks the active strategy to choose.
     *
     * @param item The item needing a worker (not modified; available to future strategies).
     * @param pool Candidate workers, e.g. registry.getByRole("TA").
     * @return The chosen worker, or nullptr if there is no strategy or the
     *         pool is empty or holds only nullptrs.
     */
    Participant *assign(Item &item, const std::vector<Participant *> &pool);
};
