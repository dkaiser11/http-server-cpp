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
#include <functional>

HttpServer::HttpServer()
    : m_server_socket(),
      m_server_address{}
{
    init_thread_pool();
}

HttpServer::HttpServer(const Router &router)
    : m_server_socket(),
      m_server_address{},
      m_router(router)
{
    init_thread_pool();
}

HttpServer::~HttpServer()
{
    shutdown_thread_pool();
    std::cout << "Server socket closed\n";
}

void HttpServer::set_router(const Router &router)
{
    m_router = router;
}

const Router &HttpServer::get_router() const
{
    return m_router;
}

int HttpServer::run(int port, int connection_backlog, int reuse)
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    m_server_socket.reset(socket(AF_INET, SOCK_STREAM, 0));

    if (!m_server_socket.is_valid())
    {
        std::cerr << "Failed to create server socket: " << strerror(errno) << "\n";
        return 1;
    }

    if (setsockopt(m_server_socket.get(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        std::cerr << "setsockopt failed: " << strerror(errno) << "\n";
        return 1;
    }

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_addr.s_addr = INADDR_ANY;
    m_server_address.sin_port = htons(port);

    if (bind(m_server_socket.get(), (struct sockaddr *)&m_server_address, sizeof(m_server_address)) != 0)
    {
        std::cerr << "Failed to bind to port " << port << ": " << strerror(errno) << "\n";
        return 1;
    }

    if (listen(m_server_socket.get(), connection_backlog) != 0)
    {
        std::cerr << "listen failed: " << strerror(errno) << "\n";
        return 1;
    }

    struct sockaddr_in client_address;
    int client_address_length = sizeof(client_address);

    std::cout << "Waiting for a client to connect...\n";

    std::atomic<bool> running(true);

    while (running)
    {
        int client_fd = accept(m_server_socket.get(), (struct sockaddr *)&client_address, (socklen_t *)&client_address_length);

        if (client_fd < 0)
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

        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_task_queue.emplace(std::bind(&HttpServer::handle_client_fd, this, client_fd));
        }
        m_condition.notify_one();
    }

    return 0;
}

void HttpServer::handle_client(SocketWrapper client_socket)
{
    HttpRequest request;

    if (receive_request(client_socket, request) < 0)
    {
        {
            std::lock_guard<std::mutex> lock(m_output_mutex);
            std::cerr << "Failed to receive request: " << strerror(errno) << "\n";
        }
        return;
    }

    HttpResponse response = m_router.handle_request(request);

    if (send_response(client_socket, response) < 0)
    {
        std::lock_guard<std::mutex> lock(m_output_mutex);
        std::cerr << "Failed to send response to client\n";
    }
    else
    {
        std::lock_guard<std::mutex> lock(m_output_mutex);
        std::cout << "Request processed: " << static_cast<int>(request.get_method())
                  << " " << request.get_uri() << " -> "
                  << static_cast<int>(response.get_code()) << "\n";
    }
}

void HttpServer::handle_client_fd(int client_fd)
{
    SocketWrapper client_socket(client_fd);
    handle_client(std::move(client_socket));
}

int HttpServer::receive_request(const SocketWrapper &client_socket, HttpRequest &request)
{
    char buffer[4096];
    int bytes_received = recv(client_socket.get(), buffer, sizeof(buffer) - 1, 0);

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
        request = HttpRequest::from_string(std::string(buffer, bytes_received));
    }
    catch (const std::exception &e)
    {
        std::lock_guard<std::mutex> lock(m_output_mutex);
        std::cerr << "Failed to parse HTTP request: " << e.what() << "\n";
        return -1;
    }

    return bytes_received;
}

int HttpServer::send_response(const SocketWrapper &client_socket, HttpResponse &response)
{
    std::string response_str = response.to_string();
    int bytes_sent = send(client_socket.get(), response_str.c_str(), response_str.size(), 0);

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

    std::filesystem::path full_path = std::filesystem::path(web_root) / file_path;

    std::filesystem::path canonical_web_root;
    std::filesystem::path canonical_full_path;

    try
    {
        if (std::filesystem::exists(web_root))
        {
            canonical_web_root = std::filesystem::canonical(web_root);
        }
        else
        {
            canonical_web_root = std::filesystem::absolute(web_root);
        }

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
        response.set_code(HttpCode::NotFound);
        response.set_body("<html><body><h1>404 Not Found</h1></body></html>");
        response.add_header("Content-Type", "text/html");
        return response;
    }

    auto relative_path = std::filesystem::relative(canonical_full_path, canonical_web_root);
    if (relative_path.string().find("..") == 0)
    {
        response.set_code(HttpCode::Forbidden);
        response.set_body("<html><body><h1>403 Forbidden</h1></body></html>");
        response.add_header("Content-Type", "text/html");
        return response;
    }

    if (!std::filesystem::exists(canonical_full_path) || !std::filesystem::is_regular_file(canonical_full_path))
    {
        response.set_code(HttpCode::NotFound);
        response.set_body("<html><body><h1>404 Not Found</h1><p>" + canonical_full_path.string() + "</p></body></html>");
        response.add_header("Content-Type", "text/html");
        return response;
    }

    std::ifstream file(canonical_full_path, std::ios::binary);
    if (!file.is_open())
    {
        response.set_code(HttpCode::InternalServerError);
        response.set_body("<html><body><h1>500 Internal Server Error</h1></body></html>");
        response.add_header("Content-Type", "text/html");
        return response;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

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

    response.set_code(HttpCode::OK);
    response.set_body(content);
    response.add_header("Content-Type", content_type);
    response.add_header("Content-Length", std::to_string(content.length()));

    return response;
}

void HttpServer::init_thread_pool(size_t num_threads)
{
    for (size_t i = 0; i < num_threads; ++i)
    {
        m_worker_threads.emplace_back(&HttpServer::worker_thread, this);
    }
}

void HttpServer::worker_thread()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_condition.wait(lock, [this]
                             { return m_stop_threads || !m_task_queue.empty(); });

            if (m_stop_threads && m_task_queue.empty())
                break;

            task = std::move(m_task_queue.front());
            m_task_queue.pop();
        }
        task();
    }
}

void HttpServer::shutdown_thread_pool()
{
    m_stop_threads = true;
    m_condition.notify_all();

    for (auto &thread : m_worker_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    m_worker_threads.clear();

    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        while (!m_task_queue.empty())
        {
            m_task_queue.pop();
        }
    }
}
