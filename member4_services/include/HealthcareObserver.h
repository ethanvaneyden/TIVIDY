#ifndef TIVIDY_HEALTHCAREOBSERVER_H
#define TIVIDY_HEALTHCAREOBSERVER_H

#include "Observer.h"
#include "ActionHistory.h"

class HealthcareObserver : public Observer {
private:
    ActionHistory* history;

public:
    explicit HealthcareObserver(ActionHistory* history);
    void update(const WorkflowEvent& event) override;
};

#endif