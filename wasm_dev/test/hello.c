/** Compile:
 * $ emcc -O3 -s WASM=1 -s EXPORTED_RUNTIME_METHODS='["cwrap"]' .\hello.c
 *
 * Reference:
 * - https://developer.mozilla.org/ja/docs/WebAssembly/existing_C_to_wasm
 */

#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h> // emcc

char *msg = NULL;

EMSCRIPTEN_KEEPALIVE
void print_hello(void)
{
    printf("Hello WASM!\n");
}

EMSCRIPTEN_KEEPALIVE
char *get_msg(void)
{
    return "WASM";
}

EMSCRIPTEN_KEEPALIVE
void free_hello(void)
{
    printf("Free: '%s'\n", msg);
    free(msg);
}

EMSCRIPTEN_KEEPALIVE
const char *hello(const char *target)
{
    if (msg)
    {
        printf("Free: '%s'\n", msg);
        free(msg);
    }

    msg = malloc(BUFSIZ * sizeof(char));
    sprintf(msg, "Hello %s!", target);
    return msg;
}
