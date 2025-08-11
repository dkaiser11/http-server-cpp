#include "server/router.hpp"

Router::Router()
{
    m_not_found_handler = [](const HttpRequest &) -> HttpResponse
    {
        HttpResponse response;
        response.set_code(HttpCode::NotFound);
        response.add_header("Content-Type", "text/html");
        response.set_body("<html><body><h1>404 - Not Found</h1></body></html>");
        return response;
    };

    m_method_not_allowed_handler = [](const HttpRequest &) -> HttpResponse
    {
        HttpResponse response;
        response.set_code(HttpCode::MethodNotAllowed);
        response.add_header("Content-Type", "text/html");
        response.set_body("<html><body><h1>405 - Method Not Allowed</h1></body></html>");
        return response;
    };
}

void Router::get(const std::string &path, RouteHandler handler)
{
    add_route(HttpMethod::GET, path, std::move(handler));
}

void Router::post(const std::string &path, RouteHandler handler)
{
    add_route(HttpMethod::POST, path, std::move(handler));
}

void Router::put(const std::string &path, RouteHandler handler)
{
    add_route(HttpMethod::PUT, path, std::move(handler));
}

void Router::delete_(const std::string &path, RouteHandler handler)
{
    add_route(HttpMethod::DELETE, path, std::move(handler));
}

void Router::add_route(HttpMethod method, const std::string &path, RouteHandler handler)
{
    m_routes.emplace_back(method, path, std::move(handler));
}

void Router::set_not_found_handler(RouteHandler handler)
{
    m_not_found_handler = std::move(handler);
}

void Router::set_method_not_allowed_handler(RouteHandler handler)
{
    m_method_not_allowed_handler = std::move(handler);
}

HttpResponse Router::handle_request(const HttpRequest &request)
{
    for (const auto &route : m_routes)
    {
        if (route.method == request.get_method() &&
            std::regex_match(request.get_uri(), route.pattern))
        {
            try
            {
                return route.handler(request);
            }
            catch (const std::exception &e)
            {
                HttpResponse error_response;
                error_response.set_code(HttpCode::InternalServerError);
                error_response.add_header("Content-Type", "text/html");
                error_response.set_body("<html><body><h1>500 - Internal Server Error</h1></body></html>");
                return error_response;
            }
        }
    }

    bool path_exists = std::any_of(m_routes.begin(), m_routes.end(),
                                   [&request](const Route &route)
                                   {
                                       return std::regex_match(request.get_uri(), route.pattern);
                                   });

    if (path_exists)
    {
        return m_method_not_allowed_handler(request);
    }

    return m_not_found_handler(request);
}