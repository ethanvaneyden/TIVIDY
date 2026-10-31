#include "include/HealthcareObserver.h"
#include "include/SystemLogCommand.h"

HealthcareObserver::HealthcareObserver(ActionHistory* history) : history(history) {
}

void HealthcareObserver::update(const WorkflowEvent& event) {
    if (event.type == "WardPublished" && history != nullptr) {
        history->execute(
            std::unique_ptr<WorkCommand>(
                new SystemLogCommand(
                    "[System] Billing information generated and hospital records updated."
                )
            )
        );
    }
}