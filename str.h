
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef struct Str {
    char *data;
    size_t len;
} Str;

#define ZERO_STR (Str){NULL, 0};

typedef struct String {
    char *data;
    size_t len;
    size_t cap;
} String;

#define ZERO_STRING (String){NULL, 0, 0};

// String functions
// Dynamic string with data on the heap
String string_make_with_cap(Str s, size_t cap);
String _string_alloc(size_t cap);
size_t string_len(String s);
size_t string_cap(String s);
String string_append(String s, Str data);
String cstring(char *s);
void string_drop(String str);
char *string_to_cstr(String str);

// Str functions
// Immutable string. A dynmaic slice into a string.
Str str_make(String s, unsigned int start, unsigned int end);
size_t str_len(Str s);
String str_concat(Str s1, Str s2);
Str string_to_str(String s);
Str cstr(char *s);
void str_contains(Str haystack, Str needle);
void str_drop(Str str);

size_t cstr_len(char *s);

inline String _string_alloc(size_t cap)
{
    void *data = malloc(sizeof(char) * cap);
    if (!data) {
        return ZERO_STRING;
    }
    return (String){
        .data = (char *) data,
        .len = 0,
        .cap = cap
    };
}

inline String string_make_with_cap(Str s, size_t cap)
{
    String ret = _string_alloc(s.len);
    memcpy(ret.data, s.data, s.len);
    ret.len = s.len;
    return ret;
}

inline String string_make(Str s)
{
    return string_make_with_cap(s, s.len);
}

inline Str str_make(String s, unsigned int start, unsigned int end)
{
    if (start >= s.len || end >= s.len) return ZERO_STR;
    char *ptr = s.data;
    Str new_str = {
        .data = ptr + start,
        .len = (size_t) end - start,
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

inline String string_append_impl(String string, Str str)
{
    String new_string = string;
    size_t require_cap = string.len + str.len;
    if (string.cap < require_cap) {
        size_t double_cap = string.cap * 2;
        size_t new_cap;
        if (require_cap < double_cap) {
            new_cap = double_cap;
        } else {
            new_cap = require_cap;
        }

        void *new_data = realloc(string.data, new_cap);
        if (!new_data) return string;
        new_string.data = (char *)new_data;
        new_string.cap = new_cap;
    }
    new_string.len = string.len + str.len;
    memcpy(&new_string.data[string.len], str.data, str.len);
    return new_string;
}

#define string_append(src, str)                         \
    _Generic(str,                                       \
    const char *: string_append_impl(src, cstr(str)),   \
    default: string_append_impl(src, str))

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
    size_t len = cstr_len(s);
    String new_string = _string_alloc(len);
    memcpy(new_string.data, s, len);
    new_string.len = len;
    return new_string;
}

inline size_t cstr_len(char *s)
{
    char *ptr = s;
    size_t len = 0;
    while (*ptr != '\0') {
        len++;
        ptr++;
    }
    return len;
}

inline void str_drop(Str str)
{
    free(str.data);
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

#include <stdio.h>
inline void string_print(String s)
{
    for (int i = 0; i < s.len; i++) {
        puts(&s.data[i]);
    }
}
