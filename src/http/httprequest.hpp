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

    void parse_request_line(const std::string &line);
    void parse_header(const std::string &header_line);
    void parse_body(const std::string &body_content);

public:
    HttpRequest();

    HttpRequest(const HttpRequest &other) = default;
    HttpRequest &operator=(const HttpRequest &other) = default;

    HttpRequest(HttpRequest &&other) = default;
    HttpRequest &operator=(HttpRequest &&other) = default;

    ~HttpRequest() = default;

    static HttpRequest from_string(std::string raw_request);

    HttpMethod get_method() const;
    const std::string &get_uri() const;
    const std::string &get_version() const;
    const std::map<std::string, std::string> &get_headers() const;
    const std::string &get_body() const;

    void set_method(HttpMethod method);
    void set_uri(const std::string &uri);
    void set_version(const std::string &version);
    void add_header(const std::string &name, const std::string &value);
    void remove_header(const std::string &name);
    void set_body(const std::string &body);

    std::string to_string() const;
};

#endif // HTTPREQUEST_HPP