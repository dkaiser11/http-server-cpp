#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "http/httpcode.hpp"
#include <map>
#include <string>

class HttpResponse
{
private:
    std::string version;
    HttpCode code;
    std::map<std::string, std::string> headers;
    std::string body;

    void parse_response_line(const std::string &line);
    void parse_header(const std::string &header_line);
    void parse_body(const std::string &body_content);

public:
    HttpResponse();

    HttpResponse(const HttpResponse &other) = default;
    HttpResponse &operator=(const HttpResponse &other) = default;

    HttpResponse(HttpResponse &&other) = default;
    HttpResponse &operator=(HttpResponse &&other) = default;

    ~HttpResponse() = default;

    static HttpResponse from_string(std::string raw_response);

    const std::string &get_version() const;
    HttpCode get_code() const;
    const std::map<std::string, std::string> &get_headers() const;
    const std::string &get_body() const;

    void set_version(const std::string &version);
    void set_code(HttpCode code);
    void add_header(const std::string &name, const std::string &value);
    void remove_header(const std::string &name);
    void set_body(const std::string &body);

    std::string to_string() const;
};

#endif // HTTPRESPONSE_HPP