#include "server/router.hpp"
#include <algorithm>

Router::Router()
{
    m_not_found_handler = [](const HttpRequest &) -> HttpResponse
    {
        HttpResponse response;
        response.setCode(HttpCode::NotFound);
        response.addHeader("Content-Type", "text/html");
        response.setBody("<html><body><h1>404 - Not Found</h1></body></html>");
        return response;
    };

    m_method_not_allowed_handler = [](const HttpRequest &) -> HttpResponse
    {
        HttpResponse response;
        response.setCode(HttpCode::MethodNotAllowed);
        response.addHeader("Content-Type", "text/html");
        response.setBody("<html><body><h1>405 - Method Not Allowed</h1></body></html>");
        return response;
    };
}

void Router::get(const std::string &path, RouteHandler handler)
{
    addRoute(HttpMethod::GET, path, std::move(handler));
}

void Router::post(const std::string &path, RouteHandler handler)
{
    addRoute(HttpMethod::POST, path, std::move(handler));
}

void Router::put(const std::string &path, RouteHandler handler)
{
    addRoute(HttpMethod::PUT, path, std::move(handler));
}

void Router::delete_(const std::string &path, RouteHandler handler)
{
    addRoute(HttpMethod::DELETE, path, std::move(handler));
}

void Router::addRoute(HttpMethod method, const std::string &path, RouteHandler handler)
{
    m_routes.emplace_back(method, path, std::move(handler));
}

void Router::setNotFoundHandler(RouteHandler handler)
{
    m_not_found_handler = std::move(handler);
}

void Router::setMethodNotAllowedHandler(RouteHandler handler)
{
    m_method_not_allowed_handler = std::move(handler);
}

HttpResponse Router::handleRequest(const HttpRequest &request)
{
    for (const auto &route : m_routes)
    {
        if (route.method == request.getMethod() &&
            std::regex_match(request.getUri(), route.pattern))
        {
            try
            {
                return route.handler(request);
            }
            catch (const std::exception &e)
            {
                HttpResponse error_response;
                error_response.setCode(HttpCode::InternalServerError);
                error_response.addHeader("Content-Type", "text/html");
                error_response.setBody("<html><body><h1>500 - Internal Server Error</h1></body></html>");
                return error_response;
            }
        }
    }

    bool path_exists = std::any_of(m_routes.begin(), m_routes.end(),
                                   [&request](const Route &route)
                                   {
                                       return std::regex_match(request.getUri(), route.pattern);
                                   });

    if (path_exists)
    {
        return m_method_not_allowed_handler(request);
    }

    return m_not_found_handler(request);
}