#include <stdio.h>
#include <stdlib.h>

#include "forth.h"

int main(const int argc, const char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL) {
        fprintf(stderr, "file %s does not exist\n", filename);
        return EXIT_FAILURE;
    }

    char* buffer = malloc(1024);
    if (fgets(buffer, 1024, fptr) == NULL) {
        fprintf(stderr, "Unable to read file %s\n", filename);
        free(buffer);
        return EXIT_FAILURE;
    }

    struct forth_context* ctx = malloc(sizeof(struct forth_context));
    ctx->stack = malloc(sizeof(struct forth_object));
    struct forth_object* obj = compile(buffer);
    ctx->stack = obj;
    free(buffer);

    execute(ctx);
    return EXIT_SUCCESS;
}
