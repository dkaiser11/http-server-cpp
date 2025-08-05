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

inline HttpCode httpCodeFromString(const std::string &code)
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

#endif // HTTPCODE_HPP