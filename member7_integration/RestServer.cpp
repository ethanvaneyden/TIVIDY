/**
 * @file RestServer.cpp
 * @brief Implementation of the REST bridge.
 *
 * Every endpoint parses the request, makes ONE call on the TividyWorkflow
 * facade and serialises the answer. Both industries share the same table
 * driven route registration, so the API of one scenario can never drift
 * from the other.
 */

#include "RestServer.h"
#include "TividyWorkflow.h"
#include "JsonSerializer.h"
#include "Iterator.h"
#include "Item.h"
#include "Root.h"
#include "json.h"

#include <cctype>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include "httplib.h"

namespace
{
    using Json = nlohmann::json;

    int toHttpStatus(CommandResult result)
    {
        switch (result)
        {
        case CommandResult::Success:            return 200;
        case CommandResult::InvalidTransition:  return 409;
        case CommandResult::Rejected:           return 409;
        case CommandResult::IntegrationFailure: return 502;
        case CommandResult::NotFound:           return 404;
        }
        return 500;
    }

    std::string toResultString(CommandResult result)
    {
        switch (result)
        {
        case CommandResult::Success:            return "Success";
        case CommandResult::InvalidTransition:  return "InvalidTransition";
        case CommandResult::Rejected:           return "Rejected";
        case CommandResult::IntegrationFailure: return "IntegrationFailure";
        case CommandResult::NotFound:           return "NotFound";
        }
        return "Unknown";
    }

    /// {"result": "...", "message": "..."} with the matching HTTP status.
    void reply(httplib::Response& res, CommandResult result, const std::string& message)
    {
        Json body = {{"result", toResultString(result)}};
        if (!message.empty())
            body["message"] = message;
        res.status = toHttpStatus(result);
        res.set_content(body.dump(), "application/json");
    }

    /// Runs the facade call FIRST, then reads its message (argument evaluation
    /// order in C++ is unspecified, so this must not be one expression).
    void respond(httplib::Response& res, TividyWorkflow* wf, CommandResult result)
    {
        reply(res, result, wf->lastMessage());
    }

    void replyError(httplib::Response& res, int status, const std::string& error, const std::string& message)
    {
        res.status = status;
        res.set_content(JsonSerializer::serializeError(error, message), "application/json");
    }

    /// Maps "inprogress" / "InProgress" / "completed" ... to the engine's state names.
    std::string normalizeState(const std::string& raw)
    {
        std::string lower;
        for (std::string::size_type i = 0; i < raw.size(); ++i)
            lower += static_cast<char>(std::tolower(static_cast<unsigned char>(raw[i])));
        if (lower == "available")  return "Available";
        if (lower == "assigned")   return "Assigned";
        if (lower == "inprogress") return "InProgress";
        if (lower == "completed")  return "Completed";
        if (lower == "rejected")   return "Rejected";
        return std::string();
    }

    /// What differs between the two industries at the HTTP level.
    struct ScenarioRoutes
    {
        std::string scenario;        ///< "education" / "healthcare"
        std::string containerRoute;  ///< POST :id/<route> creates a container
        std::string itemRoute;       ///< POST :id/containers/:cid/<route> creates an item
        std::string nameField;       ///< JSON field holding the new item's name
        std::vector<std::string> textFields;     ///< extra string attributes accepted
        std::vector<std::string> numberFields;   ///< extra integer attributes accepted
    };
}

RestServer::RestServer()
    : facade(std::unique_ptr<TividyWorkflow>(new TividyWorkflow()))
{
}

RestServer::~RestServer() = default;

