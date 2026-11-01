/**
 * @file HospitalAdmissionDirector.h
 * @brief Director for the Hospital scenario: the "recipes" for a workflow.
 */

#pragma once

#include <string>

namespace hospital
{

class HospitalAdmissionBuilder;

class HospitalAdmissionDirector
{
    private:
        HospitalAdmissionBuilder& builder;

    public:
        explicit HospitalAdmissionDirector(HospitalAdmissionBuilder& workflowBuilder);

        /** @brief A hospital with one ward and no treatments yet. */
        void buildEmptyWard();

        /** @brief A hospital with one ward and the demo patients from HospitalSeed. */
        void buildDemoWard();

        static const char* hospitalName();
        static const char* wardId();
};

}
