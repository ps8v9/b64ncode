/*
 * myb64.h : C functions for encoding and decoding base64.
 *
 * AUTHOR  : Matthew J. Fisher
 * REPO    : https://github.com/ps8v9/myb64
 * LICENSE : This is free and unencumbered software released into the public
 *           domain. See the LICENSE file for further details.
 */
#ifndef PS8_MYB64_H
#define PS8_MYB64_H

#include <stdbool.h>

/* Functions to calculate the minimum sizes for destination arrays. */
size_t encoded_dest_size(const size_t src_len, const size_t line_size);
size_t decoded_dest_size(const size_t src_len);

/* Functions for the lowest-level encoding/decoding operations. */
char encode_sextet(const char ch);
unsigned char decode_base64_pair1(const char ch1, const char ch2);
unsigned char decode_base64_pair2(const char ch2, const char ch3);
unsigned char decode_base64_pair3(const char ch3, const char ch4);

/* Functions for getting/encoding/decoding 24-bit blocks. */
bool get_binary_block();
int  get_base64_block();
int  encode_block();
int  decode_block();

/* Functions for encoding/decoding arrays of arbitrary size. */
size_t encode_array(const char *src_array, const size_t src_len,
                    char *dest_array, const size_t dest_size,
                    const size_t line_size);
size_t decode_array(const char *src_array, const size_t src_len,
                         char *dest_array, const size_t dest_len);

/* Functions for encoding/decoding using stdin/stdout. */
int encode_stdin();
int decode_stdin();

#endif /* PS8_MYB64_H */