bool RestServer::run(const std::string &host, int port)
{
    httplib::Server svr;
    TividyWorkflow* wf = facade.get();
    std::mutex* mutex = &engineMutex;

    // CORS: allow the Vite dev server and any other origin during development.
    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        if (req.method == "OPTIONS") {
            res.status = 200;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });

    // Anything thrown by a handler (e.g. a malformed JSON body) becomes a clean 400/500.
    svr.set_exception_handler([](const httplib::Request&, httplib::Response& res, std::exception_ptr ep) {
        try {
            std::rethrow_exception(ep);
        } catch (const nlohmann::json::exception&) {
            replyError(res, 400, "InvalidJson", "The request body is not valid JSON");
        } catch (const std::exception& e) {
            replyError(res, 500, "ServerError", e.what());
        } catch (...) {
            replyError(res, 500, "ServerError", "Unknown error");
        }
    });

    svr.Get("/health", [](const httplib::Request &, httplib::Response &res)
            {
        res.status = 200;
        res.set_content("OK", "text/plain"); });

    std::vector<ScenarioRoutes> scenarios;
    {
        ScenarioRoutes education;
        education.scenario = "education";
        education.containerRoute = "submit";
        education.itemRoute = "submissions";
        education.nameField = "studentName";
        education.textFields.push_back("studentId");
        scenarios.push_back(education);

        ScenarioRoutes healthcare;
        healthcare.scenario = "healthcare";
        healthcare.containerRoute = "admit";
        healthcare.itemRoute = "patients";
        healthcare.nameField = "name";
        healthcare.textFields.push_back("diagnosis");
        healthcare.numberFields.push_back("age");
        scenarios.push_back(healthcare);
    }

    for (std::vector<ScenarioRoutes>::const_iterator it = scenarios.begin(); it != scenarios.end(); ++it)
    {
        const ScenarioRoutes cfg = *it;
        const std::string base = "/api/" + cfg.scenario;
        const std::string inst = base + "/:instanceId";

        // Returns false (and answers 404) if the instance is not in this scenario.
        auto owned = [wf, cfg](const httplib::Request& req, httplib::Response& res) -> bool {
            const std::string id = req.path_params.at("instanceId");
            if (!wf->ownsInstance(cfg.scenario, id)) {
                replyError(res, 404, "NotFound", "Instance not found: " + id);
                return false;
            }
            return true;
        };

        svr.Post(base + "/start", [=](const httplib::Request &req, httplib::Response &res)
        {
            const std::string name = JsonSerializer::getString(req.body, "name", "untitled");
            std::lock_guard<std::mutex> lock(*mutex);
            const std::string id = cfg.scenario == "education" ? wf->startEducation(name)
                                                                : wf->startHealthcare(name);
            if (id.empty()) {
                replyError(res, 500, "StartFailed", "The instance could not be created");
                return;
            }
            res.status = 200;
            res.set_content(Json{{"instanceId", id}}.dump(), "application/json");
        });

        svr.Get(base + "/instances", [=](const httplib::Request &, httplib::Response &res)
        {
            std::lock_guard<std::mutex> lock(*mutex);
            const std::vector<std::string> ids = cfg.scenario == "education"
                ? wf->listEducationInstances() : wf->listHealthcareInstances();
            res.status = 200;
            res.set_content(Json(ids).dump(), "application/json");
        });

        svr.Delete(inst, [=](const httplib::Request &req, httplib::Response &res)
        {
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            const CommandResult result = wf->removeInstance(req.path_params.at("instanceId"));
            reply(res, result, wf->lastMessage());
        });

        svr.Get(inst + "/state", [=](const httplib::Request& req, httplib::Response& res)
        {
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            Root* tree = wf->getTree(req.path_params.at("instanceId"));
            if (tree == nullptr) {
                replyError(res, 404, "NotFound", "instance");
                return;
            }

            // Filtered view: ?state=Completed ... uses the StateFilterIterator.
            if (req.has_param("state") && !req.get_param_value("state").empty())
            {
                const std::string state = normalizeState(req.get_param_value("state"));
                if (state.empty()) {
                    replyError(res, 400, "InvalidState", "Unknown state: " + req.get_param_value("state"));
                    return;
                }
                std::vector<const Item*> matched;
                std::unique_ptr<Iterator> iterator(tree->createIterator(state));
                for (iterator->first(); !iterator->isDone(); iterator->next())
                {
                    const Item* item = dynamic_cast<const Item*>(iterator->current());
                    if (item != nullptr)
                        matched.push_back(item);
                }
                res.status = 200;
                res.set_content(JsonSerializer::serializeItemList(matched), "application/json");
                return;
            }

            res.status = 200;
            res.set_content(JsonSerializer::serializeTree(tree), "application/json");
        });

        svr.Get(inst + "/history", [=](const httplib::Request &req, httplib::Response &res)
        {
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            res.status = 200;
            res.set_content(JsonSerializer::serializeHistory(
                wf->getHistory(req.path_params.at("instanceId"))), "application/json");
        });

        svr.Get(inst + "/progress", [=](const httplib::Request &req, httplib::Response &res)
        {
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            res.status = 200;
            res.set_content(JsonSerializer::serializeProgress(
                wf->getTree(req.path_params.at("instanceId"))), "application/json");
        });

        // Add a container (assignment / ward).
        svr.Post(inst + "/" + cfg.containerRoute, [=](const httplib::Request &req, httplib::Response &res)
        {
            const std::string name = JsonSerializer::getString(req.body, "containerName", "");
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            respond(res, wf, wf->addContainer(req.path_params.at("instanceId"), name));
        });

        svr.Post(inst + "/containers/:containerId/open", [=](const httplib::Request &req, httplib::Response &res)
        {
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            respond(res, wf, wf->openContainer(req.path_params.at("instanceId"),
                                         req.path_params.at("containerId")));
        });

        // Add an item (submission / patient).
        svr.Post(inst + "/containers/:containerId/" + cfg.itemRoute,
                 [=](const httplib::Request &req, httplib::Response &res)
        {
            const std::string name = JsonSerializer::getString(req.body, cfg.nameField, "");
            std::map<std::string, std::string> attributes;
            for (std::vector<std::string>::const_iterator f = cfg.textFields.begin();
                 f != cfg.textFields.end(); ++f)
            {
                const std::string value = JsonSerializer::getString(req.body, *f, "");
                if (!value.empty()) attributes[*f] = value;
            }
            for (std::vector<std::string>::const_iterator f = cfg.numberFields.begin();
                 f != cfg.numberFields.end(); ++f)
            {
                const int value = JsonSerializer::getInt(req.body, *f, -1);
                if (value >= 0) attributes[*f] = std::to_string(value);
            }
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            respond(res, wf, wf->addItem(req.path_params.at("instanceId"), req.path_params.at("containerId"),
                                   name, attributes));
        });

        svr.Post(inst + "/containers/:containerId/approve", [=](const httplib::Request &req, httplib::Response &res)
        {
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            respond(res, wf, wf->approveContainer(req.path_params.at("instanceId"),
                                            req.path_params.at("containerId")));
        });

        svr.Post(inst + "/items/:itemId/start", [=](const httplib::Request &req, httplib::Response &res)
        {
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            respond(res, wf, wf->startItem(req.path_params.at("instanceId"),
                                     req.path_params.at("itemId")));
        });

        svr.Post(inst + "/items/:itemId/complete", [=](const httplib::Request &req, httplib::Response &res)
        {
            const int value = JsonSerializer::getInt(req.body, "value", -1);
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            respond(res, wf, wf->completeItem(req.path_params.at("instanceId"),
                                        req.path_params.at("itemId"), value));
        });

        svr.Post(inst + "/items/:itemId/override", [=](const httplib::Request &req, httplib::Response &res)
        {
            const int value = JsonSerializer::getInt(req.body, "value", 0);
            const bool accept = JsonSerializer::getInt(req.body, "accept", 0) != 0;
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            respond(res, wf, wf->overrideItem(req.path_params.at("instanceId"),
                                        req.path_params.at("itemId"), value, accept));
        });

        svr.Get(inst + "/strategy", [=](const httplib::Request& req, httplib::Response& res)
        {
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            res.status = 200;
            res.set_content(Json{{"strategy", wf->getStrategyName(req.path_params.at("instanceId"))}}.dump(),
                            "application/json");
        });

        svr.Put(inst + "/strategy", [=](const httplib::Request& req, httplib::Response& res)
        {
            const std::string name = JsonSerializer::getString(req.body, "strategy", "");
            if (name.empty()) {
                replyError(res, 400, "InvalidArgument", "strategy is required");
                return;
            }
            std::lock_guard<std::mutex> lock(*mutex);
            if (!owned(req, res)) return;
            respond(res, wf, wf->setStrategy(req.path_params.at("instanceId"), name));
        });
    }

    return svr.listen(host, port);
}
