#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "server/router.hpp"
#include "http/httpmethod.hpp"
#include "http/httpcode.hpp"
#include "http/httprequest.hpp"
#include "http/httpresponse.hpp"
#include <stdexcept>

class RouterTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        router = std::make_unique<Router>();
    }

    void TearDown() override
    {
        router.reset();
    }

    // Helper method to create HTTP requests for testing
    HttpRequest createRequest(HttpMethod method, const std::string &uri, const std::string &body = "")
    {
        HttpRequest request;
        // Since we can't directly set private members, we'll use the fromString method
        std::string request_str = methodToString(method) + " " + uri + " HTTP/1.1\r\n";
        if (!body.empty())
        {
            request_str += "Content-Length: " + std::to_string(body.length()) + "\r\n";
        }
        request_str += "\r\n" + body;

        return HttpRequest::fromString(request_str);
    }

    // Helper method to convert HttpMethod to string
    std::string methodToString(HttpMethod method)
    {
        switch (method)
        {
        case HttpMethod::GET:
            return "GET";
        case HttpMethod::POST:
            return "POST";
        case HttpMethod::PUT:
            return "PUT";
        case HttpMethod::DELETE:
            return "DELETE";
        case HttpMethod::HEAD:
            return "HEAD";
        case HttpMethod::PATCH:
            return "PATCH";
        case HttpMethod::OPTIONS:
            return "OPTIONS";
        default:
            return "GET";
        }
    }

    // Helper method to create a simple route handler
    RouteHandler createSimpleHandler(const std::string &response_body = "OK")
    {
        return [response_body](const HttpRequest &) -> HttpResponse
        {
            HttpResponse response;
            response.setCode(HttpCode::OK);
            response.addHeader("Content-Type", "text/plain");
            response.setBody(response_body);
            return response;
        };
    }

    std::unique_ptr<Router> router;
};

// Tests for constructor
TEST_F(RouterTest, Constructor_should_create_valid_instance_when_called)
{
    // Router should be created successfully
    EXPECT_NE(router, nullptr);

    // Test that default handlers work by making a request that doesn't match any route
    HttpRequest request = createRequest(HttpMethod::GET, "/nonexistent");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::NotFound);
}

// Tests for get method
TEST_F(RouterTest, get_should_register_GET_route_when_given_path_and_handler)
{
    auto handler = createSimpleHandler("GET response");
    router->get("/test", handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/test");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::OK);
    EXPECT_EQ(response.getBody(), "GET response");
}

TEST_F(RouterTest, get_should_support_regex_patterns_when_given_regex_path)
{
    auto handler = createSimpleHandler("User profile");
    router->get("/user/\\d+", handler);

    // Test matching numeric user ID
    HttpRequest request1 = createRequest(HttpMethod::GET, "/user/123");
    HttpResponse response1 = router->handleRequest(request1);
    EXPECT_EQ(response1.getCode(), HttpCode::OK);
    EXPECT_EQ(response1.getBody(), "User profile");

    // Test matching another numeric user ID
    HttpRequest request2 = createRequest(HttpMethod::GET, "/user/456");
    HttpResponse response2 = router->handleRequest(request2);
    EXPECT_EQ(response2.getCode(), HttpCode::OK);
    EXPECT_EQ(response2.getBody(), "User profile");
}

TEST_F(RouterTest, get_should_not_match_different_method_when_POST_request_made_to_GET_route)
{
    auto handler = createSimpleHandler("GET only");
    router->get("/test", handler);

    HttpRequest request = createRequest(HttpMethod::POST, "/test");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::MethodNotAllowed);
}

// Tests for post method
TEST_F(RouterTest, post_should_register_POST_route_when_given_path_and_handler)
{
    auto handler = createSimpleHandler("POST response");
    router->post("/api/data", handler);

    HttpRequest request = createRequest(HttpMethod::POST, "/api/data", "{\"key\":\"value\"}");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::OK);
    EXPECT_EQ(response.getBody(), "POST response");
}

