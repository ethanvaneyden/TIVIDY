/**
 * @file JsonSerializer.cpp
 * @brief Implementation of JsonSerializer.
*/

#include "JsonSerializer.h"
#include "Container.h"
#include "Item.h"
#include "Participant.h"

#include <sstream>

std::string JsonSerializer::escape(const std::string& s)
{
    std::string out;
    out.reserve(s.size());
    for(char c : s)
    {
        switch(c)
        {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c; break;
        }
    }
    return out;
}

std::string JsonSerializer::serializeItem(const Item* item)
{
    if(item == nullptr)
    {
        return "null";
    }
    std::ostringstream os;
    os << "{";
    os << "\"id\":\"" << escape(item->getId()) << "\",";
    os << "\"type\":\"Item\",";
    os << "\"name\":\"" << escape(item->getName()) << "\",";
    os << "\"state\":\"" << escape(item->getStateName()) << "\",";
    os << "\"assignee\":";
    auto* assignee = item->getAssignee();
    if(assignee != nullptr)
    {
        os << "\"" << escape(assignee->getId()) << "\"";
    }
    else
    {
        os << "null";
    }
    os << ",\"reviewer\":";
    auto* reviewer = item->getReviewer();
    if(reviewer != nullptr)
    {
        os << "\"" << escape(reviewer->getId()) << "\"";
    }
    else
    {
        os << "null";
    }
    os << ",\"reviewMethod\":\"" << escape(item->getReviewMethod()) << "\"";
    os << "}";
    return os.str();
}

void JsonSerializer::serializeComponent(const Component* c, std::string& out)
{
    if(c == nullptr)
    {
        out += "null";
        return;
    }
    const auto* item = dynamic_cast<const Item*>(c);
    if(item != nullptr)
    {
        out += serializeItem(item);
        return;
    }
    /* Container: recurse.*/
    out += "{";
    out += "\"id\":\"" + escape(c->getId()) + "\",";
    out += "\"type\":\"" + escape(c->getType()) + "\",";
    out += "\"name\":\"" + escape(c->getName()) + "\",";
    out += "\"children\":[";
    auto children = c->getChildren();
    for(size_t i = 0; i < children.size(); ++i)
    {
        if(i != 0)
        {
            out += ",";
        }
        serializeComponent(children[i], out);
    }
    out += "]}";
}

std::string JsonSerializer::serializeTree(const Component* root)
{
    if(root == nullptr)
    {
        return "null";
    }
    std::string out;
    serializeComponent(root, out);
    return out;
}

std::string JsonSerializer::serializeParticipant(const Participant* p)
{
    if(p == nullptr)
    {
        return "null";
    }
    std::ostringstream os;
    os << "{";
    os << "\"id\":\"" << escape(p->getId()) << "\",";
    os << "\"name\":\"" << escape(p->getName()) << "\",";
    os << "\"role\":\"" << escape(p->getRole()) << "\"";
    os << "}";
    return os.str();
}

std::string JsonSerializer::serializeHistory(const std::vector<std::string>& records)
{
    std::ostringstream os;
    os << "[";
    for(size_t i = 0; i < records.size(); ++i)
    {
        if(i != 0)
        {
            os << ",";
        }
        os << "\"" << escape(records[i]) << "\"";
    }
    os << "]";
    return os.str();
}

std::string JsonSerializer::serializeError(const std::string& error,
                                           const std::string& message)
{
    std::ostringstream os;
    os << "{\"error\":\"" << escape(error) << "\",";
    os << "\"message\":\"" << escape(message) << "\"}";
    return os.str();
}

std::string JsonSerializer::getString(const std::string& json,
                                      const std::string& field,
                                      const std::string& fallback)
{
    std::string needle = "\"" + field + "\"";
    auto pos = json.find(needle);
    if(pos == std::string::npos)
    {
        return fallback;
    }
    auto colon = json.find(':', pos + needle.size());
    if(colon == std::string::npos)
    {
        return fallback;
    }
    auto firstQuote = json.find('"', colon + 1);
    if(firstQuote == std::string::npos)
    {
        return fallback;
    }
    auto secondQuote = json.find('"', firstQuote + 1);
    if(secondQuote == std::string::npos)
    {
        return fallback;
    }
    return json.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

int JsonSerializer::getInt(const std::string& json,
                           const std::string& field,
                           int fallback)
{
    std::string needle = "\"" + field + "\"";
    auto pos = json.find(needle);
    if(pos == std::string::npos)
    {
        return fallback;
    }
    auto colon = json.find(':', pos + needle.size());
    if(colon == std::string::npos)
    {
        return fallback;
    }
    auto start = json.find_first_of("-0123456789", colon + 1);
    if(start == std::string::npos)
    {
        return fallback;
    }
    auto end = json.find_first_not_of("0123456789", start + (json[start] == '-' ? 1 : 0));
    if(end == std::string::npos)
    {
        end = json.size();
    }
    return std::stoi(json.substr(start, end - start));
}