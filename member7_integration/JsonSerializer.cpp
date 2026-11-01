/**
 * @file JsonSerializer.cpp
 * @brief Implementation of JsonSerializer using nlohmann/json.
 */

#include "JsonSerializer.h"
#include "Component.h"
#include "Item.h"
#include "Participant.h"

#include "json.h"
#include <limits>
#include <utility>

namespace
{
    using Json = nlohmann::json;

    Json serializeItemValue(const Item* item)
    {
        if(item == nullptr)
            return nullptr;

        Json value = {
            {"id", item->getId()},
            {"type", "Item"},
            {"name", item->getName()},
            {"state", item->getStateName()},
            {"reviewMethod", item->getReviewMethod()}
        };

        const Participant* assignee = item->getAssignee();
        value["assignee"] = assignee != nullptr
            ? Json(assignee->getId())
            : Json(nullptr);

        const Participant* reviewer = item->getReviewer();
        value["reviewer"] = reviewer != nullptr
            ? Json(reviewer->getId())
            : Json(nullptr);

        return value;
    }

    Json serializeComponentValue(const Component* component)
    {
        if(component == nullptr)
            return nullptr;

        const Item* item = dynamic_cast<const Item*>(component);
        if(item != nullptr)
            return serializeItemValue(item);

        Json children = Json::array();
        for (const Component* child : component->getChildren())
            children.push_back(serializeComponentValue(child));

        return Json{
            {"id", component->getId()},
            {"type", component->getType()},
            {"name", component->getName()},
            {"children", std::move(children)}
        };
    }

    Json parseObject(const std::string& input)
    {
        Json value = Json::parse(input);
        return value.is_object() ? value : Json::object();
    }
}

std::string JsonSerializer::serializeItem(const Item* item)
{
    return serializeItemValue(item).dump();
}

std::string JsonSerializer::serializeTree(const Component* root)
{
    return serializeComponentValue(root).dump();
}

std::string JsonSerializer::serializeParticipant(const Participant* participant)
{
    if(participant == nullptr)
        return "null";

    return nlohmann::json{
        {"id", participant->getId()},
        {"name", participant->getName()},
        {"role", participant->getRole()}
    }.dump();
}

std::string JsonSerializer::serializeHistory(
    const std::vector<std::string>& records)
{
    return nlohmann::json(records).dump();
}

std::string JsonSerializer::serializeError(
    const std::string& error,
    const std::string& message)
{
    return nlohmann::json{
        {"error", error},
        {"message", message}
    }.dump();
}

std::string JsonSerializer::getString(
    const std::string& input,
    const std::string& field,
    const std::string& fallback)
{
    const nlohmann::json object = parseObject(input);
    const nlohmann::json::const_iterator value = object.find(field);
    if(value == object.end() || !value->is_string())
        return fallback;

    return value->get<std::string>();
}

int JsonSerializer::getInt(
    const std::string& input,
    const std::string& field,
    int fallback)
{
    const nlohmann::json object = parseObject(input);
    const nlohmann::json::const_iterator value = object.find(field);
    if(value == object.end() || !value->is_number_integer())
        return fallback;

    if(value->is_number_unsigned())
    {
        const unsigned long long number = value->get<unsigned long long>();
        if(number > static_cast<unsigned long long>(
                         std::numeric_limits<int>::max()))
            return fallback;
        return static_cast<int>(number);
    }

    const long long number = value->get<long long>();
    if(number < std::numeric_limits<int>::min() ||
        number > std::numeric_limits<int>::max())
        return fallback;
    return static_cast<int>(number);
}
