#include <stdio.h>
#include <stdlib.h>

typedef enum HttpMethod {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_PATCH,
    HTTP_DELETE,
    HTTP_OPTION
} HttpMethod;

typedef struct HttpHeader {
    char *name;
    char *value;
} HttpHeader;

typedef struct HttpRequest {

    HttpMethod method;

    HttpHeader headers[64];

    char *body;

    size_t body_len;

} HttpRequest;


void http_parse(char *stream)
{

}
