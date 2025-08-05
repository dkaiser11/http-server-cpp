#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <arpa/inet.h>
#include <thread>
#include <atomic>
#include <memory>

class HttpServer
{
private:
    int m_port;
    int m_connection_backlog;
    int m_reuse;
    int m_server_file_descriptor;
    struct sockaddr_in m_server_address;
    int m_client_socket;
    struct sockaddr_in m_client_address;

    std::unique_ptr<std::thread> m_server_thread;
    std::atomic<bool> m_running;

    void server_loop();

public:
    HttpServer();

    HttpServer(const HttpServer &) = delete;
    HttpServer &operator=(const HttpServer &) = delete;

    HttpServer(HttpServer &&) = delete;
    HttpServer &operator=(HttpServer &&) = delete;

    ~HttpServer();

    bool start(int port = 8080, int connection_backlog = 5, int reuse = 1);
    void stop();
    bool is_running() const;

    int run(int port = 8080, int connection_backlog = 5, int reuse = 1);

    void handle_client();
};

#endif // HTTPSERVER_HPP
