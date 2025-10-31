#ifndef C_FORTH_FORTH_H
#define C_FORTH_FORTH_H
#include <stddef.h>

#define KNOWN_SYMBOLS_LEN 5
#define KNOWN_SYMBOLS {"+", "-", "*", "/", "DUP"}

enum forth_types {
    TYPE_INT,
    TYPE_STR,
    TYPE_BOOL,
    TYPE_LIST,
    TYPE_SYMBOL
};

struct forth_object {
    int ref_count;
    enum forth_types forth_type;
    union {
        int i;
        struct {
            char* ptr;
            size_t len;
        } str;
        struct {
            struct forth_object** ele;
            size_t len;
        } list;
    };
};

struct forth_parser {
    char* program;
    char* p;
};

struct forth_context {
    struct forth_object* stack;
};

struct forth_object* compile(char* prg_text);
int is_symbol_object(const struct forth_object* obj);
int is_string_object(const struct forth_object* obj);
int is_integer_object(const struct forth_object* obj);
int is_list_object(const struct forth_object* obj);
void execute(struct forth_context* ctx);

#endif