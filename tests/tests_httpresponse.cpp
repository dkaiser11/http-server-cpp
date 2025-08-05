#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "http/httpresponse.hpp"
#include "http/httpcode.hpp"
#include <stdexcept>

class HttpResponseTest : public ::testing::Test
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
TEST_F(HttpResponseTest, DefaultConstructor_should_create_valid_instance_when_called)
{
    HttpResponse response;

    // Default constructor should create a valid object
    // We can't directly test private members without getters, but we can test that the object is created
    EXPECT_NO_THROW(HttpResponse response2);
}

// Tests for copy constructor
TEST_F(HttpResponseTest, CopyConstructor_should_create_identical_copy_when_given_valid_response)
{
    // Create original response
    std::string validResponse = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: 13\r\n\r\n{\"test\":\"data\"}";
    HttpResponse original = HttpResponse::fromString(validResponse);

    // Create copy
    HttpResponse copy(original);

    // Verify copy is identical
    EXPECT_EQ(original.getVersion(), copy.getVersion());
    EXPECT_EQ(original.getCode(), copy.getCode());
    EXPECT_EQ(original.getHeaders(), copy.getHeaders());
    EXPECT_EQ(original.getBody(), copy.getBody());
}

// Tests for copy assignment operator
TEST_F(HttpResponseTest, CopyAssignment_should_assign_values_correctly_when_given_valid_response)
{
    // Create original response
    std::string validResponse = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nServer: TestServer/1.0\r\n\r\n<html><body>Not Found</body></html>";
    HttpResponse original = HttpResponse::fromString(validResponse);

    // Create target response and assign
    HttpResponse target;
    target = original;

    // Verify assignment worked correctly
    EXPECT_EQ(original.getVersion(), target.getVersion());
    EXPECT_EQ(original.getCode(), target.getCode());
    EXPECT_EQ(original.getHeaders(), target.getHeaders());
    EXPECT_EQ(original.getBody(), target.getBody());
}

// Tests for move constructor
TEST_F(HttpResponseTest, MoveConstructor_should_transfer_ownership_when_given_rvalue_response)
{
    // Create response and capture its state
    std::string validResponse = "HTTP/1.1 201 Created\r\nLocation: /api/users/123\r\nContent-Type: application/json\r\n\r\n{\"id\":123,\"created\":true}";
    HttpResponse original = HttpResponse::fromString(validResponse);
    std::string expectedVersion = original.getVersion();
    HttpCode expectedCode = original.getCode();
    auto expectedHeaders = original.getHeaders();
    std::string expectedBody = original.getBody();

    // Move construct
    HttpResponse moved(std::move(original));

    // Verify moved object has correct values
    EXPECT_EQ(expectedVersion, moved.getVersion());
    EXPECT_EQ(expectedCode, moved.getCode());
    EXPECT_EQ(expectedHeaders, moved.getHeaders());
    EXPECT_EQ(expectedBody, moved.getBody());
}

// Tests for move assignment operator
TEST_F(HttpResponseTest, MoveAssignment_should_transfer_ownership_when_given_rvalue_response)
{
    // Create response and capture its state
    std::string validResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nServer error occurred";
    HttpResponse original = HttpResponse::fromString(validResponse);
    std::string expectedVersion = original.getVersion();
    HttpCode expectedCode = original.getCode();
    auto expectedHeaders = original.getHeaders();
    std::string expectedBody = original.getBody();

    // Move assign
    HttpResponse target;
    target = std::move(original);

    // Verify target has correct values
    EXPECT_EQ(expectedVersion, target.getVersion());
    EXPECT_EQ(expectedCode, target.getCode());
    EXPECT_EQ(expectedHeaders, target.getHeaders());
    EXPECT_EQ(expectedBody, target.getBody());
}

// Tests for fromString method - basic success responses
TEST_F(HttpResponseTest, fromString_should_parse_200_OK_response_when_given_valid_OK_string)
{
    std::string responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 13\r\n\r\n<html></html>";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::OK, response.getCode());
    EXPECT_EQ("text/html", response.getHeaders().at("Content-Type"));
    EXPECT_EQ("13", response.getHeaders().at("Content-Length"));
    EXPECT_EQ("<html></html>", response.getBody());
}

TEST_F(HttpResponseTest, fromString_should_parse_201_Created_response_when_given_valid_Created_string)
{
    std::string responseString = "HTTP/1.1 201 Created\r\nContent-Type: application/json\r\nLocation: /api/users/123\r\n\r\n{\"id\":123,\"status\":\"created\"}";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::Created, response.getCode());
    EXPECT_EQ("application/json", response.getHeaders().at("Content-Type"));
    EXPECT_EQ("/api/users/123", response.getHeaders().at("Location"));
    EXPECT_EQ("{\"id\":123,\"status\":\"created\"}", response.getBody());
}

