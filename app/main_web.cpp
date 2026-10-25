#include "httplib.h"
#include <mutex>

int main()
{
  httplib::Server svr;
  std::mutex engineMutex;

  svr.set_default_headers({{"Access-Control-Allow-Origin", "*"},
                           {"Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS"},
                           {"Access-Control-Allow-Headers", "Content-Type"}});
  svr.Options(R"(.*)", [](const auto &, auto &res)
              { res.status = 200; });

  svr.Get("/health", [](const auto &, auto &res) {});

  // Education
  svr.Post("/api/education/start", [](const auto &, auto &res) {});

  svr.Get("/api/education/instances", [](const auto &, auto &res) {});

  svr.Get("/api/education/:instanceId/state", [](const auto &req, auto &res)
          { std::string id = req.path_params.at("instanceId"); });

  svr.Get("/api/education/:instanceId/history", [](const auto &req, auto &res)
          { std::string id = req.path_params.at("instanceId"); });

  svr.Post("/api/education/:instanceId/submit", [](const auto &req, auto &res)
           { std::string id = req.path_params.at("instanceId"); });

  svr.Post("/api/education/:instanceId/items/:itemId/start",
           [](const auto &req, auto &res)
           {
             std::string instanceId = req.path_params.at("instanceId");
             std::string itemId = req.path_params.at("itemId");
           });

  svr.Post("/api/education/:instanceId/items/:itemId/complete",
           [](const auto &req, auto &res)
           {
             std::string instanceId = req.path_params.at("instanceId");
             std::string itemId = req.path_params.at("itemId");
           });

  svr.Post("/api/education/:instanceId/containers/:containerId/approve",
           [](const auto &req, auto &res) {});

  svr.Post("/api/education/:instanceId/items/:itemId/override",
           [](const auto &req, auto &res) {});

  // Healthcare
  svr.Post("/api/healthcare/start", [](const auto &, auto &res) {});

  svr.Get("/api/healthcare/instances", [](const auto &, auto &res) {});

  svr.Get("/api/healthcare/:instanceId/state", [](const auto &req, auto &res) {});

  svr.Get("/api/healthcare/:instanceId/history", [](const auto &req, auto &res) {});

  svr.Post("/api/healthcare/:instanceId/admit", [](const auto &req, auto &res) {});

  svr.Post("/api/healthcare/:instanceId/items/:itemId/start",
           [](const auto &req, auto &res) {});

  svr.Post("/api/healthcare/:instanceId/items/:itemId/complete",
           [](const auto &req, auto &res) {});

  svr.Post("/api/healthcare/:instanceId/containers/:containerId/approve",
           [](const auto &req, auto &res) {});

  svr.Post("/api/healthcare/:instanceId/items/:itemId/override",
           [](const auto &req, auto &res) {});


  return 0;
}