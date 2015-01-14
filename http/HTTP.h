#pragma once

class HTTP
{
public:
	static const int majorVersion = 1;
    static const int minorVersion = 0;

    static const char byteArrayVersion[];

	static const std::string transceiver;
	static const std::string contentTypeXML;
	static const std::string contentTypeHTML;
    static const std::string contentTypeJPEG;
    static const std::string contentTypeGIF;
    static const std::string nocache;
	static const std::string date;
	static const std::string acceptRange;
	static const std::string lastModified;
	static const std::string ETag;
	static const std::string contentLength;
    static const std::string redirect;

	static const std::string crlf;
    static const std::string msg100[];
    static const std::string msg200[];
    static const std::string msg300[];
    static const std::string msg400[];
	static const std::string msg500[];

    static const int CONTINUE = 100;
    static const int SWITCHING = 101;
    static const int OK = 200;
    static const int CREATED = 201;
    static const int ACCEPTED = 202;
    static const int NON_AUTHORITATIVE_INFORMATION = 203;
    static const int NO_CONTENT = 204;
    static const int RESET_CONTENT = 205;
    static const int PARTIAL_CONTENT = 206;

    static const int MULTIPLE_CHOICE = 300;
    static const int MOVED_PERMANENTLY = 301;
    static const int FOUND = 302;
    static const int SEE_OTHER = 303;
    static const int NOT_MODIFIED = 304;
    static const int USE_PROXY = 305;
    static const int TEMPORARY_REDIRECT = 307;

    static const int BAD_REQUEST = 400;
    static const int UNAUTHORIZED = 401;
    static const int PAYMENT_REQUIRED = 402;
    static const int FORBIDDEN = 403;
    static const int NOT_FOUND = 404;
    static const int NOT_ALLOWED = 405;
    static const int NOT_ACCEPTABLE = 406;
    static const int PROXY_AUTH_REQUIRED = 407;
    static const int REQUEST_TIMEOUT = 408;
    static const int CONFLICT = 409;
    static const int GONE = 410;
    static const int LENGTH_REQUIRED = 411;
    static const int PRECONDITION_FAILED = 412;
    static const int REQUEST_ENTITY_TOO_LARGE = 413;
    static const int REQUEST_URI_TOO_LONG = 414;
    static const int UNSUPPORTED_MEDIA_TYPE = 415;
    static const int REQUESTED_RANGE_NOT_SATISFIABLE = 416;
    static const int EXPECTATION_FAILED = 417;

    static const int INTERNAL_SERVER_ERROR = 500;
    static const int NOT_IMPLEMENTED = 501;
    static const int BAD_GATEWAY = 502;
    static const int SERVICE_UNAVAILABLE = 503;
    static const int GATEWAY_TIMEOUT = 504;
    static const int HTTP_VERSION_NOT_SUPPORTED = 505;
    static const int NOT_EXTENDED = 510;

    static const int NOHEADER = 1000;
    static const int DONE = 1001;
};
