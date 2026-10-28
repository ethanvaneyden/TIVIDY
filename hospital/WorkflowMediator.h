#ifndef WORKFLOW_MEDIATOR_HPP
#define WORKFLOW_MEDIATOR_HPP

#include "HospitalDomain.hpp"
#include "ApprovalChain.hpp"
#include <memory>
#include <string>

namespace HospitalSystem {

class IWorkflowMediator {
public:
    virtual ~IWorkflowMediator() = default;
    virtual void notify(const std::string& sender, const std::string& event, AdmissionRequest& request) = 0;
};

class HospitalWorkflowMediator : public IWorkflowMediator {
private:
    std::shared_ptr<IApprovalHandler> approvalChainHead_;

public:
    void setApprovalChain(std::shared_ptr<IApprovalHandler> chain) {
        approvalChainHead_ = chain;
    }

    void notify(const std::string& sender, const std::string& event, AdmissionRequest& request) override {
        if (event == "START_ADMISSION") {
            request.log("[Mediator] Initiating admission workflow from " + sender);
            if (approvalChainHead_) {
                bool result = approvalChainHead_->handleRequest(request);
                if (result) {
                    request.log("[Mediator] Admission workflow completed successfully.");
                } else {
                    request.log("[Mediator] Admission workflow halted due to failure.");
                }
            }
        }
    }
};

} // namespace HospitalSystem

#endif // WORKFLOW_MEDIATOR_HPP