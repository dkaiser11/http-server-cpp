#include "httpserver.hpp"
#include <iostream>

int main()
{
    HttpServer server;
    int exit_code = server.run();
    return exit_code;
}