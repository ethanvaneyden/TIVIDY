/**
 * @file RestServer.cpp
 * @brief Implementation of the REST bridge.
 */

#include "RestServer.h"
#include "TividyWorkflow.h"
#include "JsonSerializer.h"
#include "Container.h"
#include "Root.h"
#include "StateFilterIterator.h" 
#include "Item.h"

#include <cctype>
#include "httplib.h"

namespace
{
    int toHttpStatus(CommandResult result)
    {
        switch (result)
        {
        case CommandResult::Success:
            return 200;
        case CommandResult::InvalidTransition:
            return 409;
        case CommandResult::Rejected:
            return 409;
        case CommandResult::IntegrationFailure:
            return 502;
        case CommandResult::NotFound:
            return 404;
        }
        return 500;
    }

    std::string toResultString(CommandResult result)
    {
        switch (result)
        {
        case CommandResult::Success:
            return "Success";
        case CommandResult::InvalidTransition:
            return "InvalidTransition";
        case CommandResult::Rejected:
            return "Rejected";
        case CommandResult::IntegrationFailure:
            return "IntegrationFailure";
        case CommandResult::NotFound:
            return "NotFound";
        }
        return "Unknown";
    }

    std::string resultJson(CommandResult result)
    {
        return std::string("{\"result\":\"") + toResultString(result) + "\"}";
    }
}

RestServer::RestServer()
    : facade(std::unique_ptr<TividyWorkflow>(new TividyWorkflow()))
{
}

