#ifndef ADMISSION_POLICIES_HPP
#define ADMISSION_POLICIES_HPP

#include "HospitalDomain.hpp"
#include <string>

namespace HospitalSystem {

class IAdmissionPolicy {
public:
    virtual ~IAdmissionPolicy() = default;
    virtual bool validate(const AdmissionRequest& request, std::string& outReason) const = 0;
};

class InsuranceValidationPolicy : public IAdmissionPolicy {
public:
    bool validate(const AdmissionRequest& request, std::string& outReason) const override {
        if (request.patient.triageLevel == TriageLevel::EMERGENCY ||
            request.patient.triageLevel == TriageLevel::RESUSCITATION) {
            return true; // Emergency bypass policy
        }
        if (!request.patient.insuranceVerified) {
            outReason = "Insurance validation required prior to elective/urgent admission.";
            return false;
        }
        return true;
    }
};

class BedCapacityPolicy : public IAdmissionPolicy {
private:
    int availableBeds_;

public:
    explicit BedCapacityPolicy(int availableBeds) : availableBeds_(availableBeds) {}

    bool validate(const AdmissionRequest& request, std::string& outReason) const override {
        if (availableBeds_ <= 0) {
            outReason = "No beds available in the requested department.";
            return false;
        }
        return true;
    }
};

} // namespace HospitalSystem

#endif // ADMISSION_POLICIES_HPP