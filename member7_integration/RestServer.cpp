/**
 * @file RestServer.cpp
 * @brief Implementation of the REST bridge.
*/

#include "RestServer.h"
#include "TividyWorkflow.h"
#include "JsonSerializer.h"
#include "Container.h"
#include "Root.h"

#include "httplib.h"

namespace
{
    int toHttpStatus(CommandResult result)
    {
        switch(result)
        {
            case CommandResult::Success: return 200;
            case CommandResult::InvalidTransition: return 409;
            case CommandResult::Rejected: return 409;
            case CommandResult::IntegrationFailure: return 502;
            case CommandResult::NotFound: return 404;
        }
        return 500;
    }

    std::string toResultString(CommandResult result)
    {
        switch(result)
        {
            case CommandResult::Success: return "Success";
            case CommandResult::InvalidTransition: return "InvalidTransition";
            case CommandResult::Rejected: return "Rejected";
            case CommandResult::IntegrationFailure: return "IntegrationFailure";
            case CommandResult::NotFound: return "NotFound";
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

RestServer::~RestServer() = default;

bool RestServer::run(const std::string& host, int port)
{
    httplib::Server svr;

    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res)
    {
        res.status = 200;
    });

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res)
    {
        res.status = 200;
        res.set_content("OK", "text/plain");
    });

    /* ---------------- Education ----------------*/

    svr.Post("/api/education/start",
        [this](const httplib::Request& req, httplib::Response& res)
        {
            std::string name = JsonSerializer::getString(req.body, "name", "untitled");
            std::lock_guard<std::mutex> lock(engineMutex);
            auto id = facade->startEducation(name);
            res.status = 200;
            res.set_content("{\"instanceId\":\"" + id + "\"}", "application/json");
        });

    svr.Get("/api/education/instances",
        [this](const httplib::Request&, httplib::Response& res)
        {
            std::lock_guard<std::mutex> lock(engineMutex);
            auto ids = facade->listEducationInstances();
            std::string out = "[";
            for(size_t i = 0; i < ids.size(); ++i)
            {
                if(i != 0) out += ",";
                out += "\"" + ids[i] + "\"";
            }
            out += "]";
            res.status = 200;
            res.set_content(out, "application/json");
        });

    svr.Get("/api/education/:instanceId/state",
        [this](const httplib::Request& req, httplib::Response& res)
        {
            auto id = req.path_params.at("instanceId");
            std::lock_guard<std::mutex> lock(engineMutex);
            Root* tree = facade->getTree(id);
            if(tree == nullptr)
            {
                res.status = 404;
                res.set_content(JsonSerializer::serializeError("NotFound", "instance"),
                                "application/json");
                return;
            }
            res.status = 200;
            res.set_content(JsonSerializer::serializeTree(tree), "application/json");
        });

    svr.Get("/api/education/:instanceId/history",
        [this](const httplib::Request& req, httplib::Response& res)
        {
            auto id = req.path_params.at("instanceId");
            std::lock_guard<std::mutex> lock(engineMutex);
            auto hist = facade->getHistory(id);
            res.status = 200;
            res.set_content(JsonSerializer::serializeHistory(hist), "application/json");
        });

    svr.Post("/api/education/:instanceId/submit",
        [this](const httplib::Request& req, httplib::Response& res)
        {
            auto id = req.path_params.at("instanceId");
            auto container = JsonSerializer::getString(req.body, "containerName", "submission");
            std::lock_guard<std::mutex> lock(engineMutex);
            auto result = facade->submitEducation(id, container);
            res.status = toHttpStatus(result);
            res.set_content(resultJson(result), "application/json");
        });

    svr.Post("/api/education/:instanceId/items/:itemId/start",
        [this](const httplib::Request& req, httplib::Response& res)
        {
            auto instanceId = req.path_params.at("instanceId");
            auto itemId = req.path_params.at("itemId");
            std::lock_guard<std::mutex> lock(engineMutex);
            auto result = facade->startItem(instanceId, itemId);
            res.status = toHttpStatus(result);
            res.set_content(resultJson(result), "application/json");
        });

    svr.Post("/api/education/:instanceId/items/:itemId/complete",
        [this](const httplib::Request& req, httplib::Response& res)
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
        [this](const httplib::Request& req, httplib::Response& res)
        {
            auto instanceId = req.path_params.at("instanceId");
            auto containerId = req.path_params.at("containerId");
            std::lock_guard<std::mutex> lock(engineMutex);
            auto result = facade->approveContainer(instanceId, containerId);
            res.status = toHttpStatus(result);
            res.set_content(resultJson(result), "application/json");
        });

    svr.Post("/api/education/:instanceId/items/:itemId/override",
        [this](const httplib::Request& req, httplib::Response& res)
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
        [this](const httplib::Request& req, httplib::Response& res)
        {
            std::string name = JsonSerializer::getString(req.body, "name", "untitled");
            std::lock_guard<std::mutex> lock(engineMutex);
            auto id = facade->startHealthcare(name);
            res.status = 200;
            res.set_content("{\"instanceId\":\"" + id + "\"}", "application/json");
        });

    svr.Get("/api/healthcare/instances",
        [this](const httplib::Request&, httplib::Response& res)
        {
            std::lock_guard<std::mutex> lock(engineMutex);
            auto ids = facade->listHealthcareInstances();
            std::string out = "[";
            for(size_t i = 0; i < ids.size(); ++i)
            {
                if(i != 0) out += ",";
                out += "\"" + ids[i] + "\"";
            }
            out += "]";
            res.status = 200;
            res.set_content(out, "application/json");
        });

    svr.Get("/api/healthcare/:instanceId/state",
        [this](const httplib::Request& req, httplib::Response& res)
        {
            auto id = req.path_params.at("instanceId");
            std::lock_guard<std::mutex> lock(engineMutex);
            Root* tree = facade->getTree(id);
            if(tree == nullptr)
            {
                res.status = 404;
                res.set_content(JsonSerializer::serializeError("NotFound", "instance"),
                                "application/json");
                return;
            }
            res.status = 200;
            res.set_content(JsonSerializer::serializeTree(tree), "application/json");
        });

    svr.Get("/api/healthcare/:instanceId/history",
        [this](const httplib::Request& req, httplib::Response& res)
        {
            auto id = req.path_params.at("instanceId");
            std::lock_guard<std::mutex> lock(engineMutex);
            auto hist = facade->getHistory(id);
            res.status = 200;
            res.set_content(JsonSerializer::serializeHistory(hist), "application/json");
        });

    svr.Post("/api/healthcare/:instanceId/admit",
        [this](const httplib::Request& req, httplib::Response& res)
        {
            auto id = req.path_params.at("instanceId");
            auto container = JsonSerializer::getString(req.body, "containerName", "patient");
            std::lock_guard<std::mutex> lock(engineMutex);
            auto result = facade->admitPatient(id, container);
            res.status = toHttpStatus(result);
            res.set_content(resultJson(result), "application/json");
        });

    svr.Post("/api/healthcare/:instanceId/items/:itemId/start",
        [this](const httplib::Request& req, httplib::Response& res)
        {
            auto instanceId = req.path_params.at("instanceId");
            auto itemId = req.path_params.at("itemId");
            std::lock_guard<std::mutex> lock(engineMutex);
            auto result = facade->startItem(instanceId, itemId);
            res.status = toHttpStatus(result);
            res.set_content(resultJson(result), "application/json");
        });

    svr.Post("/api/healthcare/:instanceId/items/:itemId/complete",
        [this](const httplib::Request& req, httplib::Response& res)
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
        [this](const httplib::Request& req, httplib::Response& res)
        {
            auto instanceId = req.path_params.at("instanceId");
            auto containerId = req.path_params.at("containerId");
            std::lock_guard<std::mutex> lock(engineMutex);
            auto result = facade->approveContainer(instanceId, containerId);
            res.status = toHttpStatus(result);
            res.set_content(resultJson(result), "application/json");
        });

    svr.Post("/api/healthcare/:instanceId/items/:itemId/override",
        [this](const httplib::Request& req, httplib::Response& res)
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

    return svr.listen(host, port);
}