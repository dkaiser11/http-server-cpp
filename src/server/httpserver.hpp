#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "http/httprequest.hpp"
#include "http/httpresponse.hpp"
#include "router.hpp"
#include <arpa/inet.h>
#include <mutex>

class HttpServer
{
private:
    int m_server_file_descriptor;
    struct sockaddr_in m_server_address;
    Router m_router;
    mutable std::mutex m_output_mutex;

public:
    HttpServer();
    HttpServer(const Router &router);

    HttpServer(const HttpServer &) = delete;
    HttpServer &operator=(const HttpServer &) = delete;

    HttpServer(HttpServer &&) = delete;
    HttpServer &operator=(HttpServer &&) = delete;

    ~HttpServer();

    void setRouter(const Router &router);
    const Router &getRouter() const;

    int run(int port = 8080, int connection_backlog = 5, int reuse = 1);

    void handle_client(int client_file_descriptor);
    int receive_request(int client_file_descriptor, HttpRequest &request);
    int send_response(int client_file_descriptor, HttpResponse &response);

    HttpResponse serve_static_file(const std::string &file_path, const std::string &web_root = "www");
};

#endif // HTTPSERVER_HPP
