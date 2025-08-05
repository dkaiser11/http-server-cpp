#include "httpserver.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main()
{
    HttpServer server;
    int exit_code = server.run();
    return exit_code;
}