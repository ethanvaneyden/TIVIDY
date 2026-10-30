#include "HospitalSeed.h"
#include "HospitalPolicies.h"
#include "ParticipantRegistry.h"

namespace hospital
{

void HospitalSeed::seedParticipants(ParticipantRegistry& registry)
{
    registry.add("Dr Naidoo", HospitalRoles::doctor());
    registry.add("Dr Khumalo", HospitalRoles::doctor());
    registry.add("Dr Pretorius", HospitalRoles::doctor());
    registry.add("Nurse Sithole", HospitalRoles::triageNurse());
    registry.add("Dr Mahlangu", HospitalRoles::admittingDoctor());
    registry.add("Prof Botha", HospitalRoles::chiefOfMedicine());
}

std::vector<DemoPatient> HospitalSeed::demoPatients()
{
    struct Row { const char* id; const char* name; const char* patientId;
                 const char* diagnosis; int severity; };
    static const Row rows[] = {
        { "TRT-1", "Karabo Sithole", "P-1001", "Sprained ankle",    2 },
        { "TRT-2", "Anele Zulu",     "P-1002", "Pneumonia",         5 },
        { "TRT-3", "Thabo Molefe",   "P-1003", "Cardiac arrhythmia", 9 }
    };

    std::vector<DemoPatient> result;
    for (unsigned i = 0; i < sizeof(rows) / sizeof(rows[0]); ++i)
    {
        DemoPatient p;
        p.itemId = rows[i].id;
        p.name = rows[i].name;
        p.patientId = rows[i].patientId;
        p.diagnosis = rows[i].diagnosis;
        p.severity = rows[i].severity;
        result.push_back(p);
    }
    return result;
}

DemoPatient HospitalSeed::invalidSeverityPatient()
{
    DemoPatient p;
    p.itemId = "TRT-X";
    p.name = "Lerato Nkosi";
    p.patientId = "P-1099";
    p.diagnosis = "Unassessed";
    p.severity = 0;
    return p;
}

}
