#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "http/httpmethod.hpp"
#include <map>
#include <string>

class HttpRequest
{
private:
    HttpMethod method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    void parseRequestLine(const std::string &line);
    void parseHeader(const std::string &headerLine);
    void parseBody(const std::string &bodyContent);

public:
    HttpRequest() = default;

    HttpRequest(const HttpRequest &other) = default;
    HttpRequest &operator=(const HttpRequest &other) = default;

    HttpRequest(HttpRequest &&other) = default;
    HttpRequest &operator=(HttpRequest &&other) = default;

    ~HttpRequest() = default;

    static HttpRequest fromString(std::string rawRequest);

    HttpMethod getMethod() const;
    const std::string &getUri() const;
    const std::string &getVersion() const;
    const std::map<std::string, std::string> &getHeaders() const;
    const std::string &getBody() const;
};

#endif // HTTPREQUEST_HPP