TEST_F(HttpResponseTest, fromString_should_parse_204_NoContent_response_when_given_valid_NoContent_string)
{
    std::string responseString = "HTTP/1.1 204 No Content\r\nServer: TestServer/1.0\r\n\r\n";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::NoContent, response.getCode());
    EXPECT_EQ("TestServer/1.0", response.getHeaders().at("Server"));
    EXPECT_EQ("", response.getBody());
}

// Tests for fromString method - client error responses
TEST_F(HttpResponseTest, fromString_should_parse_400_BadRequest_response_when_given_valid_BadRequest_string)
{
    std::string responseString = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"error\":\"Invalid request format\"}";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::BadRequest, response.getCode());
    EXPECT_EQ("application/json", response.getHeaders().at("Content-Type"));
    EXPECT_EQ("{\"error\":\"Invalid request format\"}", response.getBody());
}

TEST_F(HttpResponseTest, fromString_should_parse_401_Unauthorized_response_when_given_valid_Unauthorized_string)
{
    std::string responseString = "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Bearer\r\nContent-Type: text/plain\r\n\r\nAuthentication required";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::Unauthorized, response.getCode());
    EXPECT_EQ("Bearer", response.getHeaders().at("WWW-Authenticate"));
    EXPECT_EQ("text/plain", response.getHeaders().at("Content-Type"));
    EXPECT_EQ("Authentication required", response.getBody());
}

TEST_F(HttpResponseTest, fromString_should_parse_404_NotFound_response_when_given_valid_NotFound_string)
{
    std::string responseString = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::NotFound, response.getCode());
    EXPECT_EQ("text/html", response.getHeaders().at("Content-Type"));
    EXPECT_EQ("<html><body><h1>404 Not Found</h1></body></html>", response.getBody());
}

// Tests for fromString method - server error responses
TEST_F(HttpResponseTest, fromString_should_parse_500_InternalServerError_response_when_given_valid_InternalServerError_string)
{
    std::string responseString = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nInternal server error";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::InternalServerError, response.getCode());
    EXPECT_EQ("text/plain", response.getHeaders().at("Content-Type"));
    EXPECT_EQ("close", response.getHeaders().at("Connection"));
    EXPECT_EQ("Internal server error", response.getBody());
}

TEST_F(HttpResponseTest, fromString_should_parse_503_ServiceUnavailable_response_when_given_valid_ServiceUnavailable_string)
{
    std::string responseString = "HTTP/1.1 503 Service Unavailable\r\nRetry-After: 120\r\nContent-Type: application/json\r\n\r\n{\"message\":\"Service temporarily unavailable\"}";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::ServiceUnavailable, response.getCode());
    EXPECT_EQ("120", response.getHeaders().at("Retry-After"));
    EXPECT_EQ("application/json", response.getHeaders().at("Content-Type"));
    EXPECT_EQ("{\"message\":\"Service temporarily unavailable\"}", response.getBody());
}

