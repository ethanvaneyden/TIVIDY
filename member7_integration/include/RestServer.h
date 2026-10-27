/**
 * @file RestServer.h
 * @brief REST bridge between React and the TividyWorkflow facade.
*/

#ifndef RESTSERVER_H
#define RESTSERVER_H

#include <memory>
#include <mutex>
#include <string>

class TividyWorkflow;

/**
 * @class RestServer
 * @brief Owns the HTTP server and wires every endpoint to a facade call.
 *
 * No endpoint contains workflow logic. Every handler parses the request,
 * calls one facade method, and serializes the response. That is the
 * whole design.
*/
class RestServer
{
    private:
        std::unique_ptr<TividyWorkflow> facade;
        std::mutex engineMutex;

        /* Endpoint implementations are lambdas inside run(); keeping them
         * inline makes the request/response mapping readable at a glance.*/

    public:
        RestServer();
        ~RestServer() = default;

        RestServer(const RestServer&) = delete;
        RestServer& operator=(const RestServer&) = delete;

        /**
         * @brief Start the HTTP server. Blocks until the server stops.
         * @param host Bind address ("0.0.0.0" for all interfaces).
         * @param port TCP port.
         * @return true if the server started and ran, false otherwise.
        */
        bool run(const std::string& host, int port);
};

#endif /*RESTSERVER_*/