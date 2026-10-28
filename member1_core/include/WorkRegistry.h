#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "Component.h"

class WorkRegistry
{
private:
    std::unordered_map<std::string, std::unique_ptr<Component>> components;

public:
    WorkRegistry();
    ~WorkRegistry();

    Component *add(std::unique_ptr<Component> item);
    Component *get(const std::string &id) const;

    /**
     * @brief Removes a component and its registered descendants.
     * @param id Component identifier.
     * @throws InvalidTransition If removing a descendant would violate its
     *         container's phase restrictions.
     */
    void remove(const std::string &id);
    bool contains(const std::string &id) const;
    size_t size() const;
    std::vector<std::string> getAllIds() const;
};