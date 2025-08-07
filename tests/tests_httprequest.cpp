#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "http/httprequest.hpp"
#include "http/httpmethod.hpp"
#include <stdexcept>

class HttpRequestTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

// Tests for default constructor
TEST_F(HttpRequestTest, DefaultConstructor_should_create_valid_instance_when_called)
{
    HttpRequest request;

    // Default constructor should create a valid object
    // We can't directly test private members without getters, but we can test that the object is created
    EXPECT_NO_THROW(HttpRequest request2);
}

// Tests for copy constructor
TEST_F(HttpRequestTest, CopyConstructor_should_create_identical_copy_when_given_valid_request)
{
    // Create original request
    std::string validRequest = "GET /test HTTP/1.1\r\nHost: example.com\r\nContent-Length: 4\r\n\r\nbody";
    HttpRequest original = HttpRequest::from_string(validRequest);

    // Create copy
    HttpRequest copy(original);

    // Verify copy is identical
    EXPECT_EQ(original.get_method(), copy.get_method());
    EXPECT_EQ(original.get_uri(), copy.get_uri());
    EXPECT_EQ(original.get_version(), copy.get_version());
    EXPECT_EQ(original.get_headers(), copy.get_headers());
    EXPECT_EQ(original.get_body(), copy.get_body());
}

// Tests for copy assignment operator
TEST_F(HttpRequestTest, CopyAssignment_should_assign_values_correctly_when_given_valid_request)
{
    // Create original request
    std::string validRequest = "POST /api/test HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/json\r\n\r\n{\"test\":\"data\"}";
    HttpRequest original = HttpRequest::from_string(validRequest);

    // Create target request and assign
    HttpRequest target;
    target = original;

    // Verify assignment worked correctly
    EXPECT_EQ(original.get_method(), target.get_method());
    EXPECT_EQ(original.get_uri(), target.get_uri());
    EXPECT_EQ(original.get_version(), target.get_version());
    EXPECT_EQ(original.get_headers(), target.get_headers());
    EXPECT_EQ(original.get_body(), target.get_body());
}

// Tests for move constructor
TEST_F(HttpRequestTest, MoveConstructor_should_transfer_ownership_when_given_rvalue_request)
{
    // Create request and capture its state
    std::string validRequest = "PUT /resource HTTP/1.1\r\nHost: example.com\r\n\r\ntest data";
    HttpRequest original = HttpRequest::from_string(validRequest);
    HttpMethod expectedMethod = original.get_method();
    std::string expectedUri = original.get_uri();
    std::string expectedVersion = original.get_version();
    auto expectedHeaders = original.get_headers();
    std::string expectedBody = original.get_body();

    // Move construct
    HttpRequest moved(std::move(original));

    // Verify moved object has correct values
    EXPECT_EQ(expectedMethod, moved.get_method());
    EXPECT_EQ(expectedUri, moved.get_uri());
    EXPECT_EQ(expectedVersion, moved.get_version());
    EXPECT_EQ(expectedHeaders, moved.get_headers());
    EXPECT_EQ(expectedBody, moved.get_body());
}

// Tests for move assignment operator
TEST_F(HttpRequestTest, MoveAssignment_should_transfer_ownership_when_given_rvalue_request)
{
    // Create request and capture its state
    std::string validRequest = "DELETE /resource/123 HTTP/1.1\r\nHost: example.com\r\nAuthorization: Bearer token\r\n\r\n";
    HttpRequest original = HttpRequest::from_string(validRequest);
    HttpMethod expectedMethod = original.get_method();
    std::string expectedUri = original.get_uri();
    std::string expectedVersion = original.get_version();
    auto expectedHeaders = original.get_headers();
    std::string expectedBody = original.get_body();

    // Move assign
    HttpRequest target;
    target = std::move(original);

    // Verify target has correct values
    EXPECT_EQ(expectedMethod, target.get_method());
    EXPECT_EQ(expectedUri, target.get_uri());
    EXPECT_EQ(expectedVersion, target.get_version());
    EXPECT_EQ(expectedHeaders, target.get_headers());
    EXPECT_EQ(expectedBody, target.get_body());
}

