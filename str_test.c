#include <stdio.h>
#include <assert.h>

#include "str.h"

void test_passed(char *name)
{
    printf("*** Test %s passed ***\n", name);
}

        //String: "",  string_c(str),          
void test_string()
{
    String tim = cstring("Tim");
    printf("tim = \"%s\"\n", string_c(tim));
    printf("tim.len = %ld\n",  tim.len);
    printf("tim.cap = %ld\n",  tim.cap);
    assert(tim.len == 3);
    assert(tim.cap == 4);

    Str w = cstr(" W");
    String millard = cstring(" Millard");
    string_append(&tim, w);
    string_append(&tim, millard);
    string_append(&tim, " loves C");
    printf("tim = %s\n", string_c(tim));
    printf("tim.len = %ld\n",  tim.len);
    printf("tim.cap = %ld\n",  tim.cap);
    assert(tim.len == 21);
    assert(tim.cap == 39); 

    test_passed("String");

    string_drop(tim);
    string_drop(millard);
}

void test_str()
{
    Str tim = cstr("Tim Millard");
    assert(tim.len == 11);

    Str hello = cstr("Hello");
    Str world = cstr("World");

    String hello_world = str_concat(hello, world);
    printf("hello_world.len = %ld\n", hello_world.len);
    assert(hello_world.len == 10);
   
    Str loW = str_make(hello_world, 3, 6);
    assert(loW.len == 3);
    String low = string_make(loW);
    printf("low = \"%s\"\n", string_c(low));
   
    _str_debug(loW);

    test_passed("Str");

    string_drop(hello_world);
    string_drop(low);
}

int main()
{
    size_t len = _cstr_len("Tim Millard");
    assert(len == 11);

    test_string();
    test_str();

    printf("******* All tests passed *******\n");

    return 0;
}
