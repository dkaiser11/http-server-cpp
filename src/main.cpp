#include "server/httpserver.hpp"
#include <iostream>

int main()
{
    Router router;

    router.get("/", [](const HttpRequest &request) -> HttpResponse
               {
                    HttpServer server;
                    return server.serve_static_file("index.html"); });

    router.get(".*\\.(html|htm|css|js|png|jpg|jpeg|gif|svg|ico|json)$",
               [](const HttpRequest &request) -> HttpResponse
               {
                   HttpServer server;
                   std::string path = request.getUri();

                   if (path.front() == '/')
                   {
                       path = path.substr(1);
                   }

                   if (path.empty())
                   {
                       path = "index.html";
                   }

                   return server.serve_static_file(path);
               });

    HttpServer server(router);

    int exit_code = server.run();
    return exit_code;
}