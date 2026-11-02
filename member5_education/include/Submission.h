/**
 * @file Submission.h
 * @brief Concrete Leaf for the Education scenario: one student's submission.
 *
 * Extends the shared Item only to hold the student number and the mark.
 * The mark is recorded by AssignmentMarkingCoordinator.
 */

#pragma once

#include "Item.h"

#include <map>
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

        /** @brief Stored attributes plus the student number. */
        std::map<std::string, std::string> getAttributes() const override;

        const std::string& getStudentName() const;
        const std::string& getStudentId() const;

        /** @return true once a mark has been recorded. */
        bool hasMark() const;

        /** @return the recorded mark, or NO_MARK (-1) if none yet. */
        int getMark() const;

        /** @brief Record the mark. Range rules are applied by the approval chain. */
        void setMark(int value);
};

}

