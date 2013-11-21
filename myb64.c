/*
 * myb64.c : C functions for encoding and decoding base64.
 *
 * AUTHOR  : Matthew J. Fisher
 * REPO    : https://github.com/ps8v9/myb64
 * LICENSE : This is free and unencumbered software released into the public
 *           domain. See the LICENSE file for further details.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "myb64.h"

/*
 * encoded_dest_size: Calculate the minimum required size for a destination
 *                    array to hold the results of encoding to base64.
 */
size_t encoded_dest_size(const size_t src_len, const size_t line_size)
{
    /* 4 times the number of 3-byte blocks in the binary data (rounded up).*/
    size_t dest_size = (size_t)4 * ceil(src_len / 3.0);
    if (line_size) {
        dest_size += dest_size / line_size; /* 1 LF for each full line. */
        if (dest_size % line_size)
            ++dest_size;                    /* 1 LF for underfull last line. */
    }
    return ++dest_size;                     /* 1 byte for the NULL terminator */
}

/*
 * decoded_dest_size: Estimate the minimum required size for a destination
 *                    array to hold the results of decoding from base64.
 */
size_t decoded_dest_size(const size_t src_len)
{
    /* Assuming no linebreaks or padding, max number of base64 blocks is: */
    size_t blocks = ceil(src_len / 4.0);

    /* Ratio of binary blocks : base64 blocks = 1:1. Binary block length = 3. */
    return blocks * 3;
}

/*
 * encode_sextet: Encode a single sextet to a base64 character.
 */
char encode_sextet(const char ch)
{
    if   ( 0 <= ch && ch <= 25) return ch + 65; /* 'A' through 'Z' */
    if   (26 <= ch && ch <= 51) return ch + 71; /* 'a' through 'z' */
    if   (52 <= ch && ch <= 61) return ch - 4;  /* '0' through '9' */
    if   (ch == 62)             return '+';
    else                        return '/';
}

/*
 * decode_base64_pair1: Decode the 1st adjacent pair of base64 characters from a
 *                      4-byte block.
 */
unsigned char decode_base64_pair1(const char ch1, const char ch2)
{
    unsigned char octet;

    if      ('A' <= ch1 && ch1 <= 'Z') octet = 4 * (ch1 - 'A');
    else if ('a' <= ch1 && ch1 <= 'z') octet = 4 * (ch1 - 'a' + 26);
    else if ('0' <= ch1 && ch1 <= '9') octet = 4 * (ch1 - '0' + 52);
    else if (ch1 == '+')               octet = 248;
    else if (ch1 == '/')               octet = 252;

    if      ('A' <= ch2 && ch2 <= 'P') ;
    else if ('Q' <= ch2 && ch2 <= 'Z') octet += 1;
    else if ('a' <= ch2 && ch2 <= 'f') octet += 1;
    else if ('g' <= ch2 && ch2 <= 'v') octet += 2;
    else if ('w' <= ch2 && ch2 <= 'z') octet += 3;
    else if ('0' <= ch2 && ch2 <= '9') octet += 3;
    else if (ch2 == '+')               octet += 3;
    else if (ch2 == '/')               octet += 3;
    else                               ; /* '=' assumed */

    return octet;
}

/*
 * decode_base64_pair1: Decode the 2nd adjacent pair of base64 characters from a
 *                      4-byte block.
 */
unsigned char decode_base64_pair2(const char ch2, const char ch3)
{
    unsigned char octet;

    if      ('A' <= ch2 && ch2 <= 'P') octet = 16 * (ch2 - 'A');
    else if ('Q' <= ch2 && ch2 <= 'Z') octet = 16 * (ch2 - 'Q');
    else if ('a' <= ch2 && ch2 <= 'f') octet = 16 * (ch2 - 'a' + 10);
    else if ('g' <= ch2 && ch2 <= 'v') octet = 16 * (ch2 - 'g');
    else if ('w' <= ch2 && ch2 <= 'z') octet = 16 * (ch2 - 'w');
    else if ('0' <= ch2 && ch2 <= '9') octet = 16 * (ch2 - '0' + 4);
    else if (ch2 == '+')               octet = 224;
    else if (ch2 == '/')               octet = 240;
    else                               octet = 0; /* '=' assumed */

    if      ('A' <= ch3 && ch3 <= 'D') ;
    else if ('E' <= ch3 && ch3 <= 'H') octet += 1;
    else if ('I' <= ch3 && ch3 <= 'L') octet += 2;
    else if ('M' <= ch3 && ch3 <= 'P') octet += 3;
    else if ('Q' <= ch3 && ch3 <= 'T') octet += 4;
    else if ('U' <= ch3 && ch3 <= 'X') octet += 5;
    else if ('Y' <= ch3 && ch3 <= 'Z') octet += 6;
    else if ('a' <= ch3 && ch3 <= 'b') octet += 6;
    else if ('c' <= ch3 && ch3 <= 'f') octet += 7;
    else if ('g' <= ch3 && ch3 <= 'j') octet += 8;
    else if ('k' <= ch3 && ch3 <= 'n') octet += 9;
    else if ('o' <= ch3 && ch3 <= 'r') octet += 10;
    else if ('s' <= ch3 && ch3 <= 'v') octet += 11;
    else if ('w' <= ch3 && ch3 <= 'z') octet += 12;
    else if ('0' <= ch3 && ch3 <= '3') octet += 13;
    else if ('4' <= ch3 && ch3 <= '7') octet += 14;
    else if ('8' <= ch3 && ch3 <= '9') octet += 15;
    else if (ch3 == '+')               octet += 15;
    else if (ch3 == '/')               octet += 15;
    else                               ; /* '=' assumed */

    return octet;
}

