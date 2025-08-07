#include "helpers.hpp"
#include "http/httpresponse.hpp"
#include <sstream>
#include <algorithm>
#include "httpresponse.hpp"

HttpResponse HttpResponse::from_string(std::string raw_response)
{
    std::istringstream stream(raw_response);
    std::string line;
    HttpResponse response;

    if (std::getline(stream, line))
        response.parse_response_line(line);

    while (std::getline(stream, line) && line != "\r" && !line.empty())
        response.parse_header(line);

    std::streamoff pos = stream.tellg();
    std::string::size_type bodyStart = static_cast<std::string::size_type>(pos);

    if (0 <= pos && bodyStart < stream.str().size())
        response.parse_body(stream.str().substr(bodyStart));

    return response;
}

void HttpResponse::parse_response_line(const std::string &line)
{
    std::istringstream lineStream(line);
    std::string version, code;

    lineStream >> version >> code;

    this->version = version;
    this->code = http_code_from_string(code);
}

void HttpResponse::parse_header(const std::string &header_line)
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

void HttpResponse::parse_body(const std::string &body_content)
{
    this->body = body_content;
}

HttpResponse::HttpResponse()
    : version("HTTP/1.1"), code(HttpCode::OK), headers(), body()
{
}

const std::string &HttpResponse::get_version() const
{
    return version;
}

HttpCode HttpResponse::get_code() const
{
    return code;
}

const std::map<std::string, std::string> &HttpResponse::get_headers() const
{
    return headers;
}

const std::string &HttpResponse::get_body() const
{
    return body;
}

void HttpResponse::set_version(const std::string &version)
{
    this->version = version;
}

void HttpResponse::set_code(HttpCode code)
{
    this->code = code;
}

void HttpResponse::set_body(const std::string &body)
{
    this->body = body;
}

void HttpResponse::add_header(const std::string &name, const std::string &value)
{
    headers[name] = value;
}

void HttpResponse::remove_header(const std::string &name)
{
    headers.erase(name);
}

std::string HttpResponse::to_string() const
{
    std::ostringstream oss;
    oss << version << " " << http_code_to_string(code) << "\r\n";

    for (const auto &[name, value] : headers)
    {
        oss << name << ": " << value << "\r\n";
    }

    oss << "\r\n"
        << body;

    return oss.str();
}