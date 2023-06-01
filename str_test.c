#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include <unistd.h>

#include "deps/picohttpparser/picohttpparser.h"
#include "str.h"

void test_passed(char *name)
{
    printf("*** Test %s passed ***\n", name);
}

void test_string()
{

    String tim = cstring("Tim Millard");
    String *ptr = &tim;
    char **data = (char**) &tim;
    // printf("(char**)& = %s\n", *(char**)&tim);
    printf("STR(tim) = %s\n", STR(tim));
    printf("STR(*ptr) = %s\n", STR(*ptr));
    // printf("tim.data = %s\n", tim.data);
    printf("data = %s\n", *data);
    printf("tim = %s\n", string_c(tim));
    // printf("tim.len = %ld\n",  tim.len);
    // printf("tim.cap = %ld\n",  tim.cap);
    // assert(tim.len == 11);
    // assert(tim.cap == 12);

    test_passed("String");
}


int main()
{
    size_t len = _cstr_len("Tim Millard");
    assert(len == 11);

    test_string();

    Str hello = cstr("Hello");
    Str world = cstr("World");

    String hello_world = str_concat(hello, world);
    assert(string_len(hello_world) == 11);
    // assert(hello_world.len == 11);

    printf("******* All tests passed *******\n");

    return 0;
}
