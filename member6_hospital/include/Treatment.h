/**
 * @file Treatment.h
 * @brief Concrete Leaf for the Hospital scenario: one patient's treatment episode.
 *
 * Extends the shared Item only to hold patient data and the severity score.
 * Item's lifecycle (states, assignee, reviewer, phase rules) is inherited
 * unchanged.
 *
 * The severity is recorded by HospitalAdmissionCoordinator when the item
 * reports that it was completed (or overridden by the Chief of Medicine).
 */

#pragma once

#include "Item.h"

#include <string>

namespace hospital
{

class Treatment : public Item
{
    public:
        static const int NO_SEVERITY = -1;

    private:
        std::string patientId;
        std::string diagnosis;
        int severity;

    public:
        /**
         * @param id          Unique treatment id (for example "TRT-1").
         * @param patientName Shown as the item's name.
         * @param patientId   Identifier of the patient this episode belongs to.
         * @param diagnosis   Working diagnosis (may be empty until recorded).
         */
        Treatment(const std::string& id,
                  const std::string& patientName,
                  const std::string& patientId,
                  const std::string& diagnosis = std::string());
        virtual ~Treatment();

        const std::string& getPatientName() const;
        const std::string& getPatientId() const;
        const std::string& getDiagnosis() const;

        /** @brief Record or update the working diagnosis. */
        void setDiagnosis(const std::string& value);

        /** @return true once a severity has been recorded. */
        bool hasSeverity() const;

        /** @return the recorded severity, or NO_SEVERITY (-1) if none yet. */
        int getSeverity() const;

        /** @brief Record the severity. Range rules are applied by the approval chain. */
        void setSeverity(int value);
};

}