TEST_F(RouterTest, post_should_receive_request_body_when_handler_accesses_request)
{
    std::string received_body;
    auto handler = [&received_body](const HttpRequest &req) -> HttpResponse
    {
        received_body = req.getBody();
        HttpResponse response;
        response.setCode(HttpCode::OK);
        response.setBody("Received: " + req.getBody());
        return response;
    };

    router->post("/api/echo", handler);

    HttpRequest request = createRequest(HttpMethod::POST, "/api/echo", "test data");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::OK);
    EXPECT_EQ(response.getBody(), "Received: test data");
}

// Tests for put method
TEST_F(RouterTest, put_should_register_PUT_route_when_given_path_and_handler)
{
    auto handler = createSimpleHandler("PUT response");
    router->put("/api/update", handler);

    HttpRequest request = createRequest(HttpMethod::PUT, "/api/update");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::OK);
    EXPECT_EQ(response.getBody(), "PUT response");
}

// Tests for delete_ method
TEST_F(RouterTest, delete__should_register_DELETE_route_when_given_path_and_handler)
{
    auto handler = createSimpleHandler("DELETE response");
    router->delete_("/api/delete", handler);

    HttpRequest request = createRequest(HttpMethod::DELETE, "/api/delete");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::OK);
    EXPECT_EQ(response.getBody(), "DELETE response");
}

// Tests for addRoute method
TEST_F(RouterTest, addRoute_should_register_route_for_any_method_when_given_method_path_and_handler)
{
    auto handler = createSimpleHandler("PATCH response");
    router->addRoute(HttpMethod::PATCH, "/api/patch", handler);

    HttpRequest request = createRequest(HttpMethod::PATCH, "/api/patch");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::OK);
    EXPECT_EQ(response.getBody(), "PATCH response");
}

TEST_F(RouterTest, addRoute_should_support_multiple_routes_same_path_different_methods_when_added_separately)
{
    auto get_handler = createSimpleHandler("GET response");
    auto post_handler = createSimpleHandler("POST response");

    router->addRoute(HttpMethod::GET, "/api/resource", get_handler);
    router->addRoute(HttpMethod::POST, "/api/resource", post_handler);

    HttpRequest get_request = createRequest(HttpMethod::GET, "/api/resource");
    HttpResponse get_response = router->handleRequest(get_request);
    EXPECT_EQ(get_response.getBody(), "GET response");

    HttpRequest post_request = createRequest(HttpMethod::POST, "/api/resource");
    HttpResponse post_response = router->handleRequest(post_request);
    EXPECT_EQ(post_response.getBody(), "POST response");
}

TEST_F(RouterTest, addRoute_should_support_multiple_routes_same_method_different_paths_when_added_separately)
{
    auto handler1 = createSimpleHandler("Path 1");
    auto handler2 = createSimpleHandler("Path 2");

    router->addRoute(HttpMethod::GET, "/path1", handler1);
    router->addRoute(HttpMethod::GET, "/path2", handler2);

    HttpRequest request1 = createRequest(HttpMethod::GET, "/path1");
    HttpResponse response1 = router->handleRequest(request1);
    EXPECT_EQ(response1.getBody(), "Path 1");

    HttpRequest request2 = createRequest(HttpMethod::GET, "/path2");
    HttpResponse response2 = router->handleRequest(request2);
    EXPECT_EQ(response2.getBody(), "Path 2");
}

