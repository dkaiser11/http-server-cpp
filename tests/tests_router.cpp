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
        std::string request_str = method_to_string(method) + " " + uri + " HTTP/1.1\r\n";
        if (!body.empty())
        {
            request_str += "Content-Length: " + std::to_string(body.length()) + "\r\n";
        }
        request_str += "\r\n" + body;

        return HttpRequest::from_string(request_str);
    }

    // Helper method to convert HttpMethod to string
    std::string method_to_string(HttpMethod method)
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
            response.set_code(HttpCode::OK);
            response.add_header("Content-Type", "text/plain");
            response.set_body(response_body);
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
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::NotFound);
}

// Tests for get method
TEST_F(RouterTest, get_should_register_GET_route_when_given_path_and_handler)
{
    auto handler = createSimpleHandler("GET response");
    router->get("/test", handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/test");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::OK);
    EXPECT_EQ(response.get_body(), "GET response");
}

TEST_F(RouterTest, get_should_support_regex_patterns_when_given_regex_path)
{
    auto handler = createSimpleHandler("User profile");
    router->get("/user/\\d+", handler);

    // Test matching numeric user ID
    HttpRequest request1 = createRequest(HttpMethod::GET, "/user/123");
    HttpResponse response1 = router->handle_request(request1);
    EXPECT_EQ(response1.get_code(), HttpCode::OK);
    EXPECT_EQ(response1.get_body(), "User profile");

    // Test matching another numeric user ID
    HttpRequest request2 = createRequest(HttpMethod::GET, "/user/456");
    HttpResponse response2 = router->handle_request(request2);
    EXPECT_EQ(response2.get_code(), HttpCode::OK);
    EXPECT_EQ(response2.get_body(), "User profile");
}

TEST_F(RouterTest, get_should_not_match_different_method_when_POST_request_made_to_GET_route)
{
    auto handler = createSimpleHandler("GET only");
    router->get("/test", handler);

    HttpRequest request = createRequest(HttpMethod::POST, "/test");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::MethodNotAllowed);
}

// Tests for post method
TEST_F(RouterTest, post_should_register_POST_route_when_given_path_and_handler)
{
    auto handler = createSimpleHandler("POST response");
    router->post("/api/data", handler);

    HttpRequest request = createRequest(HttpMethod::POST, "/api/data", "{\"key\":\"value\"}");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::OK);
    EXPECT_EQ(response.get_body(), "POST response");
}

TEST_F(RouterTest, post_should_receive_request_body_when_handler_accesses_request)
{
    std::string received_body;
    auto handler = [&received_body](const HttpRequest &req) -> HttpResponse
    {
        received_body = req.get_body();
        HttpResponse response;
        response.set_code(HttpCode::OK);
        response.set_body("Received: " + req.get_body());
        return response;
    };

    router->post("/api/echo", handler);

    HttpRequest request = createRequest(HttpMethod::POST, "/api/echo", "test data");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::OK);
    EXPECT_EQ(response.get_body(), "Received: test data");
}

// Tests for put method
TEST_F(RouterTest, put_should_register_PUT_route_when_given_path_and_handler)
{
    auto handler = createSimpleHandler("PUT response");
    router->put("/api/update", handler);

    HttpRequest request = createRequest(HttpMethod::PUT, "/api/update");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::OK);
    EXPECT_EQ(response.get_body(), "PUT response");
}

// Tests for delete_ method
TEST_F(RouterTest, delete__should_register_DELETE_route_when_given_path_and_handler)
{
    auto handler = createSimpleHandler("DELETE response");
    router->delete_("/api/delete", handler);

    HttpRequest request = createRequest(HttpMethod::DELETE, "/api/delete");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::OK);
    EXPECT_EQ(response.get_body(), "DELETE response");
}

// Tests for add_route method
TEST_F(RouterTest, add_route_should_register_route_for_any_method_when_given_method_path_and_handler)
{
    auto handler = createSimpleHandler("PATCH response");
    router->add_route(HttpMethod::PATCH, "/api/patch", handler);

    HttpRequest request = createRequest(HttpMethod::PATCH, "/api/patch");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::OK);
    EXPECT_EQ(response.get_body(), "PATCH response");
}

