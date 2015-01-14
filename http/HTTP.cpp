#include "HTTP.h"

const char HTTP::byteArrayVersion [] =
    {
	    (byte) 'H', (byte) 'T', (byte) 'T', (byte) 'P',
	    (byte) '/', (byte) '1', (byte) '.', (byte) '0', (byte) ' '
    };

const std::string HTTP::transceiver  = "Server: Adaptinet-Transceiver/1.0";
const std::string HTTP::contentTypeXML  = "Content-Type: text/xml";
const std::string HTTP::contentTypeHTML  = "Content-Type: text/html";
const std::string HTTP::contentTypeJPEG = "Content-Type: image/jpeg";
const std::string HTTP::contentTypeGIF = "Content-Type: image/gif";
const std::string HTTP::nocache = "Pragma: no-cache";
const std::string HTTP::date  = "Date: ";
const std::string HTTP::acceptRange  = "Accept-Range: bytes";
const std::string HTTP::lastModified  = "Last-Modified: ";
const std::string HTTP::ETag  = "ETag: ";
const std::string HTTP::contentLength  = "Content-Length: ";
const std::string HTTP::redirect = "Location: ";

const std::string HTTP::crlf  = "\r\n";
const std::string HTTP::msg100[] = {" Continue", " Switching Protocols" };

const std::string HTTP::msg200[] = {" OK"," Created"," Accepted"," Non-Authoritative information",
	                    " No Content"," Reset Content"," Partial Content"};

const std::string HTTP::msg300[] = {" Multiple Choices"," Moved Permanently"," Found",
	                    " See Other"," Not Modified"," Use Proxy",""," Temporary Redirect"};

const std::string HTTP::msg400[] = {" Bad Request"," Unauthorized"," Payment Required",
	                    " Forbidden"," Not Found"," Method Not Allowed",
	                    " Not Acceptable","  Proxy Authentication Required",
	                    " Request Timeout"," Conflict"," Gone"," Length Required",
	                    " Precondition Failed"," Request Entity Too Large",
	                    " Request-URI Too Long"," Unsupported Media Type",
	                    " Requested Range Not Satisfiable",
    	                " Expectation Failed"};

const std::string HTTP::msg500[] = { " Internal Server Error"," Not Implemented",
	                     " Bad Gateway"," Service Unavailable"," Gateway Timeout",
	                     " HTTP Version Not Supported","","","",""," Not Extended"};
