#ifndef EHR_ADAPTER_HPP
#define EHR_ADAPTER_HPP

#include "HospitalDomain.hpp"
#include <string>
#include <memory>
#include <utility>

namespace HospitalSystem {

class LegacyEHRSystem {
public:
    std::string fetchLegacyPatientRecord(const std::string& legacyId) {
        return "LEGACY_DATA|ID:" + legacyId + "|ALLERGIES:Penicillin|MEDS:Aspirin|DIAG:Chest Pain";
    }
};

class IEHRAdapter {
public:
    virtual ~IEHRAdapter() = default;
    virtual MedicalRecord getMedicalRecord(const std::string& legacyId) = 0;
};

class EHRAdapter : public IEHRAdapter {
private:
    std::shared_ptr<LegacyEHRSystem> legacyEHR_;

public:
    explicit EHRAdapter(std::shared_ptr<LegacyEHRSystem> legacyEHR)
        : legacyEHR_(std::move(legacyEHR)) {}

    MedicalRecord getMedicalRecord(const std::string& legacyId) override {
        std::string rawData = legacyEHR_->fetchLegacyPatientRecord(legacyId);
        MedicalRecord record;
        record.recordId = "REC-" + legacyId;
        record.patientId = legacyId;
        
        if (rawData.find("Penicillin") != std::string::npos) {
            record.allergies.push_back("Penicillin");
        }
        if (rawData.find("Aspirin") != std::string::npos) {
            record.currentMedications.push_back("Aspirin");
        }
        record.clinicalNotes = "Imported from Legacy EHR System";
        return record;
    }
};

} // namespace HospitalSystem

#endif // EHR_ADAPTER_HPP