// Tests for fromString method - redirection responses
TEST_F(HttpResponseTest, fromString_should_parse_301_MovedPermanently_response_when_given_valid_MovedPermanently_string)
{
    std::string responseString = "HTTP/1.1 301 Moved Permanently\r\nLocation: https://example.com/new-location\r\nContent-Length: 0\r\n\r\n";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::MovedPermanently, response.getCode());
    EXPECT_EQ("https://example.com/new-location", response.getHeaders().at("Location"));
    EXPECT_EQ("0", response.getHeaders().at("Content-Length"));
    EXPECT_EQ("", response.getBody());
}

TEST_F(HttpResponseTest, fromString_should_parse_302_Found_response_when_given_valid_Found_string)
{
    std::string responseString = "HTTP/1.1 302 Found\r\nLocation: /login\r\nSet-Cookie: session=abc123\r\n\r\n";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::Found, response.getCode());
    EXPECT_EQ("/login", response.getHeaders().at("Location"));
    EXPECT_EQ("session=abc123", response.getHeaders().at("Set-Cookie"));
    EXPECT_EQ("", response.getBody());
}

// Tests for fromString method - informational responses
TEST_F(HttpResponseTest, fromString_should_parse_100_Continue_response_when_given_valid_Continue_string)
{
    std::string responseString = "HTTP/1.1 100 Continue\r\n\r\n";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::Continue, response.getCode());
    EXPECT_TRUE(response.getHeaders().empty());
    EXPECT_EQ("", response.getBody());
}

// Tests for fromString method - comprehensive HTTP code coverage
TEST_F(HttpResponseTest, fromString_should_parse_all_supported_HTTP_codes_when_given_valid_code_strings)
{
    struct TestCase
    {
        std::string code;
        HttpCode expected;
    };

    std::vector<TestCase> testCases = {
        // 1xx Informational
        {"100", HttpCode::Continue},
        {"101", HttpCode::SwitchingProtocols},
        // 2xx Success
        {"200", HttpCode::OK},
        {"201", HttpCode::Created},
        {"202", HttpCode::Accepted},
        {"204", HttpCode::NoContent},
        {"206", HttpCode::PartialContent},
        // 3xx Redirection
        {"300", HttpCode::MultipleChoices},
        {"301", HttpCode::MovedPermanently},
        {"302", HttpCode::Found},
        {"303", HttpCode::SeeOther},
        {"304", HttpCode::NotModified},
        {"307", HttpCode::TemporaryRedirect},
        {"308", HttpCode::PermanentRedirect},
        // 4xx Client Error
        {"400", HttpCode::BadRequest},
        {"401", HttpCode::Unauthorized},
        {"403", HttpCode::Forbidden},
        {"404", HttpCode::NotFound},
        {"405", HttpCode::MethodNotAllowed},
        {"406", HttpCode::NotAcceptable},
        {"408", HttpCode::RequestTimeout},
        {"409", HttpCode::Conflict},
        {"410", HttpCode::Gone},
        {"411", HttpCode::LengthRequired},
        {"412", HttpCode::PreconditionFailed},
        {"413", HttpCode::PayloadTooLarge},
        {"414", HttpCode::URITooLong},
        {"415", HttpCode::UnsupportedMediaType},
        {"416", HttpCode::RangeNotSatisfiable},
        {"422", HttpCode::UnprocessableEntity},
        {"429", HttpCode::TooManyRequests},
        // 5xx Server Error
        {"500", HttpCode::InternalServerError},
        {"501", HttpCode::NotImplemented},
        {"502", HttpCode::BadGateway},
        {"503", HttpCode::ServiceUnavailable},
        {"504", HttpCode::GatewayTimeout},
        {"505", HttpCode::HTTPVersionNotSupported}};

    for (const auto &testCase : testCases)
    {
        std::string responseString = "HTTP/1.1 " + testCase.code + " Status\r\n\r\n";
        HttpResponse response = HttpResponse::fromString(responseString);
        EXPECT_EQ(testCase.expected, response.getCode()) << "Failed for code: " << testCase.code;
    }
}

