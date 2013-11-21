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

/* Functions to calculate the minimum sizes for destination arrays. */
size_t encoded_dest_size(const size_t src_len, const size_t line_size);
size_t decoded_dest_size(const size_t src_len);

/* Functions for encoding from binary to base64. */
char encode_sextet(const char ch);
size_t encode_array(const char *src_array, const size_t src_len,
                    char *dest_array, const size_t dest_size,
                    const size_t line_size);

/* Functions for decoding from base64 to binary. */
unsigned char decode_base64_pair1(const char ch1, const char ch2);
unsigned char decode_base64_pair2(const char ch2, const char ch3);
unsigned char decode_base64_pair3(const char ch3, const char ch4);
size_t array_from_base64(const char *src_array, const size_t src_len,
                         char *dest_array, const size_t dest_len);

#endif /* PS8_MYB64_H */
