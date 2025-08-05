#include "httpserver.hpp"
#include "http/httpresponse.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

HttpServer::HttpServer()
    : m_server_file_descriptor(-1),
      m_client_socket(-1),
      m_running(false),
      m_port(8080),
      m_connection_backlog(5),
      m_reuse(1)
{
}

HttpServer::~HttpServer()
{
    stop();
}

bool HttpServer::start(int port, int connection_backlog, int reuse)
{
    if (m_running.load())
    {
        std::cerr << "Server is already running\n";
        return false;
    }

    m_port = port;
    m_connection_backlog = connection_backlog;
    m_reuse = reuse;

    try
    {
        m_server_thread = std::make_unique<std::thread>(&HttpServer::server_loop, this);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to start server thread: " << e.what() << "\n";
        return false;
    }
}

void HttpServer::stop()
{
    if (!m_running.load())
    {
        return;
    }

    m_running.store(false);

    // Close the server socket to unblock accept()
    if (m_server_file_descriptor >= 0)
    {
        close(m_server_file_descriptor);
        m_server_file_descriptor = -1;
    }

    // Wait for the server thread to finish
    if (m_server_thread && m_server_thread->joinable())
    {
        m_server_thread->join();
        m_server_thread.reset();
    }

    std::cout << "Server stopped\n";
}

bool HttpServer::is_running() const
{
    return m_running.load();
}

void HttpServer::server_loop()
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    m_server_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (m_server_file_descriptor < 0)
    {
        std::cerr << "Failed to create server socket\n";
        return;
    }

    if (setsockopt(m_server_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &m_reuse, sizeof(m_reuse)) < 0)
    {
        std::cerr << "setsockopt failed\n";
        close(m_server_file_descriptor);
        return;
    }

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_addr.s_addr = INADDR_ANY;
    m_server_address.sin_port = htons(m_port);

    if (bind(m_server_file_descriptor, (struct sockaddr *)&m_server_address, sizeof(m_server_address)) != 0)
    {
        std::cerr << "Failed to bind to port " << m_port << "\n";
        close(m_server_file_descriptor);
        return;
    }

    if (listen(m_server_file_descriptor, m_connection_backlog) != 0)
    {
        std::cerr << "listen failed\n";
        close(m_server_file_descriptor);
        return;
    }

    m_running.store(true);
    std::cout << "Server started on port " << m_port << "\n";
    std::cout << "Waiting for clients to connect...\n";

    int client_address_length = sizeof(m_client_address);

    while (m_running.load())
    {
        m_client_socket = accept(m_server_file_descriptor, (struct sockaddr *)&m_client_address, (socklen_t *)&client_address_length);

        if (m_client_socket < 0)
        {
            if (m_running.load())
            {
                std::cerr << "Failed to accept connection\n";
            }
            continue;
        }

        if (!m_running.load())
        {
            close(m_client_socket);
            break;
        }

        std::cout << "Client connected\n";
        handle_client();
    }

    if (m_server_file_descriptor >= 0)
    {
        close(m_server_file_descriptor);
        m_server_file_descriptor = -1;
    }
}

int HttpServer::run(int port, int connection_backlog, int reuse)
{
    if (!start(port, connection_backlog, reuse))
    {
        return 1;
    }

    if (m_server_thread && m_server_thread->joinable())
    {
        m_server_thread->join();
        m_server_thread.reset();
    }

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
