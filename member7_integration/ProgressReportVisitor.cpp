/**
 * @file ProgressReportVisitor.cpp
 */
#include "ProgressReportVisitor.h"

#include "Container.h"
#include "Item.h"
#include "Root.h"

#include "json.h"

const std::size_t ProgressReportVisitor::NONE = static_cast<std::size_t>(-1);

ProgressReportVisitor::ProgressReportVisitor() : total(0), completed(0), activeContainer(NONE) {}

void ProgressReportVisitor::visitRoot(const Root& root)
{
    const std::vector<Component*> kids = root.getChildren();
    for (std::vector<Component*>::const_iterator kid = kids.begin(); kid != kids.end(); ++kid)
        (*kid)->accept(*this);
}

void ProgressReportVisitor::visitContainer(const Container& container)
{
    ContainerProgress progress;
    progress.id = container.getId();
    progress.name = container.getName();
    progress.phase = container.getPhaseName();
    perContainer.push_back(progress);

    // visitItem() adds each child's figures to the container being visited.
    const std::size_t index = perContainer.size() - 1;
    const std::size_t previous = activeContainer;
    activeContainer = index;

    const std::vector<Component*> kids = container.getChildren();
    for (std::vector<Component*>::const_iterator kid = kids.begin(); kid != kids.end(); ++kid)
        (*kid)->accept(*this);

    activeContainer = previous;
    ContainerProgress& done = perContainer[index];
    done.percentComplete = done.total == 0 ? 0 : (done.completed * 100) / done.total;
}

void ProgressReportVisitor::visitItem(const Item& item)
{
    ++total;
    ++states[item.getStateName()];
    if (item.isCompleted())
        ++completed;

    if (activeContainer != NONE)
    {
        ContainerProgress& owner = perContainer[activeContainer];
        ++owner.total;
        if (item.isCompleted())
            ++owner.completed;
        if (item.isReviewed())
            ++owner.reviewed;
        if (item.isRejected())
            ++owner.rejected;
    }
}

const std::map<std::string, int>& ProgressReportVisitor::stateCounts() const { return states; }
const std::vector<ContainerProgress>& ProgressReportVisitor::containers() const { return perContainer; }
int ProgressReportVisitor::totalItems() const { return total; }
int ProgressReportVisitor::completedItems() const { return completed; }

int ProgressReportVisitor::percentComplete() const
{
    return total == 0 ? 0 : (completed * 100) / total;
}

std::string ProgressReportVisitor::toJson() const
{
    using Json = nlohmann::json;

    Json stateJson = Json::object();
    for (std::map<std::string, int>::const_iterator s = states.begin(); s != states.end(); ++s)
        stateJson[s->first] = s->second;

    Json containerJson = Json::array();
    for (std::vector<ContainerProgress>::const_iterator c = perContainer.begin();
         c != perContainer.end(); ++c)
    {
        containerJson.push_back(Json{
            {"id", c->id},
            {"name", c->name},
            {"phase", c->phase},
            {"total", c->total},
            {"completed", c->completed},
            {"reviewed", c->reviewed},
            {"rejected", c->rejected},
            {"percentComplete", c->percentComplete}});
    }

    return Json{
        {"totalItems", total},
        {"completedItems", completed},
        {"percentComplete", percentComplete()},
        {"states", stateJson},
        {"containers", containerJson}}.dump();
}
