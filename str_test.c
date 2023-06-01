#include <stdio.h>
#include <assert.h>

#include "str.h"

int main()
{
    size_t len = _cstr_len("Tim Millard");
    assert(len == 11);

    Str tim = cstr("Tim Millard");
    assert(tim.len == 11);

    Str hello = cstr("Hello");
    Str world = cstr("World");

    String hello_world = str_concat(hello, world);
    assert(hello_world.len == 11);

    printf("******* All tests passed *******\n");

    return 0;
}
