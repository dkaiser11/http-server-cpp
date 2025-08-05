#include "helpers.hpp"
#include "http/httprequest.hpp"
#include <sstream>
#include <algorithm>
#include "httprequest.hpp"

HttpRequest HttpRequest::fromString(std::string rawRequest)
{
    std::istringstream stream(rawRequest);
    std::string line;
    HttpRequest request;

    if (std::getline(stream, line))
        request.parseRequestLine(line);

    std::string headers;

    while (std::getline(stream, line) && line != "\r" && !line.empty())
        request.parseHeader(line);

    std::streamoff pos = stream.tellg();
    std::string::size_type bodyStart = static_cast<std::string::size_type>(pos);

    if (0 <= pos && bodyStart < stream.str().size())
        request.parseBody(stream.str().substr(bodyStart));

    return request;
}

void HttpRequest::parseRequestLine(const std::string &line)
{
    std::istringstream lineStream(line);
    std::string method, uri, version;

    lineStream >> method >> uri >> version;

    this->method = httpMethodFromString(method);
    this->uri = uri;
    this->version = version;
}

void HttpRequest::parseHeader(const std::string &headerLine)
{
    auto colonPos = headerLine.find(':');

    if (colonPos != std::string::npos)
    {
        std::string name = headerLine.substr(0, colonPos);

        std::string value = headerLine.substr(colonPos + 1);
        ltrim(value);
        rtrim(value);

        headers[name] = value;
    }
}

void HttpRequest::parseBody(const std::string &bodyContent)
{
    this->body = bodyContent;
}

HttpMethod HttpRequest::getMethod() const
{
    return method;
}

const std::string &HttpRequest::getUri() const
{
    return uri;
}

const std::string &HttpRequest::getVersion() const
{
    return version;
}

const std::map<std::string, std::string> &HttpRequest::getHeaders() const
{
    return headers;
}

const std::string &HttpRequest::getBody() const
{
    return body;
}

std::string HttpRequest::toString() const
{
    std::ostringstream oss;
    oss << httpMethodToString(method) << " " << uri << " " << version << "\r\n";

    for (const auto &[name, value] : headers)
    {
        oss << name << ": " << value << "\r\n";
    }

    oss << "\r\n"
        << body;

    return oss.str();
}
