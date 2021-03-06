/*
 * array_test.c : Test program for encoding/decoding base64 using arrays.
 *
 * AUTHOR  : Matthew J. Fisher
 * REPO    : https://github.com/ps8v9/b64ncode
 * LICENSE : This is free and unencumbered software released into the public
 *           domain. See the LICENSE file for further details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "b64ncode.h"

int main()
{
    /* Hardcoded filenames. Edit as needed. */
    const char *binary_file  = "tables.ods";
    const char *encoded_file = "tables.ods.encoded.txt";
    const char *decoded_file = "tables.ods.decoded.ods";

    /* Hardcoded sizes. Adjust as needed. */
    const size_t binary_src_size = 3 * 1024 * 1024;
    const size_t base64_src_size = 4 * 1024 * 1024;
    const size_t line_size = 72; /* Used when encoding base64. */

    /* Data files, arrays, and variable sizes. */
    FILE *src_file;   /* File that contains data to be encoded/decoded.   */
    FILE *dest_file;  /* File where encoded/decoded data will be written. */
    char *src_array;  /* Array to hold the data to be encoded/decoded.    */
    char *dest_array; /* Array to hold the encoded/decoded data.          */
    size_t src_size;  /* Number of bytes to allocate for src_array.       */
    size_t dest_size; /* Number of bytes to allocate for dest_array.      */
    size_t src_len;   /* Actual length of the data to be encoded/decoded. */
    size_t cnt;       /* A counter used for various purposes.             */

    /* Variables for performance testing. */
    clock_t clock_start;
    clock_t clock_diff;

    /* Allocate src_array. */
    src_size = binary_src_size;
    src_array = (char *)malloc(src_size);
    if (!src_array) return 1;

    /* Read binary data from the binary file into src_array. */
    src_file = fopen(binary_file, "rb");
    if (!src_file) return 2;
    src_len = fread(src_array, 1, src_size, src_file);
    fclose(src_file);

    /* Allocate dest_array. */
    dest_size = encoded_dest_size(src_len, line_size);
    dest_array = (char *)malloc(dest_size);
    if (!dest_array) return 3;

    /* Encode data from src_array into dest_array (NULL-terminated base64). */
    clock_start = clock();
    cnt = encode_array(src_array, src_len, dest_array, dest_size, line_size);
    clock_diff = clock() - clock_start;
    free(src_array);
    src_array = NULL;

    printf("Encoded %ld bytes into %ld bytes in %.3f seconds\n",
           src_len, cnt, ((float)clock_diff) / CLOCKS_PER_SEC);

    /* Write NULL-terminated base64 from dest_array to a text file. */
    dest_file = fopen(encoded_file, "w");
    if (!dest_file) return 4;
    fwrite(dest_array, 1, cnt, dest_file);
    fclose(dest_file);
    free(dest_array);
    dest_array = NULL;

    /* Reallocate src_array. */
    src_size = base64_src_size;
    src_array = (char *)malloc(src_size + 1);
    if (!src_array) return 5;

    /* Read base64 data from the encoded file into src_array. */
    src_file = fopen(encoded_file, "r");
    if (!src_file) return 6;
    src_len = fread(src_array, 1, src_size, src_file);
    src_array[src_len] = '\0';
    fclose(src_file);

    /* Reallocate dest_array. */
    dest_size = encoded_dest_size(src_len, line_size);
    dest_array = (char *)malloc(dest_size);
    if (!dest_array) return 7;

    /* Decode data from src_array (base64) into dest_array (binary). */
    clock_start = clock();
    cnt = decode_array(src_array, src_len, dest_array, dest_size);
    clock_diff = clock() - clock_start;
    free(src_array);
    src_array = NULL;

    printf("Decoded %ld bytes into %ld bytes in %.3f seconds\n",
           src_len, cnt, ((float)clock_diff) / CLOCKS_PER_SEC);

    /* Write decoded data in binary mode from dest_array to a file. */
    dest_file = fopen(decoded_file, "wb");
    if (!dest_file) return 8;
    fwrite(dest_array, 1, cnt, dest_file);
    fclose(dest_file);
    free(dest_array);
    dest_array = NULL;

    return 0;
}
