#include "HospitalAdmissionDirector.h"
#include "HospitalAdmissionBuilder.h"
#include "HospitalSeed.h"

#include <vector>

namespace hospital
{

HospitalAdmissionDirector::HospitalAdmissionDirector(HospitalAdmissionBuilder& workflowBuilder)
    : builder(workflowBuilder)
{
}

const char* HospitalAdmissionDirector::hospitalName()
{
    return "City Hospital";
}

const char* HospitalAdmissionDirector::wardId()
{
    return "W1";
}

void HospitalAdmissionDirector::buildEmptyWard()
{
    builder.buildRoot(hospitalName());
    builder.buildContainer(wardId(), "General Ward");
}

void HospitalAdmissionDirector::buildDemoWard()
{
    buildEmptyWard();
    const std::vector<DemoPatient> patients = HospitalSeed::demoPatients();
    for (std::vector<DemoPatient>::const_iterator p = patients.begin();
         p != patients.end(); ++p)
    {
        builder.buildTreatment(p->itemId, p->name, p->patientId, p->diagnosis);
    }
}

}