// Tests for from_string method - valid requests
TEST_F(HttpRequestTest, from_string_should_parse_GET_request_when_given_valid_GET_string)
{
    std::string requestString = "GET /path HTTP/1.1\r\nHost: example.com\r\n\r\n";

    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ(HttpMethod::GET, request.get_method());
    EXPECT_EQ("/path", request.get_uri());
    EXPECT_EQ("HTTP/1.1", request.get_version());
    EXPECT_EQ("example.com", request.get_headers().at("Host"));
    EXPECT_EQ("", request.get_body());
}

TEST_F(HttpRequestTest, from_string_should_parse_POST_request_when_given_valid_POST_string)
{
    std::string requestString = "POST /api/users HTTP/1.1\r\nHost: api.example.com\r\nContent-Type: application/json\r\nContent-Length: 25\r\n\r\n{\"name\":\"John\",\"age\":30}";

    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ(HttpMethod::POST, request.get_method());
    EXPECT_EQ("/api/users", request.get_uri());
    EXPECT_EQ("HTTP/1.1", request.get_version());
    EXPECT_EQ("api.example.com", request.get_headers().at("Host"));
    EXPECT_EQ("application/json", request.get_headers().at("Content-Type"));
    EXPECT_EQ("25", request.get_headers().at("Content-Length"));
    EXPECT_EQ("{\"name\":\"John\",\"age\":30}", request.get_body());
}

TEST_F(HttpRequestTest, from_string_should_parse_PUT_request_when_given_valid_PUT_string)
{
    std::string requestString = "PUT /api/users/123 HTTP/1.1\r\nHost: api.example.com\r\nContent-Type: application/json\r\n\r\n{\"name\":\"Jane\"}";

    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ(HttpMethod::PUT, request.get_method());
    EXPECT_EQ("/api/users/123", request.get_uri());
    EXPECT_EQ("HTTP/1.1", request.get_version());
    EXPECT_EQ("api.example.com", request.get_headers().at("Host"));
    EXPECT_EQ("application/json", request.get_headers().at("Content-Type"));
    EXPECT_EQ("{\"name\":\"Jane\"}", request.get_body());
}

TEST_F(HttpRequestTest, from_string_should_parse_DELETE_request_when_given_valid_DELETE_string)
{
    std::string requestString = "DELETE /api/users/123 HTTP/1.1\r\nHost: api.example.com\r\nAuthorization: Bearer abc123\r\n\r\n";

    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ(HttpMethod::DELETE, request.get_method());
    EXPECT_EQ("/api/users/123", request.get_uri());
    EXPECT_EQ("HTTP/1.1", request.get_version());
    EXPECT_EQ("api.example.com", request.get_headers().at("Host"));
    EXPECT_EQ("Bearer abc123", request.get_headers().at("Authorization"));
    EXPECT_EQ("", request.get_body());
}

TEST_F(HttpRequestTest, from_string_should_parse_all_HTTP_methods_when_given_valid_strings)
{
    struct TestCase
    {
        std::string method;
        HttpMethod expected;
    };

    std::vector<TestCase> testCases = {
        {"GET", HttpMethod::GET},
        {"POST", HttpMethod::POST},
        {"PUT", HttpMethod::PUT},
        {"DELETE", HttpMethod::DELETE},
        {"PATCH", HttpMethod::PATCH},
        {"HEAD", HttpMethod::HEAD},
        {"OPTIONS", HttpMethod::OPTIONS},
        {"CONNECT", HttpMethod::CONNECT},
        {"TRACE", HttpMethod::TRACE}};

    for (const auto &testCase : testCases)
    {
        std::string requestString = testCase.method + " /test HTTP/1.1\r\nHost: example.com\r\n\r\n";
        HttpRequest request = HttpRequest::from_string(requestString);
        EXPECT_EQ(testCase.expected, request.get_method()) << "Failed for method: " << testCase.method;
    }
}

