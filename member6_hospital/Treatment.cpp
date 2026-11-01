#include "Treatment.h"

namespace hospital
{

const int Treatment::NO_SEVERITY;

Treatment::Treatment(const std::string& id,
                     const std::string& patientName,
                     const std::string& patientIdentifier,
                     const std::string& diagnosisText)
    : Item(id, "treatment", patientName),
      patientId(patientIdentifier),
      diagnosis(diagnosisText),
      severity(NO_SEVERITY)
{
}

Treatment::~Treatment() {}

const std::string& Treatment::getPatientName() const
{
    return getName();
}

const std::string& Treatment::getPatientId() const
{
    return patientId;
}

const std::string& Treatment::getDiagnosis() const
{
    return diagnosis;
}

void Treatment::setDiagnosis(const std::string& value)
{
    diagnosis = value;
}

bool Treatment::hasSeverity() const
{
    return severity != NO_SEVERITY;
}

int Treatment::getSeverity() const
{
    return severity;
}

void Treatment::setSeverity(int value)
{
    severity = value;
}

}