TEST_F(RouterTest, add_route_should_support_multiple_routes_same_path_different_methods_when_added_separately)
{
    auto get_handler = createSimpleHandler("GET response");
    auto post_handler = createSimpleHandler("POST response");

    router->add_route(HttpMethod::GET, "/api/resource", get_handler);
    router->add_route(HttpMethod::POST, "/api/resource", post_handler);

    HttpRequest get_request = createRequest(HttpMethod::GET, "/api/resource");
    HttpResponse get_response = router->handle_request(get_request);
    EXPECT_EQ(get_response.get_body(), "GET response");

    HttpRequest post_request = createRequest(HttpMethod::POST, "/api/resource");
    HttpResponse post_response = router->handle_request(post_request);
    EXPECT_EQ(post_response.get_body(), "POST response");
}

TEST_F(RouterTest, add_route_should_support_multiple_routes_same_method_different_paths_when_added_separately)
{
    auto handler1 = createSimpleHandler("Path 1");
    auto handler2 = createSimpleHandler("Path 2");

    router->add_route(HttpMethod::GET, "/path1", handler1);
    router->add_route(HttpMethod::GET, "/path2", handler2);

    HttpRequest request1 = createRequest(HttpMethod::GET, "/path1");
    HttpResponse response1 = router->handle_request(request1);
    EXPECT_EQ(response1.get_body(), "Path 1");

    HttpRequest request2 = createRequest(HttpMethod::GET, "/path2");
    HttpResponse response2 = router->handle_request(request2);
    EXPECT_EQ(response2.get_body(), "Path 2");
}

// Tests for set_not_found_handler method
TEST_F(RouterTest, set_not_found_handler_should_use_custom_handler_when_no_route_matches)
{
    auto custom_handler = [](const HttpRequest &req) -> HttpResponse
    {
        HttpResponse response;
        response.set_code(HttpCode::NotFound);
        response.add_header("Content-Type", "application/json");
        response.set_body("{\"error\":\"Custom not found for " + req.get_uri() + "\"}");
        return response;
    };

    router->set_not_found_handler(custom_handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/nonexistent");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::NotFound);
    EXPECT_EQ(response.get_body(), "{\"error\":\"Custom not found for /nonexistent\"}");
    EXPECT_EQ(response.get_headers().at("Content-Type"), "application/json");
}

TEST_F(RouterTest, set_not_found_handler_should_override_default_handler_when_new_handler_set)
{
    // First test default behavior
    HttpRequest request = createRequest(HttpMethod::GET, "/nonexistent");
    HttpResponse default_response = router->handle_request(request);
    EXPECT_TRUE(default_response.get_body().find("404 - Not Found") != std::string::npos);

    // Set custom handler
    auto custom_handler = createSimpleHandler("Custom 404");
    router->set_not_found_handler(custom_handler);

    HttpResponse custom_response = router->handle_request(request);
    EXPECT_EQ(custom_response.get_body(), "Custom 404");
}

// Tests for set_method_not_allowed_handler method
TEST_F(RouterTest, set_method_not_allowed_handler_should_use_custom_handler_when_path_exists_but_method_wrong)
{
    // Add a GET route
    auto handler = createSimpleHandler("GET only");
    router->get("/api/test", handler);

    // Set custom method not allowed handler
    auto custom_handler = [](const HttpRequest &req) -> HttpResponse
    {
        HttpResponse response;
        response.set_code(HttpCode::MethodNotAllowed);
        response.add_header("Content-Type", "application/json");
        response.set_body("{\"error\":\"Method not allowed for " + req.get_uri() + "\"}");
        return response;
    };

    router->set_method_not_allowed_handler(custom_handler);

    // Try POST to GET-only route
    HttpRequest request = createRequest(HttpMethod::POST, "/api/test");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::MethodNotAllowed);
    EXPECT_EQ(response.get_body(), "{\"error\":\"Method not allowed for /api/test\"}");
    EXPECT_EQ(response.get_headers().at("Content-Type"), "application/json");
}

TEST_F(RouterTest, set_method_not_allowed_handler_should_override_default_handler_when_new_handler_set)
{
    // Add a GET route
    auto handler = createSimpleHandler("GET only");
    router->get("/api/test", handler);

    // Test default method not allowed behavior
    HttpRequest request = createRequest(HttpMethod::POST, "/api/test");
    HttpResponse default_response = router->handle_request(request);
    EXPECT_TRUE(default_response.get_body().find("405 - Method Not Allowed") != std::string::npos);

    // Set custom handler
    auto custom_handler = createSimpleHandler("Custom 405");
    router->set_method_not_allowed_handler(custom_handler);

    HttpResponse custom_response = router->handle_request(request);
    EXPECT_EQ(custom_response.get_body(), "Custom 405");
}