TEST_F(HttpRequestTest, from_string_should_parse_multiple_headers_when_given_request_with_multiple_headers)
{
    std::string requestString = "GET /test HTTP/1.1\r\n"
                                "Host: example.com\r\n"
                                "User-Agent: Mozilla/5.0\r\n"
                                "Accept: text/html\r\n"
                                "Connection: keep-alive\r\n"
                                "Cache-Control: no-cache\r\n\r\n";

    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ(HttpMethod::GET, request.get_method());
    EXPECT_EQ("/test", request.get_uri());
    EXPECT_EQ("HTTP/1.1", request.get_version());

    auto headers = request.get_headers();
    EXPECT_EQ("example.com", headers.at("Host"));
    EXPECT_EQ("Mozilla/5.0", headers.at("User-Agent"));
    EXPECT_EQ("text/html", headers.at("Accept"));
    EXPECT_EQ("keep-alive", headers.at("Connection"));
    EXPECT_EQ("no-cache", headers.at("Cache-Control"));
}

TEST_F(HttpRequestTest, from_string_should_handle_headers_with_whitespace_when_given_headers_with_spaces)
{
    std::string requestString = "GET /test HTTP/1.1\r\n"
                                "Host:   example.com   \r\n"
                                "Content-Type:application/json\r\n"
                                "Authorization:  Bearer token123  \r\n\r\n";

    HttpRequest request = HttpRequest::from_string(requestString);

    auto headers = request.get_headers();
    EXPECT_EQ("example.com", headers.at("Host"));
    EXPECT_EQ("application/json", headers.at("Content-Type"));
    EXPECT_EQ("Bearer token123", headers.at("Authorization"));
}

TEST_F(HttpRequestTest, from_string_should_parse_empty_body_when_given_request_without_body)
{
    std::string requestString = "GET /test HTTP/1.1\r\nHost: example.com\r\n\r\n";

    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ("", request.get_body());
}

TEST_F(HttpRequestTest, from_string_should_parse_complex_URI_when_given_request_with_query_parameters)
{
    std::string requestString = "GET /search?q=test&limit=10&offset=20 HTTP/1.1\r\nHost: example.com\r\n\r\n";

    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ(HttpMethod::GET, request.get_method());
    EXPECT_EQ("/search?q=test&limit=10&offset=20", request.get_uri());
    EXPECT_EQ("HTTP/1.1", request.get_version());
}

TEST_F(HttpRequestTest, from_string_should_parse_different_HTTP_versions_when_given_valid_versions)
{
    struct TestCase
    {
        std::string version;
    };

    std::vector<TestCase> testCases = {
        {"HTTP/1.0"},
        {"HTTP/1.1"},
        {"HTTP/2.0"}};

    for (const auto &testCase : testCases)
    {
        std::string requestString = "GET /test " + testCase.version + "\r\nHost: example.com\r\n\r\n";
        HttpRequest request = HttpRequest::from_string(requestString);
        EXPECT_EQ(testCase.version, request.get_version()) << "Failed for version: " << testCase.version;
    }
}

// Tests for edge cases and error conditions
TEST_F(HttpRequestTest, from_string_should_throw_exception_when_given_invalid_HTTP_method)
{
    std::string requestString = "INVALID /test HTTP/1.1\r\nHost: example.com\r\n\r\n";

    EXPECT_THROW(HttpRequest::from_string(requestString), std::invalid_argument);
}

TEST_F(HttpRequestTest, from_string_should_handle_empty_string_when_given_empty_input)
{
    std::string requestString = "";

    // This should not crash, though the resulting object may have default values
    EXPECT_NO_THROW(HttpRequest::from_string(requestString));
}

TEST_F(HttpRequestTest, from_string_should_handle_malformed_headers_when_given_headers_without_colon)
{
    std::string requestString = "GET /test HTTP/1.1\r\n"
                                "Host: example.com\r\n"
                                "InvalidHeaderWithoutColon\r\n"
                                "Content-Type: application/json\r\n\r\n";

    HttpRequest request = HttpRequest::from_string(requestString);

    // Should still parse valid headers
    auto headers = request.get_headers();
    EXPECT_EQ("example.com", headers.at("Host"));
    EXPECT_EQ("application/json", headers.at("Content-Type"));
    // Invalid header should be ignored
    EXPECT_EQ(headers.end(), headers.find("InvalidHeaderWithoutColon"));
}