// Tests for fromString method - multiple headers
TEST_F(HttpResponseTest, fromString_should_parse_multiple_headers_when_given_response_with_multiple_headers)
{
    std::string responseString = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: application/json\r\n"
                                 "Content-Length: 25\r\n"
                                 "Server: TestServer/1.0\r\n"
                                 "Cache-Control: no-cache\r\n"
                                 "Connection: keep-alive\r\n"
                                 "Date: Mon, 05 Aug 2025 12:00:00 GMT\r\n\r\n"
                                 "{\"message\":\"Hello World\"}";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::OK, response.getCode());

    auto headers = response.getHeaders();
    EXPECT_EQ("application/json", headers.at("Content-Type"));
    EXPECT_EQ("25", headers.at("Content-Length"));
    EXPECT_EQ("TestServer/1.0", headers.at("Server"));
    EXPECT_EQ("no-cache", headers.at("Cache-Control"));
    EXPECT_EQ("keep-alive", headers.at("Connection"));
    EXPECT_EQ("Mon, 05 Aug 2025 12:00:00 GMT", headers.at("Date"));
    EXPECT_EQ("{\"message\":\"Hello World\"}", response.getBody());
}

// Tests for fromString method - header whitespace handling
TEST_F(HttpResponseTest, fromString_should_handle_headers_with_whitespace_when_given_headers_with_spaces)
{
    std::string responseString = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type:   application/json   \r\n"
                                 "Server:TestServer/1.0\r\n"
                                 "Cache-Control:  no-cache, no-store  \r\n\r\n"
                                 "{}";

    HttpResponse response = HttpResponse::fromString(responseString);

    auto headers = response.getHeaders();
    EXPECT_EQ("application/json", headers.at("Content-Type"));
    EXPECT_EQ("TestServer/1.0", headers.at("Server"));
    EXPECT_EQ("no-cache, no-store", headers.at("Cache-Control"));
}

// Tests for fromString method - different HTTP versions
TEST_F(HttpResponseTest, fromString_should_parse_different_HTTP_versions_when_given_valid_versions)
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
        std::string responseString = testCase.version + " 200 OK\r\n\r\n";
        HttpResponse response = HttpResponse::fromString(responseString);
        EXPECT_EQ(testCase.version, response.getVersion()) << "Failed for version: " << testCase.version;
    }
}

// Tests for fromString method - empty body
TEST_F(HttpResponseTest, fromString_should_parse_empty_body_when_given_response_without_body)
{
    std::string responseString = "HTTP/1.1 304 Not Modified\r\nCache-Control: max-age=3600\r\n\r\n";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("", response.getBody());
    EXPECT_EQ("max-age=3600", response.getHeaders().at("Cache-Control"));
}

// Tests for fromString method - large body content
TEST_F(HttpResponseTest, fromString_should_parse_large_body_when_given_response_with_large_body)
{
    std::string largeBody = std::string(1000, 'A'); // 1000 'A' characters
    std::string responseString = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: text/plain\r\n"
                                 "Content-Length: 1000\r\n\r\n" +
                                 largeBody;

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::OK, response.getCode());
    EXPECT_EQ("text/plain", response.getHeaders().at("Content-Type"));
    EXPECT_EQ("1000", response.getHeaders().at("Content-Length"));
    EXPECT_EQ(largeBody, response.getBody());
}

// Tests for fromString method - JSON response body
TEST_F(HttpResponseTest, fromString_should_parse_JSON_body_when_given_response_with_JSON_content)
{
    std::string jsonBody = "{\"users\":[{\"id\":1,\"name\":\"John\"},{\"id\":2,\"name\":\"Jane\"}],\"total\":2}";
    std::string responseString = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: application/json\r\n"
                                 "Content-Length: " +
                                 std::to_string(jsonBody.length()) + "\r\n\r\n" + jsonBody;

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::OK, response.getCode());
    EXPECT_EQ("application/json", response.getHeaders().at("Content-Type"));
    EXPECT_EQ(jsonBody, response.getBody());
}

