#include "server/httpserver.hpp"
#include <iostream>

int main()
{
    Router router;

    router.get("/", [](const HttpRequest &) -> HttpResponse
               {
        HttpResponse response;
        response.addHeader("Content-Type", "text/html");
        response.setBody("<html><body><h1>Hello world!</h1></body></html>");
        return response; });

    router.get("/\\d+", [](const HttpRequest &) -> HttpResponse
               {
        HttpResponse response;
        response.addHeader("Content-Type", "text/html");
        response.setBody("<html><body><h1>Hello number!</h1></body></html>");
        return response; });

    router.get("/lol/1", [](const HttpRequest &) -> HttpResponse
               {
        HttpResponse response;
        response.addHeader("Content-Type", "text/html");
        response.setBody("<html><body><h1>1!</h1></body></html>");
        return response; });

    router.get("/lol/2", [](const HttpRequest &) -> HttpResponse
               {
        HttpResponse response;
        response.addHeader("Content-Type", "text/html");
        response.setBody("<html><body><h1>2!</h1></body></html>");
        return response; });

    HttpServer server(router);
    int exit_code = server.run();
    return exit_code;
}