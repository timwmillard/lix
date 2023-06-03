#include <stdio.h>
#include <stdlib.h>

#include "str.h"

typedef enum HttpMethod {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_PATCH,
    HTTP_DELETE,
    HTTP_OPTION
} HttpMethod;

typedef struct HttpHeader {
    Str name;
    Str value;
} HttpHeader;

typedef struct HttpBodyReader {
   size_t already_read;
   void *next; 
   int sock;
} HttpBodyReader;

typedef struct HttpRequest {

    HttpMethod method;

    HttpHeader headers[64];

    HttpBodyReader body;

} HttpRequest;


void http_parse(char *stream)
{

}

int http_read_body(HttpBodyReader reader, void *buf, size_t buf_size)
{
    return 0;
}

void http_close_body(HttpBodyReader reader)
{
}

