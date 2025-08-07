#ifndef HTTPCODE_HPP
#define HTTPCODE_HPP

#include <string>
#include <stdexcept>

enum class HttpCode
{
    // 1xx Informational
    Continue = 100,
    SwitchingProtocols = 101,

    // 2xx Success
    OK = 200,
    Created = 201,
    Accepted = 202,
    NoContent = 204,
    PartialContent = 206,

    // 3xx Redirection
    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    TemporaryRedirect = 307,
    PermanentRedirect = 308,

    // 4xx Client Error
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    LengthRequired = 411,
    PreconditionFailed = 412,
    PayloadTooLarge = 413,
    URITooLong = 414,
    UnsupportedMediaType = 415,
    RangeNotSatisfiable = 416,
    UnprocessableEntity = 422,
    TooManyRequests = 429,

    // 5xx Server Error
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HTTPVersionNotSupported = 505
};

inline HttpCode http_code_from_string(const std::string &code)
{
    try
    {
        int codeInt = std::stoi(code);
        switch (codeInt)
        {
        // 1xx Informational
        case 100:
            return HttpCode::Continue;
        case 101:
            return HttpCode::SwitchingProtocols;

        // 2xx Success
        case 200:
            return HttpCode::OK;
        case 201:
            return HttpCode::Created;
        case 202:
            return HttpCode::Accepted;
        case 204:
            return HttpCode::NoContent;
        case 206:
            return HttpCode::PartialContent;

        // 3xx Redirection
        case 300:
            return HttpCode::MultipleChoices;
        case 301:
            return HttpCode::MovedPermanently;
        case 302:
            return HttpCode::Found;
        case 303:
            return HttpCode::SeeOther;
        case 304:
            return HttpCode::NotModified;
        case 307:
            return HttpCode::TemporaryRedirect;
        case 308:
            return HttpCode::PermanentRedirect;

        // 4xx Client Error
        case 400:
            return HttpCode::BadRequest;
        case 401:
            return HttpCode::Unauthorized;
        case 403:
            return HttpCode::Forbidden;
        case 404:
            return HttpCode::NotFound;
        case 405:
            return HttpCode::MethodNotAllowed;
        case 406:
            return HttpCode::NotAcceptable;
        case 408:
            return HttpCode::RequestTimeout;
        case 409:
            return HttpCode::Conflict;
        case 410:
            return HttpCode::Gone;
        case 411:
            return HttpCode::LengthRequired;
        case 412:
            return HttpCode::PreconditionFailed;
        case 413:
            return HttpCode::PayloadTooLarge;
        case 414:
            return HttpCode::URITooLong;
        case 415:
            return HttpCode::UnsupportedMediaType;
        case 416:
            return HttpCode::RangeNotSatisfiable;
        case 422:
            return HttpCode::UnprocessableEntity;
        case 429:
            return HttpCode::TooManyRequests;

        // 5xx Server Error
        case 500:
            return HttpCode::InternalServerError;
        case 501:
            return HttpCode::NotImplemented;
        case 502:
            return HttpCode::BadGateway;
        case 503:
            return HttpCode::ServiceUnavailable;
        case 504:
            return HttpCode::GatewayTimeout;
        case 505:
            return HttpCode::HTTPVersionNotSupported;

        default:
            throw std::invalid_argument("Unknown HTTP status code: " + code);
        }
    }
    catch (const std::invalid_argument &e)
    {
        throw std::invalid_argument("Invalid HTTP status code format: " + code);
    }
}

inline std::string http_code_to_string(HttpCode code)
{
    switch (code)
    {
    // 1xx Informational
    case HttpCode::Continue:
        return "100 Continue";
    case HttpCode::SwitchingProtocols:
        return "101 Switching Protocols";

    // 2xx Success
    case HttpCode::OK:
        return "200 OK";
    case HttpCode::Created:
        return "201 Created";
    case HttpCode::Accepted:
        return "202 Accepted";
    case HttpCode::NoContent:
        return "204 No Content";
    case HttpCode::PartialContent:
        return "206 Partial Content";

    // 3xx Redirection
    case HttpCode::MultipleChoices:
        return "300 Multiple Choices";
    case HttpCode::MovedPermanently:
        return "301 Moved Permanently";
    case HttpCode::Found:
        return "302 Found";
    case HttpCode::SeeOther:
        return "303 See Other";
    case HttpCode::NotModified:
        return "304 Not Modified";
    case HttpCode::TemporaryRedirect:
        return "307 Temporary Redirect";
    case HttpCode::PermanentRedirect:
        return "308 Permanent Redirect";

    // 4xx Client Error
    case HttpCode::BadRequest:
        return "400 Bad Request";
    case HttpCode::Unauthorized:
        return "401 Unauthorized";
    case HttpCode::Forbidden:
        return "403 Forbidden";
    case HttpCode::NotFound:
        return "404 Not Found";
    case HttpCode::MethodNotAllowed:
        return "405 Method Not Allowed";
    case HttpCode::NotAcceptable:
        return "406 Not Acceptable";
    case HttpCode::RequestTimeout:
        return "408 Request Timeout";
    case HttpCode::Conflict:
        return "409 Conflict";
    case HttpCode::Gone:
        return "410 Gone";
    case HttpCode::LengthRequired:
        return "411 Length Required";
    case HttpCode::PreconditionFailed:
        return "412 Precondition Failed";
    case HttpCode::PayloadTooLarge:
        return "413 Payload Too Large";
    case HttpCode::URITooLong:
        return "414 URI Too Long";
    case HttpCode::UnsupportedMediaType:
        return "415 Unsupported Media Type";
    case HttpCode::RangeNotSatisfiable:
        return "416 Range Not Satisfiable";
    case HttpCode::UnprocessableEntity:
        return "422 Unprocessable Entity";
    case HttpCode::TooManyRequests:
        return "429 Too Many Requests";

    // 5xx Server Error
    case HttpCode::InternalServerError:
        return "500 Internal Server Error";
    case HttpCode::NotImplemented:
        return "501 Not Implemented";
    case HttpCode::BadGateway:
        return "502 Bad Gateway";
    case HttpCode::ServiceUnavailable:
        return "503 Service Unavailable";
    case HttpCode::GatewayTimeout:
        return "504 Gateway Timeout";
    case HttpCode::HTTPVersionNotSupported:
        return "505 HTTP Version Not Supported";

    default:
        throw std::invalid_argument("Unknown HTTP status code");
    }
}

#endif // HTTPCODE_HPP