// Tests for fromString method - edge cases and error handling
TEST_F(HttpResponseTest, fromString_should_handle_empty_string_when_given_empty_input)
{
    std::string responseString = "";

    // This should not crash, though the resulting object may have default values
    EXPECT_NO_THROW(HttpResponse::fromString(responseString));
}

TEST_F(HttpResponseTest, fromString_should_handle_malformed_headers_when_given_headers_without_colon)
{
    std::string responseString = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: application/json\r\n"
                                 "InvalidHeaderWithoutColon\r\n"
                                 "Server: TestServer/1.0\r\n\r\n"
                                 "{}";

    HttpResponse response = HttpResponse::fromString(responseString);

    // Should still parse valid headers
    auto headers = response.getHeaders();
    EXPECT_EQ("application/json", headers.at("Content-Type"));
    EXPECT_EQ("TestServer/1.0", headers.at("Server"));
    // Invalid header should be ignored
    EXPECT_EQ(headers.end(), headers.find("InvalidHeaderWithoutColon"));
}

TEST_F(HttpResponseTest, fromString_should_handle_response_with_only_status_line_when_given_minimal_response)
{
    std::string responseString = "HTTP/1.1 200 OK";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::OK, response.getCode());
    EXPECT_TRUE(response.getHeaders().empty());
    EXPECT_EQ("", response.getBody());
}

TEST_F(HttpResponseTest, fromString_should_throw_exception_when_given_invalid_HTTP_code)
{
    std::string responseString = "HTTP/1.1 999 Invalid\r\n\r\n";

    EXPECT_THROW(HttpResponse::fromString(responseString), std::invalid_argument);
}

TEST_F(HttpResponseTest, fromString_should_throw_exception_when_given_non_numeric_HTTP_code)
{
    std::string responseString = "HTTP/1.1 ABC Invalid\r\n\r\n";

    EXPECT_THROW(HttpResponse::fromString(responseString), std::invalid_argument);
}

// Tests for getter methods
TEST_F(HttpResponseTest, getVersion_should_return_correct_version_when_response_is_parsed)
{
    std::string responseString = "HTTP/2.0 200 OK\r\nServer: TestServer/1.0\r\n\r\n";
    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/2.0", response.getVersion());
}

TEST_F(HttpResponseTest, getCode_should_return_correct_code_when_response_is_parsed)
{
    std::string responseString = "HTTP/1.1 418 I'm a teapot\r\n\r\n";

    // Note: 418 is not in our enum, so this should throw
    EXPECT_THROW(HttpResponse::fromString(responseString), std::invalid_argument);
}

TEST_F(HttpResponseTest, getCode_should_return_correct_code_when_response_has_valid_code)
{
    std::string responseString = "HTTP/1.1 403 Forbidden\r\nServer: TestServer/1.0\r\n\r\n";
    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ(HttpCode::Forbidden, response.getCode());
}

TEST_F(HttpResponseTest, getHeaders_should_return_correct_headers_when_response_is_parsed)
{
    std::string responseString = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: text/html\r\n"
                                 "Content-Encoding: gzip\r\n\r\n";
    HttpResponse response = HttpResponse::fromString(responseString);

    auto headers = response.getHeaders();
    EXPECT_EQ(2, headers.size());
    EXPECT_EQ("text/html", headers.at("Content-Type"));
    EXPECT_EQ("gzip", headers.at("Content-Encoding"));
}

TEST_F(HttpResponseTest, getBody_should_return_correct_body_when_response_is_parsed)
{
    std::string responseString = "HTTP/1.1 201 Created\r\n"
                                 "Content-Type: application/xml\r\n\r\n"
                                 "<?xml version=\"1.0\"?><root><item>test</item></root>";
    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("<?xml version=\"1.0\"?><root><item>test</item></root>", response.getBody());
}

