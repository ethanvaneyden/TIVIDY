/**
 * @file JsonTreeVisitor.cpp
 * @brief JSON rendering of Root / Container / Item.
 */
#include "JsonTreeVisitor.h"

#include "Container.h"
#include "Item.h"
#include "Participant.h"
#include "Root.h"

#include "json.h"

#include <cctype>
#include <map>
#include <string>
#include <utility>

namespace
{
using Json = nlohmann::json;

bool isWholeNumber(const std::string& text)
{
    if (text.empty() || text.size() > 9)
        return false;
    for (std::string::size_type i = 0; i < text.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(text[i])))
            return false;
    }
    return true;
}

Json participantName(const Participant* p)
{
    return p != nullptr ? Json(p->getName()) : Json(nullptr);
}

Json participantId(const Participant* p)
{
    return p != nullptr ? Json(p->getId()) : Json(nullptr);
}
}

struct JsonTreeVisitor::Impl
{
    Json current;

    Json renderChildren(const Component& parent)
    {
        Json children = Json::array();
        const std::vector<Component*> kids = parent.getChildren();
        for (std::vector<Component*>::const_iterator kid = kids.begin(); kid != kids.end(); ++kid)
        {
            (*kid)->accept(*owner);
            children.push_back(std::move(current));
        }
        return children;
    }

    JsonTreeVisitor* owner = nullptr;
};

JsonTreeVisitor::JsonTreeVisitor() : impl(new Impl())
{
    impl->owner = this;
    impl->current = nullptr;
}

JsonTreeVisitor::~JsonTreeVisitor() = default;

void JsonTreeVisitor::visitRoot(const Root& root)
{
    Json children = impl->renderChildren(root);
    impl->current = Json{
        {"id", root.getId()},
        {"type", root.getType()},
        {"name", root.getName()},
        {"children", std::move(children)}};
}

void JsonTreeVisitor::visitContainer(const Container& container)
{
    Json children = impl->renderChildren(container);
    impl->current = Json{
        {"id", container.getId()},
        {"type", container.getType()},
        {"name", container.getName()},
        {"phase", container.getPhaseName()},
        {"ready", container.isReadyToPublish()},
        {"children", std::move(children)}};
}

void JsonTreeVisitor::visitItem(const Item& item)
{
    Json value = {
        {"id", item.getId()},
        {"type", "Item"},
        {"kind", item.getType()},
        {"name", item.getName()},
        {"state", item.getStateName()},
        {"reviewMethod", item.getReviewMethod()},
        {"assignee", participantName(item.getAssignee())},
        {"assigneeId", participantId(item.getAssignee())},
        {"reviewer", participantName(item.getReviewer())},
        {"reviewerId", participantId(item.getReviewer())}};

    if (item.getAssignee() != nullptr)
        value["assigneeRole"] = item.getAssignee()->getRole();
    if (item.getReviewer() != nullptr)
        value["reviewerRole"] = item.getReviewer()->getRole();
    if (item.hasResult())
        value["result"] = item.getResult();
    if (!item.getRejectionReason().empty())
        value["rejectionReason"] = item.getRejectionReason();

    const std::map<std::string, std::string> attributes = item.getAttributes();
    for (std::map<std::string, std::string>::const_iterator a = attributes.begin();
         a != attributes.end(); ++a)
    {
        if (value.find(a->first) != value.end())
            continue; // never let an attribute shadow a core field
        if (a->first == "age" && isWholeNumber(a->second))
            value[a->first] = std::stoi(a->second);
        else
            value[a->first] = a->second;
    }

    impl->current = std::move(value);
}

std::string JsonTreeVisitor::result() const
{
    return impl->current.dump();
}
