#include "helpers.hpp"
#include "http/httprequest.hpp"
#include <sstream>
#include <algorithm>

HttpRequest HttpRequest::from_string(std::string raw_request)
{
    std::istringstream stream(raw_request);
    std::string line;
    HttpRequest request;

    if (std::getline(stream, line))
        request.parse_request_line(line);

    std::string headers;

    while (std::getline(stream, line) && line != "\r" && !line.empty())
        request.parse_header(line);

    std::streamoff pos = stream.tellg();
    std::string::size_type bodyStart = static_cast<std::string::size_type>(pos);

    if (0 <= pos && bodyStart < stream.str().size())
        request.parse_body(stream.str().substr(bodyStart));

    return request;
}

void HttpRequest::parse_request_line(const std::string &line)
{
    std::istringstream lineStream(line);
    std::string method, uri, version;

    lineStream >> method >> uri >> version;

    this->method = http_method_from_string(method);
    this->uri = uri;
    this->version = version;
}

void HttpRequest::parse_header(const std::string &header_line)
{
    auto colonPos = header_line.find(':');

    if (colonPos != std::string::npos)
    {
        std::string name = header_line.substr(0, colonPos);

        std::string value = header_line.substr(colonPos + 1);
        ltrim(value);
        rtrim(value);

        headers[name] = value;
    }
}

void HttpRequest::parse_body(const std::string &body_content)
{
    this->body = body_content;
}

HttpRequest::HttpRequest()
    : method(HttpMethod::GET), uri("/"), version("HTTP/1.1"), headers(), body()
{
}

HttpMethod HttpRequest::get_method() const
{
    return method;
}

const std::string &HttpRequest::get_uri() const
{
    return uri;
}

const std::string &HttpRequest::get_version() const
{
    return version;
}

const std::map<std::string, std::string> &HttpRequest::get_headers() const
{
    return headers;
}

const std::string &HttpRequest::get_body() const
{
    return body;
}

void HttpRequest::set_method(HttpMethod method)
{
    this->method = method;
}

void HttpRequest::set_uri(const std::string &uri)
{
    this->uri = uri;
}

void HttpRequest::set_version(const std::string &version)
{
    this->version = version;
}

void HttpRequest::add_header(const std::string &name, const std::string &value)
{
    headers[name] = value;
}

void HttpRequest::remove_header(const std::string &name)
{
    headers.erase(name);
}

void HttpRequest::set_body(const std::string &body)
{
    this->body = body;
}

std::string HttpRequest::to_string() const
{
    std::ostringstream oss;
    oss << http_method_to_string(method) << " " << uri << " " << version << "\r\n";

    for (const auto &[name, value] : headers)
    {
        oss << name << ": " << value << "\r\n";
    }

    oss << "\r\n"
        << body;

    return oss.str();
}
