#include "httpserver.hpp"
#include "http/httpresponse.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

int HttpServer::run(int port, int connection_backlog, int reuse)
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    m_server_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (m_server_file_descriptor < 0)
    {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    if (setsockopt(m_server_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        std::cerr << "setsockopt failed\n";
        return 1;
    }

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_addr.s_addr = INADDR_ANY;
    m_server_address.sin_port = htons(port);

    if (bind(m_server_file_descriptor, (struct sockaddr *)&m_server_address, sizeof(m_server_address)) != 0)
    {
        std::cerr << "Failed to bind to port " << port << "\n";
        return 1;
    }

    if (listen(m_server_file_descriptor, connection_backlog) != 0)
    {
        std::cerr << "listen failed\n";
        return 1;
    }

    int client_address_length = sizeof(m_client_address);

    std::cout << "Waiting for a client to connect...\n";

    while (true)
    {
        m_client_socket = accept(m_server_file_descriptor, (struct sockaddr *)&m_client_address, (socklen_t *)&client_address_length);

        if (m_client_socket < 0)
        {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        std::cout << "Client connected\n";

        handle_client();
    }

    close(m_server_file_descriptor);

    return 0;
}

void HttpServer::handle_client()
{
    char buffer[1024];
    auto bytes_received = recv(m_client_socket, buffer, sizeof(buffer), 0);

    if (bytes_received < 0)
    {
        std::cerr << "Failed to receive request\n";
        return;
    }

    buffer[bytes_received] = '\0';

    std::cout << "Received request:\n"
              << buffer << "\n";

    HttpResponse response;

    send(m_client_socket, response.toString().c_str(), response.toString().size(), 0);

    close(m_client_socket);
}
