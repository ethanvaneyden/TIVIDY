#ifndef HOSPITAL_DOMAIN_HPP
#define HOSPITAL_DOMAIN_HPP

#include <string>
#include <vector>

namespace HospitalSystem {

enum class TriageLevel {
    NON_URGENT,
    SEMI_URGENT,
    URGENT,
    EMERGENCY,
    RESUSCITATION
};

enum class AdmissionStatus {
    DRAFT,
    TRIAGED,
    INSURANCE_CHECKED,
    PHYSICIAN_APPROVED,
    BED_ASSIGNED,
    ADMITTED,
    REJECTED
};

struct Patient {
    std::string patientId;
    std::string name;
    int age{0};
    TriageLevel triageLevel{TriageLevel::NON_URGENT};
    std::string primaryDiagnosis;
    std::string insuranceId;
    bool insuranceVerified{false};
};

struct MedicalRecord {
    std::string recordId;
    std::string patientId;
    std::vector<std::string> allergies;
    std::vector<std::string> currentMedications;
    std::string clinicalNotes;
};

struct AdmissionRequest {
    std::string requestId;
    Patient patient;
    MedicalRecord record;
    AdmissionStatus status{AdmissionStatus::DRAFT};
    std::string assignedBedId;
    std::string assignedDepartment;
    std::vector<std::string> auditTrail;

    void log(const std::string& message) {
        auditTrail.push_back(message);
    }
};

} // namespace HospitalSystem

#endif // HOSPITAL_DOMAIN_HPP