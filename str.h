#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

void *_malloc(size_t size);
inline void *_malloc(size_t size)
{
    return malloc(size);
}

void *_realloc(void *ptr, size_t size);
inline void *_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

// Dynamic String
// Can be resized.
// Compatiable with C Strings.
typedef struct String {
    char *data;
    size_t len;
    size_t cap;
} String;

#define ZERO_STRING (String){NULL, 0, 0};

// String Slice
// Window into a allocated String.
typedef struct Str {
    char *data;
    size_t len;
} Str;

#define ZERO_STR (Str){NULL, 0};

// String functions
// Dynamic string with data on the heap
String string_make(Str s);
String string_make_with_cap(Str s, size_t cap);
String cstring(char *s);
void string_append(String *string, Str str);
size_t string_len(String s);
size_t string_cap(String s);
void string_drop(String str);
char *string_c(String str);
Str string_to_str(String s);
String _string_alloc(size_t cap);
void _string_grow(String *str, size_t cap);
void _string_debug(String str);

// Str functions
// Immutable string. A dynmaic slice into a string.
Str str_make(String s, unsigned int start, unsigned int end);
Str cstr(char *s);
size_t str_len(Str s);
String str_concat(Str s1, Str s2);
void str_contains(Str haystack, Str needle);
void _str_debug(Str str);

size_t _cstr_len(char *s);

inline String _string_alloc(size_t cap)
{
    // Add extra character for null terminated string.
    char *data = (char *) _malloc(sizeof(char) * cap + 1);
    if (!data) {
        return ZERO_STRING;
    }
    data[0] = '\0';
    return (String){
        .data = data,
        .len = 0,
        .cap = cap + 1,
    };
}

inline String string_make_with_cap(Str s, size_t cap)
{
    String ret = _string_alloc(s.len);
    memcpy(ret.data, s.data, s.len);
    ret.len = s.len;
    ret.data[ret.len] = '\0';
    return ret;
}

inline String string_make(Str s)
{
    return string_make_with_cap(s, s.len);
}

inline Str str_make(String s, unsigned int low, unsigned int high)
{
    size_t len = (size_t) high - low;
    if (low >= s.len || len >= s.len) return ZERO_STR;
    char *ptr = s.data + low;
    Str new_str = {
        .data = ptr,
        .len = len,
    };
    return new_str;
}

inline String str_concat(Str s1, Str s2)
{
    size_t len = s1.len + s2.len;
    String new_string = string_make_with_cap(s1, len);
    memcpy(&new_string.data[s1.len], s2.data, s2.len);
    new_string.len = len;
    return new_string;
}

inline void _string_grow(String *str, size_t cap)
{
    if (str == NULL) return;
    if (str->cap >= cap) return;

    size_t double_cap = str->cap * 2;
    size_t new_cap;
    if (cap < double_cap) {
        new_cap = double_cap;
    } else {
        new_cap = cap;
    }
    new_cap++; // Add extra space for null terminator.

    void *new_data = _realloc(str->data, new_cap);
    if (!new_data) return;
    str->data = (char *)new_data;
    str->cap = new_cap;
}

inline void string_append(String *string, Str str)
{
    if (string == NULL) return;

    size_t len = string->len + str.len;
    _string_grow(string, len);
    char *start = (char *)&string->data[string->len];
    memcpy(start, str.data, str.len);
    string->len = len;
    string->data[len] = '\0';
}

// #define string_append(src, str)                         \
//     _Generic(str,                                       \
//     const char *: string_append_impl(src, cstr(str)),   \
//     default: string_append_impl(src, str))

inline Str cstr(char *s)
{
    char *ptr = s;
    size_t len = 0;
    while (*ptr != '\0') {
        len++;
        ptr++;
    }
    return (Str){
        .data = s,
        .len = len
    };
}

inline String cstring(char *s)
{
    size_t len = _cstr_len(s);
    String new_string = _string_alloc(len);
    memcpy(new_string.data, s, len);
    new_string.len = len;
    return new_string;
}

inline size_t _cstr_len(char *s)
{
    char *ptr = s;
    size_t len = 0;
    while (*ptr != '\0') {
        len++;
        ptr++;
    }
    return len;
}

inline void string_drop(String str)
{
    free(str.data);
}

inline size_t str_len(Str s)
{
    return s.len;
}

inline size_t string_len(String s)
{
    return s.len;
}

inline size_t string_cap(String s)
{
    return s.cap;
}

inline Str string_to_str(String s)
{
    return (Str){
        .data = s.data,
        .len = s.len,
    };
}

inline char *string_c(String str)
{
    return str.data;
}

inline void _string_debug(String str)
{
    printf("cstring = %s\n", string_c(str));
    printf("len = %ld\n", str.len);
    printf("cap = %ld\n", str.cap);

    putchar('|');
    for (int i = 0; i < str.cap; i++) {
        putchar(' ');
        if (str.data[i] == '\0')
            putchar('@');
        else
            putchar(str.data[i]);
        putchar(' ');
        putchar('|');
    }
    putchar('\n');
}

inline void _str_debug(Str str)
{
    printf("len = %ld\n", str.len);

    putchar('|');
    for (int i = 0; i < str.len; i++) {
        putchar(' ');
        if (str.data[i] == '\0')
            putchar('@');
        else
            putchar(str.data[i]);
        putchar(' ');
        putchar('|');
    }
    putchar('\n');
}