/*
 * decode_base64_pair1: Decode the 3rd adjacent pair of base64 characters from a
 *                      4-byte block.
 */
unsigned char decode_base64_pair3(const char ch3, const char ch4)
{
    unsigned char octet;

    switch (ch3) {
        case 'A': case 'E': case 'I': case 'M': case 'Q': case 'U': case 'Y':
        case 'c': case 'g': case 'k': case 'o': case 's': case 'w': case '0':
        case '4': case '8':
            octet = 0;
            break;
        case 'B': case 'F': case 'J': case 'N': case 'R': case 'V': case 'Z':
        case 'd': case 'h': case 'l': case 'p': case 't': case 'x': case '1':
        case '5': case '9':
            octet = 64;
            break;
        case 'C': case 'G': case 'K': case 'O': case 'S': case 'W': case 'a':
        case 'e': case 'i': case 'm': case 'q': case 'u': case 'y': case '2':
        case '6': case '+':
            octet = 128;
            break;
        case 'D': case 'H': case 'L': case 'P': case 'T': case 'X': case 'b':
        case 'f': case 'j': case 'n': case 'r': case 'v': case 'z': case '3':
        case '7': case '/':
            octet = 192;
            break;
        default: /* '=' assumed */
            octet = 0;
            break;
    }

    if      ('A' <= ch4 && ch4 <= 'Z') octet += ch4 - 'A';
    else if ('a' <= ch4 && ch4 <= 'z') octet += ch4 - 'a' + 26;
    else if ('0' <= ch4 && ch4 <= '9') octet += ch4 - '0' + 52;
    else if (ch4 == '+')               octet += 62;
    else if (ch4 == '/')               octet += 63;
    else                               ; /* '=' assumed */

    return octet;
}

/*
 * encode_array: Encode from src_array (binary bytes) into dest_array (NULL-
 *               terminated base64). If a valid line_size is specified, LF
 *               linebreaks will be included in dest_array. Returns the number
 *               of base64 characters placed into dest_array, or -1 on error.
 */
size_t encode_array(const char *src_array, const size_t src_len,
                       char *dest_array, const size_t dest_size,
                       const size_t line_size)
{
    int    octet[3];
    char   sextet[4];
    int    i;
    int    j;
    size_t cnt;

    if (src_len < 0 || dest_size < 1 || line_size < 0)
        return -1; /* Bad input. */

    if (line_size % 4 != 0)
        return -1; /* Some decoders require lines to be a multiple of 4. */

    for (i = 0, cnt = 0; i < src_len; i += 3) {
        /* Get next 3 octets. Pad with zeroes as needed. */
        octet[0] = src_array[i];
        octet[1] = i + 1 < src_len ? src_array[i + 1] : 0;
        octet[2] = i + 2 < src_len ? src_array[i + 2] : 0;

        /* Convert two's complement bit patterns to signed positive. */
        for (j = 0; j < 3; j++)
            if (octet[j] < 0)
                octet[j] += 256;

        /* Expand 3 right-aligned octets to 4 right-aligned sextets. */
        sextet[0] = octet[0] / 4;
        sextet[1] = ((octet[0] % 4) * 16) + (octet[1] / 16);
        sextet[2] = ((octet[1] % 16) * 4) + (octet[2] / 64);
        sextet[3] = octet[2] % 64;

        /* Encode each sextet as a base64 character. Pad as needed. */
        dest_array[cnt++] = encode_sextet(sextet[0]);
        dest_array[cnt++] = encode_sextet(sextet[1]);
        dest_array[cnt++] = i + 1 < src_len ? encode_sextet(sextet[2]) : '=';
        dest_array[cnt++] = i + 2 < src_len ? encode_sextet(sextet[3]) : '=';

        if (line_size > 0)
          /* Insert an LF if current line is full, or is the last line. */
          if ((cnt + 1) % (line_size + 1) == 0 || cnt == dest_size - 2)
            dest_array[cnt++] = '\n';
    }

    dest_array[cnt] = '\0'; /* NULL terminator is appended but not counted. */

    return cnt;
}

/*
 * decode_array: Decode from src_array (base64) into dest_array (binary).
 *               Returns the number of bytes that were placed into dest_array,
 *               or -1 on error.
 */
size_t decode_array(const char *src_array, const size_t src_len,
                    char *dest_array, const size_t dest_size)
{
    int cnt = 0;

    char ch[4];
    int i = 0;
    int j = 0;

    if (dest_size < decoded_dest_size(src_len))
        return -1; /* Caller didn't give us enough space. */

    while (i < src_len) {
        /* Eat CRs and LFs. */
        while ((src_array[i] == '\r' || src_array[i] == '\n') && i < src_len) {
            ++i;
        }

        if (i == src_len)
            break; /* done decoding */

        /* Get a 4-character block of base64. Eat embedded CRs and LFs. */
        for (j = 0; j < 4; ++j) {
            do {
                ch[j] = src_array[i++];
            } while ((ch[j] == '\r' || ch[j] == '\n') && i < src_len);

            if (ch[j] == '\r' || ch[j] == '\n')
                return -1; /* bad data */
        }

        /* Decode the block. Skip padding characters. */
        dest_array[cnt++] = decode_base64_pair1(ch[0], ch[1]);
        if (ch[2] != '=')
            dest_array[cnt++] = decode_base64_pair2(ch[1], ch[2]);
        if (ch[3] != '=')
            dest_array[cnt++] = decode_base64_pair3(ch[2], ch[3]);
    }

    return cnt;
}
