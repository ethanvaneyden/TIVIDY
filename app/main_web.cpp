
#include "httplib.h"
#include <mutex>
#include <string>

int main()
{
    httplib::Server svr;
    std::mutex engineMutex;

    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    svr.Options(R"(.*)", [](const httplib::Request &, httplib::Response &res)
    {
        res.status = 200;
    });

    svr.Get("/health", [](const httplib::Request &, httplib::Response &res)
    {
        res.status = 200;
        res.set_content("OK", "text/plain");
    });

    // Education
    svr.Post("/api/education/start",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Get("/api/education/instances",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Get("/api/education/:instanceId/state",
        [](const httplib::Request &req, httplib::Response &)
        {
            std::string id = req.path_params.at("instanceId");
        });

    svr.Get("/api/education/:instanceId/history",
        [](const httplib::Request &req, httplib::Response &)
        {
            std::string id = req.path_params.at("instanceId");
        });

    svr.Post("/api/education/:instanceId/submit",
        [](const httplib::Request &req, httplib::Response &)
        {
            std::string id = req.path_params.at("instanceId");
        });

    svr.Post("/api/education/:instanceId/items/:itemId/start",
        [](const httplib::Request &req, httplib::Response &)
        {
            std::string instanceId = req.path_params.at("instanceId");
            std::string itemId = req.path_params.at("itemId");
        });

    svr.Post("/api/education/:instanceId/items/:itemId/complete",
        [](const httplib::Request &req, httplib::Response &)
        {
            std::string instanceId = req.path_params.at("instanceId");
            std::string itemId = req.path_params.at("itemId");
        });

    svr.Post("/api/education/:instanceId/containers/:containerId/approve",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Post("/api/education/:instanceId/items/:itemId/override",
        [](const httplib::Request &, httplib::Response &) {});

    // Healthcare
    svr.Post("/api/healthcare/start",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Get("/api/healthcare/instances",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Get("/api/healthcare/:instanceId/state",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Get("/api/healthcare/:instanceId/history",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Post("/api/healthcare/:instanceId/admit",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Post("/api/healthcare/:instanceId/items/:itemId/start",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Post("/api/healthcare/:instanceId/items/:itemId/complete",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Post("/api/healthcare/:instanceId/containers/:containerId/approve",
        [](const httplib::Request &, httplib::Response &) {});

    svr.Post("/api/healthcare/:instanceId/items/:itemId/override",
        [](const httplib::Request &, httplib::Response &) {});

    return 0;
}
