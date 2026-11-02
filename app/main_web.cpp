/**
 * @file main_web.cpp
 * @brief Entry point for the TIVIDY REST server.
*/

#include "RestServer.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    std::string host = "0.0.0.0";
    int port = 8080;

    if(argc > 1)
    {
        port = std::atoi(argv[1]);
    }
    if(argc > 2)
    {
        host = argv[2];
    }

    std::cout << "TIVIDY REST server on http://" << host << ":" << port << std::endl;

    RestServer server;
    if(!server.run(host, port))
    {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    return 0;
}