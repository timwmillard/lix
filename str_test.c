#include <stdio.h>
#include <assert.h>

#include "str.h"

void test_passed(char *name)
{
    printf("*** Test %s passed ***\n", name);
}

void test_string()
{

    String tim = cstring("Tim Millard");
    printf("tim = %s\n", string_c(tim));
    printf("tim.len = %ld\n",  tim.len);
    printf("tim.cap = %ld\n",  tim.cap);
    assert(tim.len == 11);
    assert(tim.cap == 12);

    string_append(&tim, cstr(" loves C"));
    printf("tim = %s\n", string_c(tim));
    printf("tim.len = %ld\n",  tim.len);
    printf("tim.cap = %ld\n",  tim.cap);
    assert(tim.len == 19);
    assert(tim.cap == 25); // Cap doubled

    test_passed("String");

    string_drop(tim);
}

int main()
{
    size_t len = _cstr_len("Tim Millard");
    assert(len == 11);

    test_string();

    Str tim = cstr("Tim Millard");
    assert(tim.len == 11);

    Str hello = cstr("Hello");
    Str world = cstr("World");

    // String hello_world = str_concat(hello, world);
    // assert(hello_world.len == 11);

    printf("******* All tests passed *******\n");

    return 0;
}
