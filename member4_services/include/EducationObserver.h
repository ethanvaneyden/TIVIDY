#ifndef TIVIDY_EDUCATIONOBSERVER_H
#define TIVIDY_EDUCATIONOBSERVER_H

#include "Observer.h"
#include "ActionHistory.h"

class EducationObserver : public Observer {
private:
    ActionHistory* history;

public:
    explicit EducationObserver(ActionHistory* history);

    void update(const WorkflowEvent& event) override;
};

#endif