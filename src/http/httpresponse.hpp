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

    void parseResponseLine(const std::string &line);
    void parseHeader(const std::string &headerLine);
    void parseBody(const std::string &bodyContent);

public:
    HttpResponse() = default;

    HttpResponse(const HttpResponse &other) = default;
    HttpResponse &operator=(const HttpResponse &other) = default;

    HttpResponse(HttpResponse &&other) = default;
    HttpResponse &operator=(HttpResponse &&other) = default;

    ~HttpResponse() = default;

    static HttpResponse fromString(std::string rawResponse);

    const std::string &getVersion() const;
    HttpCode getCode() const;
    const std::map<std::string, std::string> &getHeaders() const;
    const std::string &getBody() const;
};

#endif // HTTPRESPONSE_HPP