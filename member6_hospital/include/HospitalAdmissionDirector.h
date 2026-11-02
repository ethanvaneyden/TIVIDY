/**
 * @file HospitalAdmissionDirector.h
 * @brief Director for the Hospital scenario: the "recipes" for a workflow.
 *
 * GoF Builder - Director. Knows the order of building steps (hospital, ward,
 * treatments) and drives a HospitalAdmissionBuilder through them.
 */

#pragma once

#include "WorkflowBuilder.h"
#include "WorkflowDirector.h"

#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace hospital
{

class HospitalAdmissionDirector : public WorkflowDirector
{
    private:
        WorkflowBuilder& builder;

        void buildDemo(WorkflowBuilder& target, const std::string& rootName);

    public:
        explicit HospitalAdmissionDirector(WorkflowBuilder& workflowBuilder);
        virtual ~HospitalAdmissionDirector();

        /** @brief A hospital with one ward and no treatments yet. */
        void buildEmptyWard();

        /** @brief A hospital with one ward and the demo patients from HospitalSeed. */
        void buildDemoWard();

        /** @brief Same as buildDemoWard() but the hospital carries the given name. */
        void buildDemoWard(const std::string& name);

        /** @brief Build the demo hospital on the given builder. */
        virtual Root* buildDemoTree(WorkflowBuilder& target,
                                    const std::string& rootName) override;

        /**
         * @brief Build a hospital from an explicit specification.
         * @param containers Pairs of (wardId, name).
         * @param items      Triples of (wardId, treatmentId, patient name).
         * @throws std::invalid_argument if an item names an unknown ward (nothing is built).
         */
        virtual Root* buildTree(
            WorkflowBuilder& target,
            const std::string& rootName,
            const std::vector<std::pair<std::string, std::string>>& containers,
            const std::vector<std::tuple<std::string, std::string, std::string>>& items) override;

        static const char* hospitalName();
        static const char* wardId();
};

}