TEST_F(HttpRequestTest, from_string_should_handle_request_with_only_request_line_when_given_minimal_request)
{
    std::string requestString = "GET /test HTTP/1.1";

    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ(HttpMethod::GET, request.get_method());
    EXPECT_EQ("/test", request.get_uri());
    EXPECT_EQ("HTTP/1.1", request.get_version());
    EXPECT_TRUE(request.get_headers().empty());
    EXPECT_EQ("", request.get_body());
}

// Tests for getter methods
TEST_F(HttpRequestTest, get_method_should_return_correct_method_when_request_is_parsed)
{
    std::string requestString = "PATCH /api/resource HTTP/1.1\r\nHost: example.com\r\n\r\n";
    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ(HttpMethod::PATCH, request.get_method());
}

TEST_F(HttpRequestTest, get_uri_should_return_correct_uri_when_request_is_parsed)
{
    std::string requestString = "GET /api/v1/users/123 HTTP/1.1\r\nHost: example.com\r\n\r\n";
    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ("/api/v1/users/123", request.get_uri());
}

TEST_F(HttpRequestTest, get_version_should_return_correct_version_when_request_is_parsed)
{
    std::string requestString = "GET /test HTTP/2.0\r\nHost: example.com\r\n\r\n";
    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ("HTTP/2.0", request.get_version());
}

TEST_F(HttpRequestTest, get_headers_should_return_correct_headers_when_request_is_parsed)
{
    std::string requestString = "GET /test HTTP/1.1\r\n"
                                "Host: example.com\r\n"
                                "Content-Type: text/plain\r\n\r\n";
    HttpRequest request = HttpRequest::from_string(requestString);

    auto headers = request.get_headers();
    EXPECT_EQ(2, headers.size());
    EXPECT_EQ("example.com", headers.at("Host"));
    EXPECT_EQ("text/plain", headers.at("Content-Type"));
}

TEST_F(HttpRequestTest, get_body_should_return_correct_body_when_request_is_parsed)
{
    std::string requestString = "POST /api/data HTTP/1.1\r\n"
                                "Host: example.com\r\n"
                                "Content-Type: application/json\r\n\r\n"
                                "{\"message\":\"Hello World\"}";
    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ("{\"message\":\"Hello World\"}", request.get_body());
}

// Tests for const correctness of getters
TEST_F(HttpRequestTest, getters_should_work_on_const_objects_when_called_on_const_reference)
{
    std::string requestString = "GET /test HTTP/1.1\r\nHost: example.com\r\n\r\ntest body";
    const HttpRequest request = HttpRequest::from_string(requestString);

    // All these should compile and work on const object
    EXPECT_EQ(HttpMethod::GET, request.get_method());
    EXPECT_EQ("/test", request.get_uri());
    EXPECT_EQ("HTTP/1.1", request.get_version());
    EXPECT_EQ("example.com", request.get_headers().at("Host"));
    EXPECT_EQ("test body", request.get_body());
}

// Integration test for complete request parsing
TEST_F(HttpRequestTest, from_string_should_parse_complete_realistic_request_when_given_full_HTTP_request)
{
    std::string requestString =
        "POST /api/v1/users HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "User-Agent: MyClient/1.0\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 45\r\n"
        "Authorization: Bearer eyJhbGciOiJIUzI1NiJ9\r\n"
        "Accept: application/json\r\n"
        "Connection: close\r\n\r\n"
        "{\"username\":\"john_doe\",\"email\":\"john@example.com\"}";

    HttpRequest request = HttpRequest::from_string(requestString);

    EXPECT_EQ(HttpMethod::POST, request.get_method());
    EXPECT_EQ("/api/v1/users", request.get_uri());
    EXPECT_EQ("HTTP/1.1", request.get_version());

    auto headers = request.get_headers();
    EXPECT_EQ("api.example.com", headers.at("Host"));
    EXPECT_EQ("MyClient/1.0", headers.at("User-Agent"));
    EXPECT_EQ("application/json", headers.at("Content-Type"));
    EXPECT_EQ("45", headers.at("Content-Length"));
    EXPECT_EQ("Bearer eyJhbGciOiJIUzI1NiJ9", headers.at("Authorization"));
    EXPECT_EQ("application/json", headers.at("Accept"));
    EXPECT_EQ("close", headers.at("Connection"));

    EXPECT_EQ("{\"username\":\"john_doe\",\"email\":\"john@example.com\"}", request.get_body());
}
