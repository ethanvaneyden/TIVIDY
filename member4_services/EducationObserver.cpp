#include "include/EducationObserver.h"

#include "include/SystemLogCommand.h"

EducationObserver::EducationObserver(ActionHistory* history) : history(history) {
}

void EducationObserver::update(const WorkflowEvent& event) {
    if (event.type == "ResultsPublished" && history != nullptr) {
        history->execute(
            std::unique_ptr<WorkCommand>(
                new SystemLogCommand(
                    "[System] Marks released for all students. Emails sent and LMS synced."
                )
            )
        );
    }
}