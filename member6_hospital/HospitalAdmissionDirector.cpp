#include "HospitalAdmissionDirector.h"
#include "HospitalAdmissionBuilder.h"
#include "HospitalSeed.h"

#include <map>
#include <stdexcept>
#include <vector>

namespace hospital
{

HospitalAdmissionDirector::HospitalAdmissionDirector(WorkflowBuilder& workflowBuilder)
    : builder(workflowBuilder)
{
}

HospitalAdmissionDirector::~HospitalAdmissionDirector() {}

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

void HospitalAdmissionDirector::buildDemo(WorkflowBuilder& target, const std::string& rootName)
{
    target.buildRoot(rootName);
    target.buildContainer(wardId(), "General Ward");

    const std::vector<DemoPatient> patients = HospitalSeed::demoPatients();
    for (std::vector<DemoPatient>::const_iterator p = patients.begin();
         p != patients.end(); ++p)
    {
        std::map<std::string, std::string> attributes;
        attributes["patientId"] = p->patientId;
        attributes["diagnosis"] = p->diagnosis;
        attributes["age"] = std::to_string(p->age);
        target.buildItem(p->itemId, p->name, attributes);
    }
}

void HospitalAdmissionDirector::buildDemoWard()
{
    buildDemo(builder, hospitalName());
}

void HospitalAdmissionDirector::buildDemoWard(const std::string& name)
{
    buildDemo(builder, name.empty() ? std::string(hospitalName()) : name);
}

Root* HospitalAdmissionDirector::buildDemoTree(WorkflowBuilder& target, const std::string& rootName)
{
    buildDemo(target, rootName);
    return target.getRoot();
}

Root* HospitalAdmissionDirector::buildTree(
    WorkflowBuilder& target,
    const std::string& rootName,
    const std::vector<std::pair<std::string, std::string>>& containers,
    const std::vector<std::tuple<std::string, std::string, std::string>>& items)
{
    // Check first, so a bad specification builds nothing.
    for (std::vector<std::tuple<std::string, std::string, std::string>>::const_iterator i = items.begin();
         i != items.end(); ++i)
    {
        bool known = false;
        for (std::vector<std::pair<std::string, std::string>>::const_iterator c = containers.begin();
             c != containers.end(); ++c)
        {
            if (c->first == std::get<0>(*i))
            {
                known = true;
                break;
            }
        }
        if (!known)
        {
            throw std::invalid_argument("Item " + std::get<1>(*i) +
                                        " names an unknown container: " + std::get<0>(*i));
        }
    }

    target.buildRoot(rootName);
    for (std::vector<std::pair<std::string, std::string>>::const_iterator c = containers.begin();
         c != containers.end(); ++c)
    {
        target.buildContainer(c->first, c->second);
        for (std::vector<std::tuple<std::string, std::string, std::string>>::const_iterator i = items.begin();
             i != items.end(); ++i)
        {
            if (std::get<0>(*i) == c->first)
            {
                target.buildItem(std::get<1>(*i), std::get<2>(*i));
            }
        }
    }
    return target.getRoot();
}

}
