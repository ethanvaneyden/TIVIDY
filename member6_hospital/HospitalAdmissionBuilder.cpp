#include "HospitalAdmissionBuilder.h"
#include "Container.h"
#include "Root.h"
#include "Treatment.h"
#include "WorkRegistry.h"

#include <memory>
#include <stdexcept>
#include <utility>

namespace hospital
{

HospitalAdmissionBuilder::HospitalAdmissionBuilder()
    : registry(nullptr), root(nullptr), currentContainer(nullptr)
{
}

HospitalAdmissionBuilder::HospitalAdmissionBuilder(WorkRegistry& workRegistry)
    : registry(&workRegistry), root(nullptr), currentContainer(nullptr)
{
}

HospitalAdmissionBuilder::~HospitalAdmissionBuilder() {}

void HospitalAdmissionBuilder::setRegistry(WorkRegistry& workRegistry)
{
    registry = &workRegistry;
    root = nullptr;
    currentContainer = nullptr;
}

void HospitalAdmissionBuilder::buildRoot(const std::string& rootName)
{
    if (registry == nullptr)
    {
        throw std::logic_error("buildRoot called before setRegistry");
    }
    std::unique_ptr<Root> node(new Root(rootName, "hospital", rootName));
    Root* raw = node.get();
    registry->add(std::move(node));
    root = raw;
    currentContainer = nullptr;
}

void HospitalAdmissionBuilder::buildContainer(const std::string& containerId,
                                              const std::string& name)
{
    if (root == nullptr)
    {
        throw std::logic_error("buildContainer called before buildRoot");
    }
    std::unique_ptr<Container> node(new Container(containerId, "ward", name));
    Container* raw = node.get();
    registry->add(std::move(node));
    try
    {
        root->add(raw);
    }
    catch (...)
    {
        registry->remove(containerId);
        throw;
    }
    currentContainer = raw;
}

void HospitalAdmissionBuilder::buildItem(const std::string& itemId, const std::string& name)
{
    buildTreatment(itemId, name, itemId, std::string());
}

void HospitalAdmissionBuilder::buildTreatment(const std::string& itemId,
                                              const std::string& name,
                                              const std::string& patientId,
                                              const std::string& diagnosis)
{
    if (currentContainer == nullptr)
    {
        throw std::logic_error("buildItem called before buildContainer");
    }
    std::unique_ptr<Treatment> node(new Treatment(itemId, name, patientId, diagnosis));
    Treatment* raw = node.get();
    registry->add(std::move(node));
    try
    {
        currentContainer->add(raw);
    }
    catch (...)
    {
        registry->remove(itemId);
        throw;
    }
}

Root* HospitalAdmissionBuilder::getRoot() const
{
    return root;
}

Container* HospitalAdmissionBuilder::getCurrentContainer() const
{
    return currentContainer;
}

}
