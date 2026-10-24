#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "composite/Component.h"

class WorkRegistry
{
private:
    std::unordered_map<std::string, std::unique_ptr<Component>> items;

public:
    WorkRegistry();
    ~WorkRegistry();

    Component *add(std::unique_ptr<Component> item);
    Component *get(const std::string &id) const;
    void remove(const std::string &id);
    bool contains(const std::string &id) const;
    size_t size() const;
    std::vector<std::string> getAllIds() const;
};