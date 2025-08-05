#include "helpers.hpp"
#include "http/httpresponse.hpp"
#include <sstream>
#include <algorithm>

HttpResponse HttpResponse::fromString(std::string rawResponse)
{
    std::istringstream stream(rawResponse);
    std::string line;
    HttpResponse response;

    if (std::getline(stream, line))
        response.parseResponseLine(line);

    while (std::getline(stream, line) && line != "\r" && !line.empty())
        response.parseHeader(line);

    std::streamoff pos = stream.tellg();
    std::string::size_type bodyStart = static_cast<std::string::size_type>(pos);

    if (0 <= pos && bodyStart < stream.str().size())
        response.parseBody(stream.str().substr(bodyStart));

    return response;
}

void HttpResponse::parseResponseLine(const std::string &line)
{
    std::istringstream lineStream(line);
    std::string version, code;

    lineStream >> version >> code;

    this->version = version;
    this->code = httpCodeFromString(code);
}

void HttpResponse::parseHeader(const std::string &headerLine)
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

void HttpResponse::parseBody(const std::string &bodyContent)
{
    this->body = bodyContent;
}

const std::string &HttpResponse::getVersion() const
{
    return version;
}

HttpCode HttpResponse::getCode() const
{
    return code;
}

const std::map<std::string, std::string> &HttpResponse::getHeaders() const
{
    return headers;
}

const std::string &HttpResponse::getBody() const
{
    return body;
}