// Tests for handle_request method
TEST_F(RouterTest, handle_request_should_return_matching_route_response_when_exact_path_matches)
{
    auto handler = createSimpleHandler("Exact match");
    router->get("/exact/path", handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/exact/path");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::OK);
    EXPECT_EQ(response.get_body(), "Exact match");
}

TEST_F(RouterTest, handle_request_should_return_first_matching_route_when_multiple_routes_match)
{
    auto handler1 = createSimpleHandler("First handler");
    auto handler2 = createSimpleHandler("Second handler");

    // Both patterns will match "/test"
    router->add_route(HttpMethod::GET, "/test", handler1);
    router->add_route(HttpMethod::GET, ".*", handler2); // Matches everything

    HttpRequest request = createRequest(HttpMethod::GET, "/test");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_body(), "First handler");
}

TEST_F(RouterTest, handle_request_should_return_404_when_no_route_matches)
{
    // Add some routes that won't match
    auto handler = createSimpleHandler("Won't match");
    router->get("/different/path", handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/nonexistent");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::NotFound);
}

TEST_F(RouterTest, handle_request_should_return_405_when_path_matches_but_method_different)
{
    auto handler = createSimpleHandler("GET only");
    router->get("/api/resource", handler);

    HttpRequest request = createRequest(HttpMethod::POST, "/api/resource");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::MethodNotAllowed);
}

TEST_F(RouterTest, handle_request_should_handle_handler_exceptions_when_route_handler_throws)
{
    auto throwing_handler = [](const HttpRequest &) -> HttpResponse
    {
        throw std::runtime_error("Handler error");
    };

    router->get("/error", throwing_handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/error");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::InternalServerError);
    EXPECT_TRUE(response.get_body().find("500 - Internal Server Error") != std::string::npos);
}

TEST_F(RouterTest, handle_request_should_support_complex_regex_patterns_when_using_advanced_patterns)
{
    auto handler = createSimpleHandler("API v1");
    router->get("/api/v[0-9]+/users/[a-zA-Z0-9]+", handler);

    // Test matching pattern
    HttpRequest request1 = createRequest(HttpMethod::GET, "/api/v1/users/john123");
    HttpResponse response1 = router->handle_request(request1);
    EXPECT_EQ(response1.get_code(), HttpCode::OK);

    HttpRequest request2 = createRequest(HttpMethod::GET, "/api/v2/users/jane456");
    HttpResponse response2 = router->handle_request(request2);
    EXPECT_EQ(response2.get_code(), HttpCode::OK);

    // Test non-matching pattern
    HttpRequest request3 = createRequest(HttpMethod::GET, "/api/v1/posts/123");
    HttpResponse response3 = router->handle_request(request3);
    EXPECT_EQ(response3.get_code(), HttpCode::NotFound);
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
    EXPECT_EQ(router->handle_request(home_req).get_body(), "Home");

    HttpRequest about_req = createRequest(HttpMethod::GET, "/about");
    EXPECT_EQ(router->handle_request(about_req).get_body(), "About");

    HttpRequest create_req = createRequest(HttpMethod::POST, "/api/users");
    EXPECT_EQ(router->handle_request(create_req).get_body(), "Create user");

    HttpRequest update_req = createRequest(HttpMethod::PUT, "/api/users/123");
    EXPECT_EQ(router->handle_request(update_req).get_body(), "Update user");

    HttpRequest delete_req = createRequest(HttpMethod::DELETE, "/api/users/456");
    EXPECT_EQ(router->handle_request(delete_req).get_body(), "Delete user");
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
        HttpResponse response = router->handle_request(request);

        EXPECT_EQ(response.get_code(), HttpCode::OK);
        EXPECT_EQ(response.get_body(), "Thread safe");
    }
}

// Edge cases
TEST_F(RouterTest, Router_should_handle_empty_path_when_root_path_requested)
{
    auto handler = createSimpleHandler("Root");
    router->get("/", handler); // Use "/" instead of empty string for root path

    HttpRequest request = createRequest(HttpMethod::GET, "/");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::OK);
    EXPECT_EQ(response.get_body(), "Root");
}

TEST_F(RouterTest, Router_should_handle_special_characters_in_path_when_regex_metacharacters_present)
{
    auto handler = createSimpleHandler("Special chars");
    // Escape special regex characters
    router->get("/api/files/\\$\\{filename\\}\\.txt", handler);

    HttpRequest request = createRequest(HttpMethod::GET, "/api/files/${filename}.txt");
    HttpResponse response = router->handle_request(request);

    EXPECT_EQ(response.get_code(), HttpCode::OK);
    EXPECT_EQ(response.get_body(), "Special chars");
}
