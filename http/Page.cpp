#include "Page.h"

const char * Page::POST="POST";
const char * Page::GET="GET";
const char * Page::HOST="HOST";
const char * Page::CONTENT_TYPE = "content-type";
const char * Page::VERSION = " HTTP/1.1\r\n";
const char * Page::ACCEPT = "Accept: */*\r\n";
const char * Page::USERAGENT = "User-Agent: Mozilla/4.0 (Compatible)\r\n";
const char * Page::NEWLINE = "\r\n";
const char * Page::empty="";

const char * Page::methods[] = {
	"GET", "POST", "HEAD", "OPTIONS", "PUT", "DELETE", "TRACE"};
