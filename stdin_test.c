/*
 * stdin_test.c : Test program for encoding/decoding base64 using stdin/stdout.
 *
 * AUTHOR  : Matthew J. Fisher
 * REPO    : https://github.com/ps8v9/myb64
 * LICENSE : This is free and unencumbered software released into the public
 *           domain. See the LICENSE file for further details.
 */
#include <stdio.h>
#include <time.h>

#include "myb64.h"

/* A main function to test the encoding/decoding functions. */
int main()
{
    /* Variables for performance testing. */
    clock_t clock_start;
    clock_t clock_diff;
    size_t  get_cnt = 0;
    size_t  put_cnt = 0;

    /* TODO: Process command line options here (-e to encode, -d to decode). */

    clock_start = clock();
    /* TODO: Depending on option, encode or decode data from stdin to stdout. */
    clock_diff = clock() - clock_start;

    printf("Encoded %ld bytes into %ld bytes in %f seconds\n",
           get_cnt, put_cnt, ((float)clock_diff) / CLOCKS_PER_SEC);

    return 0;
}
