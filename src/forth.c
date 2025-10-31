#include "forth.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHARSET_SIZE 256

char* my_string_tokenizer(char* string, const char* delimiter) {
    static char* last = NULL;
    static bool delimiter_dict[CHARSET_SIZE];
    static char prev_delimiter[CHARSET_SIZE];

    if (!prev_delimiter[0] || strcmp(prev_delimiter, delimiter) != 0) {
        memset(delimiter_dict, false, sizeof(delimiter_dict));
        for (const unsigned char* p = (const unsigned char*)delimiter; *p; p++)
            delimiter_dict[*p] = true;
        strncpy(prev_delimiter, delimiter, sizeof(prev_delimiter) - 1);
        prev_delimiter[sizeof(prev_delimiter) - 1] = '\0';
    }

    if (string)
        last = string;
    if (!last)
        return NULL;

    while (*last && delimiter_dict[(unsigned char)*last])
        last++;

    if (*last == '\0') {
        last = NULL;
        return NULL;
    }

    char* token_start = last;

    while (*last && !delimiter_dict[(unsigned char)*last])
        last++;

    if (*last) {
        *last = '\0';
        last++;
    } else {
        last = NULL;
    }

    return token_start;
}

void* x_malloc(const size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Out of memory allocating %zu bytes\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

int is_symbol_object(const struct forth_object* obj) {
    return obj->forth_type == TYPE_SYMBOL;
}

int is_string_object(const struct forth_object* obj) {
    return obj->forth_type == TYPE_STR;
}

int is_integer_object(const struct forth_object* obj) {
    return obj->forth_type == TYPE_INT;
}

int is_list_object(const struct forth_object* obj) {
    return obj->forth_type == TYPE_LIST;
}

struct forth_object* create_object(const enum forth_types type) {
    struct forth_object* o = x_malloc(sizeof(struct forth_object));
    o->forth_type = type;
    o->ref_count = 1;
    return o;
}

struct forth_object* create_string_object(const char* s, const size_t len) {
    struct forth_object* o = create_object(TYPE_STR);
    o->str.ptr = malloc(len + 1);
    memcpy(o->str.ptr, s, len);
    o->str.ptr[len] = '\0';
    o->str.len = len;
    return o;
}

struct forth_object* create_int_object(const int i) {
    struct forth_object* o = create_object(TYPE_INT);
    o->i = i;
    return o;
}

struct forth_object* create_bool_object(const bool b) {
    struct forth_object* o = create_object(TYPE_BOOL);
    o->i = b;
    return o;
}

struct forth_object* create_symbol_object(char* s, const size_t len) {
    struct forth_object* o = create_object(TYPE_SYMBOL);
    o->str.ptr = malloc(len + 1);
    memcpy(o->str.ptr, s, len);
    o->str.ptr[len] = '\0';
    o->str.len = len;
    return o;
}

struct forth_object* create_list_object() {
    struct forth_object* o = create_object(TYPE_LIST);
    o->list.ele = NULL;
    o->list.len = 0;
    return o;
}

struct forth_object* try_symbol_parsing(char* part) {
    struct forth_object* new_obj = NULL;
    char* known_symbols[KNOWN_SYMBOLS_LEN] = KNOWN_SYMBOLS;
    for (int i=0; i<KNOWN_SYMBOLS_LEN; i++) {
        char* symbol = known_symbols[i];
        if (strncmp(part, symbol, strlen(symbol)) == 0) {
            return create_symbol_object(part, strlen(part));
        }
    }
    return new_obj;
}

struct forth_object* try_int_parsing(char* part) {
    struct forth_object* new_obj = NULL;
    size_t n_checks = 0;
    for (int i=0; i<strlen(part); i++) {
        if (!isdigit(part[i])) break;
        n_checks++;
    }
    if (n_checks == strlen(part)) {
        return create_int_object((int) strtol(part, NULL, 10));
    }
    return new_obj;
}

struct forth_object* compile(char* prg_text) {

    struct forth_object* obj = create_list_object();
    char* part = my_string_tokenizer(prg_text, " ");

    while (part != NULL) {
        printf("extracting part %s\n", part);
        struct forth_object* new_obj = NULL;

        new_obj = try_symbol_parsing(part);

        if (new_obj == NULL) {
            new_obj = try_int_parsing(part);
        }

        if (new_obj == NULL) {
            new_obj = create_string_object(part, strlen(part));
        }

        size_t new_size = (obj->list.len + 1) * sizeof(*obj->list.ele);
        obj->list.ele = realloc(obj->list.ele, new_size);
        obj->list.ele[obj->list.len] = new_obj;
        obj->list.len += 1;
        part = my_string_tokenizer(NULL, " ");
    }
    return obj;
}

void execute(struct forth_context* ctx) {
    for (size_t i=0; i<ctx->stack->list.len; i++) {
        const struct forth_object* el = ctx->stack->list.ele[i];
        if (is_string_object(el)) printf("executing string element %s\n", el->str.ptr);
        else if (is_symbol_object(el)) printf("executing symbol element %s\n", el->str.ptr);
        else if (is_integer_object(el)) printf("executing integer element %d\n", el->i);
        else printf("unknown symbol type %d\n", el->forth_type);
    }
}
