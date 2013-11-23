/*
 * stdin_test.c : Test program for encoding/decoding base64 using stdin/stdout.
 *
 * AUTHOR  : Matthew J. Fisher
 * REPO    : https://github.com/ps8v9/b64ncode
 * LICENSE : This is free and unencumbered software released into the public
 *           domain. See the LICENSE file for further details.
 */
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#include "b64ncode.h"

extern char binary_block[];
extern char base64_block[];

int main(int argc, char *argv[])
{
#if _WIN32
/* Prevent Windows from doing ANSI C line ending translations. */
/* see: http://oldwiki.mingw.org/index.php/binary              */
/*      http://msdn.microsoft.com/en-us/library/tw4k6df8.aspx  */
    _setmode(_fileno(stdin),  _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stderr), _O_BINARY);
#endif

    /* Variables for performance testing. */
    clock_t clock_start;
    clock_t clock_diff;

    /* TODO: Process command line options here (-e to encode, -d to decode). */
    bool encode = true; /* This option is still just hardcoded. */

    int err_code;

    clock_start = clock();

    if (encode)
        err_code = encode_stdin();
    else {
        err_code = decode_stdin();
        if (err_code == 1) {
             fprintf(stderr, "%s: input was not a multiple of 4 "
                     "bytes\n", argv[0]);
            return err_code;
         }
    }

    clock_diff = clock() - clock_start;

    fprintf(stderr, "Encoding took %f seconds\n",
      ((float)clock_diff) / CLOCKS_PER_SEC);

    return 0;
}
