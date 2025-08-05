#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <arpa/inet.h>

class HttpServer
{
private:
    int m_server_file_descriptor;
    struct sockaddr_in m_server_address;
    int m_client_socket;
    struct sockaddr_in m_client_address;

public:
    HttpServer() = default;

    HttpServer(const HttpServer &) = delete;
    HttpServer &operator=(const HttpServer &) = delete;

    HttpServer(HttpServer &&) = delete;
    HttpServer &operator=(HttpServer &&) = delete;

    ~HttpServer() = default;

    int run(int port = 8080, int connection_backlog = 5, int reuse = 1);

    void handle_client();
};

#endif // HTTPSERVER_HPP
