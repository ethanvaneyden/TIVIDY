/**
 * @file HospitalSeed.h
 * @brief Demo data for the Hospital scenario: participants and sample patients.
 *
 * Nothing here is used by the generic engine.
 */

#pragma once

#include <string>
#include <vector>

class ParticipantRegistry;

namespace hospital
{

/**
 * @struct DemoPatient
 * @brief One sample patient for demos and tests.
 */
struct DemoPatient
{
    std::string itemId;    /* Treatment identifier, e.g. "TRT-1". */
    std::string name;      /* Patient name. */
    std::string patientId; /* Patient identifier, e.g. "P-1001". */
    std::string diagnosis; /* Working diagnosis. */
    int severity;          /* Severity the doctor will enter on completion. */
};

/**
 * @class HospitalSeed
 * @brief Creates the standard cast and sample patients.
 */
class HospitalSeed
{
    public:
        /**
         * @brief Register 3 doctors, 1 triage nurse, 1 admitting doctor and
         *        1 chief of medicine.
         */
        static void seedParticipants(ParticipantRegistry& registry);

        /** @brief Three patients, one per approval band (severity 2, 5 and 9). */
        static std::vector<DemoPatient> demoPatients();

        /** @brief A patient whose severity (0) the chain cannot approve. */
        static DemoPatient invalidSeverityPatient();
};

}
