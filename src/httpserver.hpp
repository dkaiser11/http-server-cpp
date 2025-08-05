#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

class HttpServer
{
private:
public:
    HttpServer() = default;

    HttpServer(const HttpServer &) = delete;
    HttpServer &operator=(const HttpServer &) = delete;

    HttpServer(HttpServer &&) = delete;
    HttpServer &operator=(HttpServer &&) = delete;

    ~HttpServer() = default;
};

#endif // HTTPSERVER_HPP
