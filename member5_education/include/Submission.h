/**
 * @file Submission.h
 * @brief Concrete Leaf for the Education scenario: one student's submission.
 *
 * Extends the shared Item only to hold the student number and a mark. Item's lifecycle (states,
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
        std::string studentId;
        int mark;

    public:
        /**
         * @param id          Unique submission id (for example "SUB-A").
         * @param studentName Shown as the item's name.
         * @param studentId   Student number this submission belongs to (may be empty).
         */
        Submission(const std::string& id,
                   const std::string& studentName,
                   const std::string& studentId = std::string());
        virtual ~Submission();

        const std::string& getStudentName() const;
        const std::string& getStudentId() const;

        /** @return true once a mark has been recorded. */
        bool hasMark() const;

        /** @return the recorded mark, or NO_MARK (-1) if none yet. */
        int getMark() const;

        /** @brief Record the mark. Range rules are applied by the approval chain. */
        void setMark(int value);
};

} // namespace education

