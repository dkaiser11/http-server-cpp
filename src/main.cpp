#include "server/httpserver.hpp"
#include <iostream>

int main()
{
    HttpServer server;
    Router router;

    router.get("/", [&server](const HttpRequest &request) -> HttpResponse
               { return server.serve_static_file("index.html"); });

    router.get(".*\\.(html|htm|css|js|png|jpg|jpeg|gif|svg|ico|json)$",
               [&server](const HttpRequest &request) -> HttpResponse
               {
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

    server.setRouter(router);

    int exit_code = server.run();
    return exit_code;
}