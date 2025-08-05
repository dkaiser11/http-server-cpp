#ifndef HTTPMETHOD_HPP
#define HTTPMETHOD_HPP

#include <string>
#include <optional>
#include <stdexcept>

enum class HttpMethod
{
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS,
    CONNECT,
    TRACE,
};

inline HttpMethod httpMethodFromString(const std::string &method)
{
    if (method == "GET")
        return HttpMethod::GET;
    if (method == "POST")
        return HttpMethod::POST;
    if (method == "PUT")
        return HttpMethod::PUT;
    if (method == "DELETE")
        return HttpMethod::DELETE;
    if (method == "PATCH")
        return HttpMethod::PATCH;
    if (method == "HEAD")
        return HttpMethod::HEAD;
    if (method == "OPTIONS")
        return HttpMethod::OPTIONS;
    if (method == "CONNECT")
        return HttpMethod::CONNECT;
    if (method == "TRACE")
        return HttpMethod::TRACE;

    throw std::invalid_argument("Unknown HTTP method: " + method);
}

#endif // HTTPMETHOD_HPP
