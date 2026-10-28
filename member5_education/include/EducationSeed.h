/**
 * @file EducationSeed.h
 * @brief Demo data for the Education scenario: participants and sample
 */

#pragma once

#include <string>
#include <vector>

class ParticipantRegistry;

namespace education
{

/**
 * @struct DemoSubmission
 * @brief One sample submission for demos and tests.
 */
struct DemoSubmission
{
    std::string itemId;      /**< Item identifier, e.g. "SUB-A". */
    std::string studentName; /**< Student who submitted. */
    int mark;                /**< Mark the operator will enter on completion. */
};

/**
 * @class EducationSeed
 * @brief Creates the standard cast and sample submissions.
 */
class EducationSeed
{
    public:
        /**
         * @brief Register 3 TAs, 1 lecturer and 1 moderator.
         * @param registry Registry to add them to.
         */
        static void seedParticipants(ParticipantRegistry& registry);

        /**
         * @brief Eight submissions whose marks cover every approval band.
         *
         * Includes marks in each band, the boundary values, and two marks
         * below 40 so the Rejected path can be demonstrated.
         */
        static std::vector<DemoSubmission> demoSubmissions();
};

} // namespace education
