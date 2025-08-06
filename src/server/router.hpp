#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "http/httpcode.hpp"
#include "http/httpmethod.hpp"
#include "http/httprequest.hpp"
#include "http/httpresponse.hpp"
#include <map>
#include <string>
#include <regex>
#include <functional>

using RouteHandler = std::function<HttpResponse(const HttpRequest &)>;

struct Route
{
    HttpMethod method;
    std::regex pattern;
    RouteHandler handler;

    Route(HttpMethod m, const std::string &p, RouteHandler h)
        : method(m), pattern(p), handler(std::move(h)) {}
};

class Router
{
private:
    std::vector<Route> m_routes;
    RouteHandler m_not_found_handler;
    RouteHandler m_method_not_allowed_handler;

public:
    Router();

    // Add routes
    void get(const std::string &path, RouteHandler handler);
    void post(const std::string &path, RouteHandler handler);
    void put(const std::string &path, RouteHandler handler);
    void delete_(const std::string &path, RouteHandler handler);

    // Generic route registration
    void addRoute(HttpMethod method, const std::string &path, RouteHandler handler);

    // Set custom error handlers
    void setNotFoundHandler(RouteHandler handler);
    void setMethodNotAllowedHandler(RouteHandler handler);

    // Route matching and execution
    HttpResponse handleRequest(const HttpRequest &request);

private:
    RouteHandler findHandler(const HttpRequest &request);
    HttpResponse createNotFoundResponse();
    HttpResponse createMethodNotAllowedResponse();
};

#endif // ROUTER_HPP