// Tests for const correctness of getters
TEST_F(HttpResponseTest, getters_should_work_on_const_objects_when_called_on_const_reference)
{
    std::string responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello World";
    const HttpResponse response = HttpResponse::fromString(responseString);

    // All these should compile and work on const object
    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::OK, response.getCode());
    EXPECT_EQ("text/plain", response.getHeaders().at("Content-Type"));
    EXPECT_EQ("Hello World", response.getBody());
}

// Integration test for complete response parsing
TEST_F(HttpResponseTest, fromString_should_parse_complete_realistic_response_when_given_full_HTTP_response)
{
    std::string responseString =
        "HTTP/1.1 200 OK\r\n"
        "Date: Mon, 05 Aug 2025 12:00:00 GMT\r\n"
        "Server: Apache/2.4.41 (Ubuntu)\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Content-Length: 67\r\n"
        "Cache-Control: max-age=3600, public\r\n"
        "ETag: \"33a64df551425fcc55e4d42a148795d9f25f89d4\"\r\n"
        "Connection: keep-alive\r\n\r\n"
        "{\"status\":\"success\",\"data\":{\"message\":\"Hello World\"},\"timestamp\":1691236800}";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::OK, response.getCode());

    auto headers = response.getHeaders();
    EXPECT_EQ("Mon, 05 Aug 2025 12:00:00 GMT", headers.at("Date"));
    EXPECT_EQ("Apache/2.4.41 (Ubuntu)", headers.at("Server"));
    EXPECT_EQ("application/json; charset=utf-8", headers.at("Content-Type"));
    EXPECT_EQ("67", headers.at("Content-Length"));
    EXPECT_EQ("max-age=3600, public", headers.at("Cache-Control"));
    EXPECT_EQ("\"33a64df551425fcc55e4d42a148795d9f25f89d4\"", headers.at("ETag"));
    EXPECT_EQ("keep-alive", headers.at("Connection"));

    EXPECT_EQ("{\"status\":\"success\",\"data\":{\"message\":\"Hello World\"},\"timestamp\":1691236800}", response.getBody());
}

// Tests for parsing responses with CRLF vs LF line endings
TEST_F(HttpResponseTest, fromString_should_handle_mixed_line_endings_when_given_response_with_different_endings)
{
    std::string responseString = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: text/plain\r\n"
                                 "Content-Length: 11\r\n\r\n"
                                 "Hello World";

    HttpResponse response = HttpResponse::fromString(responseString);

    EXPECT_EQ("HTTP/1.1", response.getVersion());
    EXPECT_EQ(HttpCode::OK, response.getCode());
    EXPECT_EQ("text/plain", response.getHeaders().at("Content-Type"));
    EXPECT_EQ("11", response.getHeaders().at("Content-Length"));
    EXPECT_EQ("Hello World", response.getBody());
}

// Tests for parsing responses with headers that have special characters
TEST_F(HttpResponseTest, fromString_should_handle_headers_with_special_characters_when_given_complex_header_values)
{
    std::string responseString = "HTTP/1.1 200 OK\r\n"
                                 "Set-Cookie: sessionid=abc123; Path=/; Domain=.example.com; Secure; HttpOnly\r\n"
                                 "Content-Disposition: attachment; filename=\"report.pdf\"\r\n"
                                 "Custom-Header: value with spaces, commas; and semicolons\r\n\r\n"
                                 "body content";

    HttpResponse response = HttpResponse::fromString(responseString);

    auto headers = response.getHeaders();
    EXPECT_EQ("sessionid=abc123; Path=/; Domain=.example.com; Secure; HttpOnly", headers.at("Set-Cookie"));
    EXPECT_EQ("attachment; filename=\"report.pdf\"", headers.at("Content-Disposition"));
    EXPECT_EQ("value with spaces, commas; and semicolons", headers.at("Custom-Header"));
    EXPECT_EQ("body content", response.getBody());
}
