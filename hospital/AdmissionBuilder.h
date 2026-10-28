#ifndef ADMISSION_BUILDER_HPP
#define ADMISSION_BUILDER_HPP

#include "HospitalDomain.hpp"
#include "EHRAdapter.hpp"
#include <string>
#include <cstdlib>

namespace HospitalSystem {

class AdmissionRequestBuilder {
private:
    AdmissionRequest request_;

public:
    AdmissionRequestBuilder() {
        request_.requestId = "REQ-" + std::to_string(rand() % 10000);
    }

    AdmissionRequestBuilder& setPatient(const std::string& id, const std::string& name, int age, TriageLevel level) {
        request_.patient.patientId = id;
        request_.patient.name = name;
        request_.patient.age = age;
        request_.patient.triageLevel = level;
        return *this;
    }

    AdmissionRequestBuilder& setDiagnosis(const std::string& diagnosis) {
        request_.patient.primaryDiagnosis = diagnosis;
        return *this;
    }

    AdmissionRequestBuilder& importLegacyMedicalRecord(IEHRAdapter& adapter, const std::string& legacyId) {
        request_.record = adapter.getMedicalRecord(legacyId);
        return *this;
    }

    AdmissionRequestBuilder& setDepartment(const std::string& department) {
        request_.assignedDepartment = department;
        return *this;
    }

    AdmissionRequest build() {
        return request_;
    }
};

} // namespace HospitalSystem

#endif // ADMISSION_BUILDER_HPP