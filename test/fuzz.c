/* Written by u/skeeto from r/C_Programming */

/* Usage:
 * $ mkdir in/
 * $ printf '\x03\x34hello\x04' >in/string-in-array
 * $ afl-gcc -m32 -Os -fsanitize=address,undefined fuzz.c
 * $ afl-fuzz -m800 -i in -o out -- ./a.out
 */

#include "json-build.h"
#include <stdio.h>

int
main(void)
{
    jsonb b[1];
    int i, cmdlen;
    char buf[1 << 15];
    char cmd[1 << 12];

    cmdlen = fread(cmd, 1, sizeof(cmd), stdin);

    jsonb_init(b);
    for (i = 0; i < cmdlen; i++) {
        int c = cmd[i] & 0xff;
        int arg = c / 9;
        long r = 0;
        size_t z = sizeof(buf);

#ifdef EXPLAIN
        printf("%02x:%3d%3d\n", c, c % 9, c / 9);
#endif

        switch (c % 9) {
        case 0:
            r = jsonb_push_object(b, buf, z);
            break;
        case 1:
            r = jsonb_pop_object(b, buf, z);
            break;
        case 2:
            if (arg > cmdlen - i - 1) {
                r = JSONB_ERROR_INPUT;
            }
            else {
                r = jsonb_push_key(b, cmd + i + 1, arg, buf, z);
                i += arg;
            }
            break;
        case 3:
            r = jsonb_push_array(b, buf, z);
            break;
        case 4:
            r = jsonb_pop_array(b, buf, z);
            break;
        case 5:
            r = jsonb_push_bool(b, arg % 2, buf, z);
            break;
        case 6:
            r = jsonb_push_null(b, buf, z);
            break;
        case 7:
            if (arg > cmdlen - i - 1) {
                r = JSONB_ERROR_INPUT;
            }
            else {
                r = jsonb_push_string(b, cmd + i + 1, arg, buf, z);
                i += arg;
            }
            break;
        case 8:
            r = jsonb_push_number(b, arg / 28.0, buf, z);
            break;
        }

        if (r < 0) {
            return 1;
        }
    }

    fwrite(buf, b->pos, 1, stdout);
    putchar('\n');
    fflush(stdout);
    return !ferror(stdin) && !ferror(stdout);
}