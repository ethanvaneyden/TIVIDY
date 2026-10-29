/**
 * @file Submission.h
 * @brief Concrete Leaf for the Education scenario: one student's submission.
 *
 * Extends the shared Item only to hold a mark. Item's lifecycle (states,
 * assignee, reviewer, phase rules) is inherited unchanged, and nothing from
 * Item is overridden.
 *
 * The mark is recorded by AssignmentMarkingCoordinator when the item reports
 * that it was completed (or overridden by the lecturer).
 */

#pragma once

#include "Item.h"

#include <string>

namespace education
{

class Submission : public Item
{
    public:
        static const int NO_MARK = -1;

    private:
        int mark;

    public:
        /**
         * @param id          Unique submission id (for example "SUB-A").
         * @param studentName Shown as the item's name.
         */
        Submission(const std::string& id, const std::string& studentName);
        virtual ~Submission();

        const std::string& getStudentName() const;

        /** @return true once a mark has been recorded. */
        bool hasMark() const;

        /** @return the recorded mark, or NO_MARK (-1) if none yet. */
        int getMark() const;

        /** @brief Record the mark. Range rules are applied by the approval chain. */
        void setMark(int value);
};

}

