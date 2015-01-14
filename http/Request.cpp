#include "Request.h"

const char * Request::POST="POST";
const char * Request::GET="GET";
const char * Request::HOST="HOST";
const char * Request::CONTENT_TYPE = "content-type";
const char * Request::VERSION = " HTTP/1.1\r\n";
const char * Request::ACCEPT = "Accept: */*\r\n";
const char * Request::USERAGENT = "User-Agent: Mozilla/4.0 (Compatible)\r\n";
const char * Request::NEWLINE = "\r\n";
const char * Request::empty="";

const char * Request::methods[] = {
	"GET", "POST", "HEAD", "OPTIONS", "PUT", "DELETE", "TRACE"};
