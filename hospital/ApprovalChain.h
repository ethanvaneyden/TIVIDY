#ifndef APPROVAL_CHAIN_HPP
#define APPROVAL_CHAIN_HPP

#include "HospitalDomain.hpp"
#include "AdmissionPolicies.hpp"
#include <memory>
#include <string>
#include <cstdlib>

namespace HospitalSystem {

class IApprovalHandler {
protected:
    std::shared_ptr<IApprovalHandler> nextHandler_;

public:
    virtual ~IApprovalHandler() = default;

    void setNext(std::shared_ptr<IApprovalHandler> next) {
        nextHandler_ = next;
    }

    virtual bool handleRequest(AdmissionRequest& request) {
        if (nextHandler_) {
            return nextHandler_->handleRequest(request);
        }
        return true;
    }
};

class TriageAssessmentHandler : public IApprovalHandler {
public:
    bool handleRequest(AdmissionRequest& request) override {
        request.log("[Approval Chain] Triage assessment completed for Level: " +
                    std::to_string(static_cast<int>(request.patient.triageLevel)));
        request.status = AdmissionStatus::TRIAGED;
        return IApprovalHandler::handleRequest(request);
    }
};

class FinancialClearanceHandler : public IApprovalHandler {
private:
    InsuranceValidationPolicy policy_;

public:
    bool handleRequest(AdmissionRequest& request) override {
        std::string reason;
        if (!policy_.validate(request, reason)) {
            request.status = AdmissionStatus::REJECTED;
            request.log("[Approval Chain] Financial Clearance Failed: " + reason);
            return false;
        }
        request.patient.insuranceVerified = true;
        request.status = AdmissionStatus::INSURANCE_CHECKED;
        request.log("[Approval Chain] Financial clearance approved.");
        return IApprovalHandler::handleRequest(request);
    }
};

class PhysicianSignOffHandler : public IApprovalHandler {
public:
    bool handleRequest(AdmissionRequest& request) override {
        if (request.record.clinicalNotes.empty()) {
            request.status = AdmissionStatus::REJECTED;
            request.log("[Approval Chain] Physician Sign-Off Failed: Clinical notes missing.");
            return false;
        }
        request.status = AdmissionStatus::PHYSICIAN_APPROVED;
        request.log("[Approval Chain] Physician sign-off acquired.");
        return IApprovalHandler::handleRequest(request);
    }
};

class BedAllocationHandler : public IApprovalHandler {
private:
    BedCapacityPolicy policy_;

public:
    explicit BedAllocationHandler(int availableBeds) : policy_(availableBeds) {}

    bool handleRequest(AdmissionRequest& request) override {
        std::string reason;
        if (!policy_.validate(request, reason)) {
            request.status = AdmissionStatus::REJECTED;
            request.log("[Approval Chain] Bed Allocation Failed: " + reason);
            return false;
        }
        request.assignedBedId = "BED-ICU-" + std::to_string(rand() % 100 + 1);
        request.status = AdmissionStatus::ADMITTED;
        request.log("[Approval Chain] Bed successfully allocated: " + request.assignedBedId);
        return IApprovalHandler::handleRequest(request);
    }
};

} // namespace HospitalSystem

#endif // APPROVAL_CHAIN_HPP