#include "server/httpserver.hpp"
#include <thread>
#include <atomic>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

HttpServer::HttpServer()
    : m_server_file_descriptor(-1),
      m_server_address{}
{
}

HttpServer::~HttpServer()
{
    if (m_server_file_descriptor >= 0)
    {
        close(m_server_file_descriptor);
    }
    m_server_file_descriptor = -1;
    std::cout << "Server socket closed\n";
}

int HttpServer::run(int port, int connection_backlog, int reuse)
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    m_server_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (m_server_file_descriptor < 0)
    {
        std::cerr << "Failed to create server socket: " << strerror(errno) << "\n";
        return 1;
    }

    if (setsockopt(m_server_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        std::cerr << "setsockopt failed: " << strerror(errno) << "\n";
        close(m_server_file_descriptor);
        return 1;
    }

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_addr.s_addr = INADDR_ANY;
    m_server_address.sin_port = htons(port);

    if (bind(m_server_file_descriptor, (struct sockaddr *)&m_server_address, sizeof(m_server_address)) != 0)
    {
        std::cerr << "Failed to bind to port " << port << ": " << strerror(errno) << "\n";
        close(m_server_file_descriptor);
        return 1;
    }

    if (listen(m_server_file_descriptor, connection_backlog) != 0)
    {
        std::cerr << "listen failed: " << strerror(errno) << "\n";
        close(m_server_file_descriptor);
        return 1;
    }

    struct sockaddr_in client_address;
    int client_address_length = sizeof(client_address);

    std::cout << "Waiting for a client to connect...\n";

    std::atomic<bool> running(true);

    while (running)
    {
        int client_file_descriptor = accept(m_server_file_descriptor, (struct sockaddr *)&client_address, (socklen_t *)&client_address_length);

        if (client_file_descriptor < 0)
        {
            if (errno == EINTR || errno == EBADF)
                break;
            std::cerr << "Failed to accept connection: " << strerror(errno) << "\n";
            continue;
        }

        std::cout << "Client connected\n";

        std::thread([this, client_file_descriptor]()
                    { handle_client(client_file_descriptor); })
            .detach();
    }

    return 0;
}

void HttpServer::handle_client(int client_file_descriptor)
{
    HttpRequest request;

    if (receive_request(client_file_descriptor, request) < 0)
    {
        std::cerr << "Failed to receive request: " << strerror(errno) << "\n";
        close(client_file_descriptor);
        return;
    }

    HttpResponse response;

    send_response(client_file_descriptor, response);

    close(client_file_descriptor);
}

int HttpServer::receive_request(int client_file_descriptor, HttpRequest &request)
{
    char buffer[4096];
    int bytes_received = recv(client_file_descriptor, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received <= 0)
    {
        if (bytes_received == 0)
            std::cout << "Client disconnected\n";
        else
            std::cerr << "Failed to receive request: " << strerror(errno) << "\n";

        return bytes_received;
    }

    buffer[bytes_received] = '\0';

    std::cout << "Received request:\n"
              << buffer << "\n";

    request = HttpRequest::fromString(std::string(buffer, bytes_received));
    return bytes_received;
}

int HttpServer::send_response(int client_file_descriptor, HttpResponse &response)
{
    int bytes_sent = send(client_file_descriptor, response.toString().c_str(), response.toString().size(), 0);

    if (bytes_sent < 0)
    {
        std::cerr << "Failed to send response: " << strerror(errno) << "\n";
    }

    return bytes_sent;
}
