#include "server/httpserver.hpp"
#include <thread>
#include <atomic>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <mutex>
#include <netinet/in.h>
#include <fstream>
#include <sstream>
#include <filesystem>

HttpServer::HttpServer()
    : m_server_file_descriptor(-1),
      m_server_address{}
{
}

HttpServer::HttpServer(const Router &router)
    : m_server_file_descriptor(-1),
      m_server_address{},
      m_router(router)
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
            {
                std::lock_guard<std::mutex> lock(m_output_mutex);
                std::cerr << "Failed to accept connection: " << strerror(errno) << "\n";
            }
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(m_output_mutex);
            std::cout << "Client connected\n";
        }

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
        {
            std::lock_guard<std::mutex> lock(m_output_mutex);
            std::cerr << "Failed to receive request: " << strerror(errno) << "\n";
        }
        close(client_file_descriptor);
        return;
    }

    // Use router to handle the request and generate response
    HttpResponse response = m_router.handleRequest(request);

    if (send_response(client_file_descriptor, response) < 0)
    {
        std::lock_guard<std::mutex> lock(m_output_mutex);
        std::cerr << "Failed to send response to client\n";
    }
    else
    {
        std::lock_guard<std::mutex> lock(m_output_mutex);
        std::cout << "Request processed: " << static_cast<int>(request.getMethod())
                  << " " << request.getUri() << " -> "
                  << static_cast<int>(response.getCode()) << "\n";
    }

    close(client_file_descriptor);
}

int HttpServer::receive_request(int client_file_descriptor, HttpRequest &request)
{
    char buffer[4096];
    int bytes_received = recv(client_file_descriptor, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received <= 0)
    {
        std::lock_guard<std::mutex> lock(m_output_mutex);
        if (bytes_received == 0)
            std::cout << "Client disconnected\n";
        else
            std::cerr << "Failed to receive request: " << strerror(errno) << "\n";

        return bytes_received;
    }

    buffer[bytes_received] = '\0';

    {
        std::lock_guard<std::mutex> lock(m_output_mutex);
        std::cout << "Received request:\n"
                  << buffer << "\n";
    }

    try
    {
        request = HttpRequest::fromString(std::string(buffer, bytes_received));
    }
    catch (const std::exception &e)
    {
        std::lock_guard<std::mutex> lock(m_output_mutex);
        std::cerr << "Failed to parse HTTP request: " << e.what() << "\n";
        return -1;
    }

    return bytes_received;
}

int HttpServer::send_response(int client_file_descriptor, HttpResponse &response)
{
    std::string response_str = response.toString();
    int bytes_sent = send(client_file_descriptor, response_str.c_str(), response_str.size(), 0);

    if (bytes_sent < 0)
    {
        std::lock_guard<std::mutex> lock(m_output_mutex);
        std::cerr << "Failed to send response: " << strerror(errno) << "\n";
    }

    return bytes_sent;
}

HttpResponse HttpServer::serve_static_file(const std::string &file_path, const std::string &web_root)
{
    HttpResponse response;

    // Construct full path
    std::filesystem::path full_path = std::filesystem::path(web_root) / file_path;

    // Security: prevent directory traversal
    std::filesystem::path canonical_web_root;
    std::filesystem::path canonical_full_path;

    try
    {
        // Check if web_root exists, if not create canonical path anyway
        if (std::filesystem::exists(web_root))
        {
            canonical_web_root = std::filesystem::canonical(web_root);
        }
        else
        {
            canonical_web_root = std::filesystem::absolute(web_root);
        }

        // Try to get canonical path for the file
        if (std::filesystem::exists(full_path))
        {
            canonical_full_path = std::filesystem::canonical(full_path);
        }
        else
        {
            canonical_full_path = std::filesystem::absolute(full_path);
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        response.setCode(HttpCode::NotFound);
        response.setBody("<html><body><h1>404 Not Found</h1></body></html>");
        response.addHeader("Content-Type", "text/html");
        return response;
    }

    // Check if file is within web root (security)
    auto relative_path = std::filesystem::relative(canonical_full_path, canonical_web_root);
    if (relative_path.string().find("..") == 0)
    {
        response.setCode(HttpCode::Forbidden);
        response.setBody("<html><body><h1>403 Forbidden</h1></body></html>");
        response.addHeader("Content-Type", "text/html");
        return response;
    }

    // Check if file exists and is a regular file
    if (!std::filesystem::exists(canonical_full_path) || !std::filesystem::is_regular_file(canonical_full_path))
    {
        response.setCode(HttpCode::NotFound);
        response.setBody("<html><body><h1>404 Not Found</h1><p>" + canonical_full_path.string() + "</p></body></html>");
        response.addHeader("Content-Type", "text/html");
        return response;
    }

    // Read file
    std::ifstream file(canonical_full_path, std::ios::binary);
    if (!file.is_open())
    {
        response.setCode(HttpCode::InternalServerError);
        response.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
        response.addHeader("Content-Type", "text/html");
        return response;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Set content type based on file extension
    std::string extension = canonical_full_path.extension().string();
    std::string content_type = "text/plain";

    if (extension == ".html" || extension == ".htm")
    {
        content_type = "text/html";
    }
    else if (extension == ".css")
    {
        content_type = "text/css";
    }
    else if (extension == ".js")
    {
        content_type = "application/javascript";
    }
    else if (extension == ".json")
    {
        content_type = "application/json";
    }
    else if (extension == ".png")
    {
        content_type = "image/png";
    }
    else if (extension == ".jpg" || extension == ".jpeg")
    {
        content_type = "image/jpeg";
    }
    else if (extension == ".gif")
    {
        content_type = "image/gif";
    }
    else if (extension == ".svg")
    {
        content_type = "image/svg+xml";
    }
    else if (extension == ".ico")
    {
        content_type = "image/x-icon";
    }

    response.setCode(HttpCode::OK);
    response.setBody(content);
    response.addHeader("Content-Type", content_type);
    response.addHeader("Content-Length", std::to_string(content.length()));

    return response;
}