bool RestServer::run(const std::string &host, int port)
{
    httplib::Server svr;

    svr.set_default_headers({{"Access-Control-Allow-Origin", "*"},
                             {"Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS"},
                             {"Access-Control-Allow-Headers", "Content-Type"}});

    svr.Options(R"(.*)", [](const httplib::Request &, httplib::Response &res)
                { res.status = 200; });

    svr.Get("/health", [](const httplib::Request &, httplib::Response &res)
            {
        res.status = 200;
        res.set_content("OK", "text/plain"); });

    /* ---------------- Education ----------------*/

    svr.Post("/api/education/start",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 std::string name = JsonSerializer::getString(req.body, "name", "untitled");
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto id = facade->startEducation(name);
                 res.status = 200;
                 res.set_content("{\"instanceId\":\"" + id + "\"}", "application/json");
             });

    svr.Get("/api/education/instances",
            [this](const httplib::Request &, httplib::Response &res)
            {
                std::lock_guard<std::mutex> lock(engineMutex);
                auto ids = facade->listEducationInstances();
                std::string out = "[";
                for (size_t i = 0; i < ids.size(); ++i)
                {
                    if (i != 0)
                        out += ",";
                    out += "\"" + ids[i] + "\"";
                }
                out += "]";
                res.status = 200;
                res.set_content(out, "application/json");
            });

    // svr.Get("/api/education/:instanceId/state",
    //         [this](const httplib::Request &req, httplib::Response &res)
    //         {
    //             auto id = req.path_params.at("instanceId");
    //             std::lock_guard<std::mutex> lock(engineMutex);
    //             Root *tree = facade->getTree(id);
    //             if (tree == nullptr)
    //             {
    //                 res.status = 404;
    //                 res.set_content(JsonSerializer::serializeError("NotFound", "instance"),
    //                                 "application/json");
    //                 return;
    //             }
    //             res.status = 200;
    //             res.set_content(JsonSerializer::serializeTree(tree), "application/json");
    //         });

    svr.Get("/api/education/:instanceId/state",
            [this](const httplib::Request& req, httplib::Response& res)
            {
                auto id = req.path_params.at("instanceId");
                std::lock_guard<std::mutex> lock(engineMutex);
                Root* tree = facade->getTree(id);
                if (tree == nullptr)
                {
                    res.status = 404;
                    res.set_content(
                        JsonSerializer::serializeError("NotFound", "instance"),
                        "application/json");
                    return;
                }

                // Filtered view: ?state=Completed etc. Uses StateFilterIterator.
                const auto stateFilter = req.get_param_value("state");
                if (!stateFilter.empty())
                {
                    // Case-insensitive normalization: the iterator compares exact names.
                    std::string normalized = stateFilter;
                    normalized[0] = static_cast<char>(
                        std::toupper(static_cast<unsigned char>(normalized[0])));

                    std::vector<const Item*> matched;
                    StateFilterIterator it(tree, normalized);
                    for (it.first(); !it.isDone(); it.next())
                    {
                        const Item* item = dynamic_cast<const Item*>(it.current());
                        if (item != nullptr)
                        {
                            matched.push_back(item);
                        }
                    }

                    res.status = 200;
                    res.set_content(JsonSerializer::serializeItemList(matched), "application/json");
                    return;
                }

                // Unfiltered: returns the whole
                res.status = 200;
                res.set_content(JsonSerializer::serializeTree(tree), "application/json");
            });

    svr.Get("/api/education/:instanceId/history",
            [this](const httplib::Request &req, httplib::Response &res)
            {
                auto id = req.path_params.at("instanceId");
                std::lock_guard<std::mutex> lock(engineMutex);
                auto hist = facade->getHistory(id);
                res.status = 200;
                res.set_content(JsonSerializer::serializeHistory(hist), "application/json");
            });

    svr.Post("/api/education/:instanceId/submit",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 auto id = req.path_params.at("instanceId");
                 auto container = JsonSerializer::getString(req.body, "containerName", "submission");
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto result = facade->submit(id, container);
                 res.status = toHttpStatus(result);
                 res.set_content(resultJson(result), "application/json");
             });

    svr.Post("/api/education/:instanceId/items/:itemId/start",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 auto instanceId = req.path_params.at("instanceId");
                 auto itemId = req.path_params.at("itemId");
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto result = facade->startItem(instanceId, itemId);
                 res.status = toHttpStatus(result);
                 res.set_content(resultJson(result), "application/json");
             });

    svr.Post("/api/education/:instanceId/items/:itemId/complete",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 auto instanceId = req.path_params.at("instanceId");
                 auto itemId = req.path_params.at("itemId");
                 int value = JsonSerializer::getInt(req.body, "value", 0);
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto result = facade->completeItem(instanceId, itemId, value);
                 res.status = toHttpStatus(result);
                 res.set_content(resultJson(result), "application/json");
             });

    svr.Post("/api/education/:instanceId/containers/:containerId/approve",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 auto instanceId = req.path_params.at("instanceId");
                 auto containerId = req.path_params.at("containerId");
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto result = facade->approveContainer(instanceId, containerId);
                 res.status = toHttpStatus(result);
                 res.set_content(resultJson(result), "application/json");
             });

    svr.Post("/api/education/:instanceId/items/:itemId/override",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 auto instanceId = req.path_params.at("instanceId");
                 auto itemId = req.path_params.at("itemId");
                 int value = JsonSerializer::getInt(req.body, "value", 0);
                 bool accept = JsonSerializer::getInt(req.body, "accept", 0) != 0;
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto result = facade->overrideItem(instanceId, itemId, value, accept);
                 res.status = toHttpStatus(result);
                 res.set_content(resultJson(result), "application/json");
             });

    /* ---------------- Healthcare ----------------*/

    svr.Post("/api/healthcare/start",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 std::string name = JsonSerializer::getString(req.body, "name", "untitled");
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto id = facade->startHealthcare(name);
                 res.status = 200;
                 res.set_content("{\"instanceId\":\"" + id + "\"}", "application/json");
             });

    svr.Get("/api/healthcare/instances",
            [this](const httplib::Request &, httplib::Response &res)
            {
                std::lock_guard<std::mutex> lock(engineMutex);
                auto ids = facade->listHealthcareInstances();
                std::string out = "[";
                for (size_t i = 0; i < ids.size(); ++i)
                {
                    if (i != 0)
                        out += ",";
                    out += "\"" + ids[i] + "\"";
                }
                out += "]";
                res.status = 200;
                res.set_content(out, "application/json");
            });

    // svr.Get("/api/healthcare/:instanceId/state",
    //         [this](const httplib::Request &req, httplib::Response &res)
    //         {
    //             auto id = req.path_params.at("instanceId");
    //             std::lock_guard<std::mutex> lock(engineMutex);
    //             Root *tree = facade->getTree(id);
    //             if (tree == nullptr)
    //             {
    //                 res.status = 404;
    //                 res.set_content(JsonSerializer::serializeError("NotFound", "instance"),
    //                                 "application/json");
    //                 return;
    //             }
    //             res.status = 200;
    //             res.set_content(JsonSerializer::serializeTree(tree), "application/json");
    //         });

    svr.Get("/api/healthcare/:instanceId/state",
            [this](const httplib::Request& req, httplib::Response& res)
            {
                auto id = req.path_params.at("instanceId");
                std::lock_guard<std::mutex> lock(engineMutex);
                Root* tree = facade->getTree(id);
                if (tree == nullptr)
                {
                    res.status = 404;
                    res.set_content(
                        JsonSerializer::serializeError("NotFound", "instance"),
                        "application/json");
                    return;
                }

                // Filtered view: ?state=Completed etc. Uses StateFilterIterator.
                const auto stateFilter = req.get_param_value("state");
                if (!stateFilter.empty())
                {
                    // Case-insensitive normalization: the iterator compares exact names.
                    std::string normalized = stateFilter;
                    normalized[0] = static_cast<char>(
                        std::toupper(static_cast<unsigned char>(normalized[0])));

                    std::vector<const Item*> matched;
                    StateFilterIterator it(tree, normalized);
                    for (it.first(); !it.isDone(); it.next())
                    {
                        const Item* item = dynamic_cast<const Item*>(it.current());
                        if (item != nullptr)
                        {
                            matched.push_back(item);
                        }
                    }

                    res.status = 200;
                    res.set_content(JsonSerializer::serializeItemList(matched), "application/json");
                    return;
                }

                // Unfiltered: returns the whole tree as before.
                res.status = 200;
                res.set_content(JsonSerializer::serializeTree(tree), "application/json");
            });

    svr.Get("/api/healthcare/:instanceId/history",
            [this](const httplib::Request &req, httplib::Response &res)
            {
                auto id = req.path_params.at("instanceId");
                std::lock_guard<std::mutex> lock(engineMutex);
                auto hist = facade->getHistory(id);
                res.status = 200;
                res.set_content(JsonSerializer::serializeHistory(hist), "application/json");
            });

    svr.Post("/api/healthcare/:instanceId/admit",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 auto id = req.path_params.at("instanceId");
                 auto container = JsonSerializer::getString(req.body, "containerName", "patient");
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto result = facade->admitPatient(id, container);
                 res.status = toHttpStatus(result);
                 res.set_content(resultJson(result), "application/json");
             });

    svr.Post("/api/healthcare/:instanceId/items/:itemId/start",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 auto instanceId = req.path_params.at("instanceId");
                 auto itemId = req.path_params.at("itemId");
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto result = facade->startItem(instanceId, itemId);
                 res.status = toHttpStatus(result);
                 res.set_content(resultJson(result), "application/json");
             });

    svr.Post("/api/healthcare/:instanceId/items/:itemId/complete",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 auto instanceId = req.path_params.at("instanceId");
                 auto itemId = req.path_params.at("itemId");
                 int value = JsonSerializer::getInt(req.body, "value", 0);
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto result = facade->completeItem(instanceId, itemId, value);
                 res.status = toHttpStatus(result);
                 res.set_content(resultJson(result), "application/json");
             });

    svr.Post("/api/healthcare/:instanceId/containers/:containerId/approve",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 auto instanceId = req.path_params.at("instanceId");
                 auto containerId = req.path_params.at("containerId");
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto result = facade->approveContainer(instanceId, containerId);
                 res.status = toHttpStatus(result);
                 res.set_content(resultJson(result), "application/json");
             });

    svr.Post("/api/healthcare/:instanceId/items/:itemId/override",
             [this](const httplib::Request &req, httplib::Response &res)
             {
                 auto instanceId = req.path_params.at("instanceId");
                 auto itemId = req.path_params.at("itemId");
                 int value = JsonSerializer::getInt(req.body, "value", 0);
                 bool accept = JsonSerializer::getInt(req.body, "accept", 0) != 0;
                 std::lock_guard<std::mutex> lock(engineMutex);
                 auto result = facade->overrideItem(instanceId, itemId, value, accept);
                 res.status = toHttpStatus(result);
                 res.set_content(resultJson(result), "application/json");
            });

    // Get current strategy
    svr.Get(R"(/api/(education|healthcare)/:instanceId/strategy)",
            [this](const httplib::Request& req, httplib::Response& res)
            {
                auto id = req.path_params.at("instanceId");
                std::lock_guard<std::mutex> lock(engineMutex);
                auto name = facade->getStrategyName(id);
                if (name.empty())
                {
                    res.status = 404;
                    res.set_content(
                        JsonSerializer::serializeError("NotFound", "instance"),
                        "application/json");
                    return;
                }
                res.status = 200;
                res.set_content("{\"strategy\":\"" + name + "\"}", "application/json");
            });

    // Set strategy
    svr.Put(R"(/api/(education|healthcare)/:instanceId/strategy)",
            [this](const httplib::Request& req, httplib::Response& res)
            {
                auto id = req.path_params.at("instanceId");
                auto name = JsonSerializer::getString(req.body, "strategy", "");
                if (name.empty())
                {
                    res.status = 400;
                    res.set_content(
                        JsonSerializer::serializeError("InvalidArgument", "strategy is required"),
                        "application/json");
                    return;
                }
                std::lock_guard<std::mutex> lock(engineMutex);
                auto result = facade->setStrategy(id, name);
                res.status = toHttpStatus(result);
                res.set_content(resultJson(result), "application/json");
            });

    //CORS handler
    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        if (req.method == "OPTIONS") {
            res.status = 200;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });

    return svr.listen(host, port);
}