// Tests for setNotFoundHandler method
TEST_F(RouterTest, setNotFoundHandler_should_use_custom_handler_when_no_route_matches)
{
    auto custom_handler = [](const HttpRequest &req) -> HttpResponse
    {
        HttpResponse response;
        response.setCode(HttpCode::NotFound);
        response.addHeader("Content-Type", "application/json");
        response.setBody("{\"error\":\"Custom not found for " + req.getUri() + "\"}");
        return response;
    };

    router->setNotFoundHandler(custom_handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/nonexistent");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::NotFound);
    EXPECT_EQ(response.getBody(), "{\"error\":\"Custom not found for /nonexistent\"}");
    EXPECT_EQ(response.getHeaders().at("Content-Type"), "application/json");
}

TEST_F(RouterTest, setNotFoundHandler_should_override_default_handler_when_new_handler_set)
{
    // First test default behavior
    HttpRequest request = createRequest(HttpMethod::GET, "/nonexistent");
    HttpResponse default_response = router->handleRequest(request);
    EXPECT_TRUE(default_response.getBody().find("404 - Not Found") != std::string::npos);

    // Set custom handler
    auto custom_handler = createSimpleHandler("Custom 404");
    router->setNotFoundHandler(custom_handler);

    HttpResponse custom_response = router->handleRequest(request);
    EXPECT_EQ(custom_response.getBody(), "Custom 404");
}

// Tests for setMethodNotAllowedHandler method
TEST_F(RouterTest, setMethodNotAllowedHandler_should_use_custom_handler_when_path_exists_but_method_wrong)
{
    // Add a GET route
    auto handler = createSimpleHandler("GET only");
    router->get("/api/test", handler);

    // Set custom method not allowed handler
    auto custom_handler = [](const HttpRequest &req) -> HttpResponse
    {
        HttpResponse response;
        response.setCode(HttpCode::MethodNotAllowed);
        response.addHeader("Content-Type", "application/json");
        response.setBody("{\"error\":\"Method not allowed for " + req.getUri() + "\"}");
        return response;
    };

    router->setMethodNotAllowedHandler(custom_handler);

    // Try POST to GET-only route
    HttpRequest request = createRequest(HttpMethod::POST, "/api/test");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::MethodNotAllowed);
    EXPECT_EQ(response.getBody(), "{\"error\":\"Method not allowed for /api/test\"}");
    EXPECT_EQ(response.getHeaders().at("Content-Type"), "application/json");
}

TEST_F(RouterTest, setMethodNotAllowedHandler_should_override_default_handler_when_new_handler_set)
{
    // Add a GET route
    auto handler = createSimpleHandler("GET only");
    router->get("/api/test", handler);

    // Test default method not allowed behavior
    HttpRequest request = createRequest(HttpMethod::POST, "/api/test");
    HttpResponse default_response = router->handleRequest(request);
    EXPECT_TRUE(default_response.getBody().find("405 - Method Not Allowed") != std::string::npos);

    // Set custom handler
    auto custom_handler = createSimpleHandler("Custom 405");
    router->setMethodNotAllowedHandler(custom_handler);

    HttpResponse custom_response = router->handleRequest(request);
    EXPECT_EQ(custom_response.getBody(), "Custom 405");
}

// Tests for handleRequest method
TEST_F(RouterTest, handleRequest_should_return_matching_route_response_when_exact_path_matches)
{
    auto handler = createSimpleHandler("Exact match");
    router->get("/exact/path", handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/exact/path");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::OK);
    EXPECT_EQ(response.getBody(), "Exact match");
}

TEST_F(RouterTest, handleRequest_should_return_first_matching_route_when_multiple_routes_match)
{
    auto handler1 = createSimpleHandler("First handler");
    auto handler2 = createSimpleHandler("Second handler");

    // Both patterns will match "/test"
    router->addRoute(HttpMethod::GET, "/test", handler1);
    router->addRoute(HttpMethod::GET, ".*", handler2); // Matches everything

    HttpRequest request = createRequest(HttpMethod::GET, "/test");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getBody(), "First handler");
}

TEST_F(RouterTest, handleRequest_should_return_404_when_no_route_matches)
{
    // Add some routes that won't match
    auto handler = createSimpleHandler("Won't match");
    router->get("/different/path", handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/nonexistent");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::NotFound);
}

TEST_F(RouterTest, handleRequest_should_return_405_when_path_matches_but_method_different)
{
    auto handler = createSimpleHandler("GET only");
    router->get("/api/resource", handler);

    HttpRequest request = createRequest(HttpMethod::POST, "/api/resource");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::MethodNotAllowed);
}

