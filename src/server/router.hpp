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
    void add_route(HttpMethod method, const std::string &path, RouteHandler handler);

    // Set custom error handlers
    void set_not_found_handler(RouteHandler handler);
    void set_method_not_allowed_handler(RouteHandler handler);

    // Route matching and execution
    HttpResponse handle_request(const HttpRequest &request);

private:
    RouteHandler find_handler(const HttpRequest &request);
    HttpResponse create_not_found_response();
    HttpResponse create_method_not_allowed_response();
};

#endif // ROUTER_HPP