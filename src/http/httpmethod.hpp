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

inline std::string httpMethodToString(const HttpMethod &method)
{
    switch (method)
    {
    case HttpMethod::GET:
        return "GET";
    case HttpMethod::POST:
        return "POST";
    case HttpMethod::PUT:
        return "PUT";
    case HttpMethod::DELETE:
        return "DELETE";
    case HttpMethod::PATCH:
        return "PATCH";
    case HttpMethod::HEAD:
        return "HEAD";
    case HttpMethod::OPTIONS:
        return "OPTIONS";
    case HttpMethod::CONNECT:
        return "CONNECT";
    case HttpMethod::TRACE:
        return "TRACE";
    default:
        throw std::invalid_argument("Unknown HttpMethod enum value");
    }
}

#endif // HTTPMETHOD_HPP