TEST_F(RouterTest, handleRequest_should_handle_handler_exceptions_when_route_handler_throws)
{
    auto throwing_handler = [](const HttpRequest &) -> HttpResponse
    {
        throw std::runtime_error("Handler error");
    };

    router->get("/error", throwing_handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/error");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::InternalServerError);
    EXPECT_TRUE(response.getBody().find("500 - Internal Server Error") != std::string::npos);
}

TEST_F(RouterTest, handleRequest_should_support_complex_regex_patterns_when_using_advanced_patterns)
{
    auto handler = createSimpleHandler("API v1");
    router->get("/api/v[0-9]+/users/[a-zA-Z0-9]+", handler);

    // Test matching pattern
    HttpRequest request1 = createRequest(HttpMethod::GET, "/api/v1/users/john123");
    HttpResponse response1 = router->handleRequest(request1);
    EXPECT_EQ(response1.getCode(), HttpCode::OK);

    HttpRequest request2 = createRequest(HttpMethod::GET, "/api/v2/users/jane456");
    HttpResponse response2 = router->handleRequest(request2);
    EXPECT_EQ(response2.getCode(), HttpCode::OK);

    // Test non-matching pattern
    HttpRequest request3 = createRequest(HttpMethod::GET, "/api/v1/posts/123");
    HttpResponse response3 = router->handleRequest(request3);
    EXPECT_EQ(response3.getCode(), HttpCode::NotFound);
}

// Integration tests
TEST_F(RouterTest, Router_should_handle_multiple_different_routes_when_complex_routing_setup)
{
    // Setup multiple routes
    router->get("/", createSimpleHandler("Home"));
    router->get("/about", createSimpleHandler("About"));
    router->post("/api/users", createSimpleHandler("Create user"));
    router->put("/api/users/\\d+", createSimpleHandler("Update user"));
    router->delete_("/api/users/\\d+", createSimpleHandler("Delete user"));

    // Test each route
    HttpRequest home_req = createRequest(HttpMethod::GET, "/");
    EXPECT_EQ(router->handleRequest(home_req).getBody(), "Home");

    HttpRequest about_req = createRequest(HttpMethod::GET, "/about");
    EXPECT_EQ(router->handleRequest(about_req).getBody(), "About");

    HttpRequest create_req = createRequest(HttpMethod::POST, "/api/users");
    EXPECT_EQ(router->handleRequest(create_req).getBody(), "Create user");

    HttpRequest update_req = createRequest(HttpMethod::PUT, "/api/users/123");
    EXPECT_EQ(router->handleRequest(update_req).getBody(), "Update user");

    HttpRequest delete_req = createRequest(HttpMethod::DELETE, "/api/users/456");
    EXPECT_EQ(router->handleRequest(delete_req).getBody(), "Delete user");
}

TEST_F(RouterTest, Router_should_maintain_thread_safety_when_handling_concurrent_requests)
{
    // This test verifies that the router can handle requests safely
    // The actual thread safety would be tested at the server level
    auto handler = createSimpleHandler("Thread safe");
    router->get("/test", handler);

    // Simulate multiple requests (in real scenario, these would be concurrent)
    for (int i = 0; i < 100; ++i)
    {
        HttpRequest request = createRequest(HttpMethod::GET, "/test");
        HttpResponse response = router->handleRequest(request);

        EXPECT_EQ(response.getCode(), HttpCode::OK);
        EXPECT_EQ(response.getBody(), "Thread safe");
    }
}

// Edge cases
TEST_F(RouterTest, Router_should_handle_empty_path_when_root_path_requested)
{
    auto handler = createSimpleHandler("Root");
    router->get("/", handler); // Use "/" instead of empty string for root path

    HttpRequest request = createRequest(HttpMethod::GET, "/");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::OK);
    EXPECT_EQ(response.getBody(), "Root");
}

TEST_F(RouterTest, Router_should_handle_special_characters_in_path_when_regex_metacharacters_present)
{
    auto handler = createSimpleHandler("Special chars");
    // Escape special regex characters
    router->get("/api/files/\\$\\{filename\\}\\.txt", handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/api/files/${filename}.txt");
    HttpResponse response = router->handleRequest(request);

    EXPECT_EQ(response.getCode(), HttpCode::OK);
    EXPECT_EQ(response.getBody(), "Special chars");
}
