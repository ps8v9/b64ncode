/*
 * myb64.c : C functions for encoding and decoding base64.
 *
 * AUTHOR  : Matthew J. Fisher
 * REPO    : https://github.com/ps8v9/myb64
 * LICENSE : This work is released into the public domain.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Functions to calculate the minimum sizes for destination arrays. */
size_t encoded_dest_size(const size_t src_len, const size_t line_len);
size_t decoded_dest_size(const size_t src_len);

/* Functions for encoding from binary to base64. */
char  sextet_to_base64(const char ch);
size_t array_to_base64(const char *src, const size_t src_len,
                       char *dest, const size_t dest_len,
                       const size_t line_len);

/* Functions for decoding from base64 to binary. */
unsigned char base64_pair1_to_octet(const char ch1, const char ch2);
unsigned char base64_pair2_to_octet(const char ch2, const char ch3);
unsigned char base64_pair3_to_octet(const char ch3, const char ch4);
size_t array_from_base64(const char *src, const size_t src_len,
                         char *dest, const size_t dest_len);

/* A main function to test the encoding/decoding functions. */
int main()
{
    /* Hardcoded filenames. Edit as needed. */
    const char *binary_file  = "Ps_04_Dunferlime.mp3";
    const char *encoded_file = "Ps_04_Dunferlime.mp3.encoded.txt";
    const char *decoded_file = "Ps_04_Dunferlime.mp3.decoded.mp3";

    /* Hardcoded sizes. Adjust as needed. */
    const size_t binary_src_size = 12 * 1024 * 1024;
    const size_t base64_src_size = 16 * 1024 * 1024;
    const size_t line_len = 72; /* Used when encoding base64; 0 = unbounded. */

    /* Data files, arrays, and variable sizes. */
    FILE *src_file;   /* File that contains data to be encoded/decoded.   */
    FILE *dest_file;  /* File where encoded/decoded data will be written. */
    char *src;        /* Array to hold the data to be encoded/decoded.    */
    char *dest;       /* Array to hold the encoded/decoded data.          */
    size_t src_size;  /* Number of bytes to allocate for src.             */
    size_t dest_size; /* Number of bytes to allocate for dest.            */
    size_t src_len;   /* Actual length of the data to be encoded/decoded. */
    size_t cnt;       /* A counter used for various purposes.             */

    /* Variables for performance testing. */
    clock_t clock_start;
    clock_t clock_diff;

    /* Allocate the src array. */
    src_size = binary_src_size;
    src = (char *)malloc(src_size);
    if (!src) return 1;

    /* Read binary data from a file into src. */
    src_file = fopen(binary_file, "rb");
    if (!src_file) return 2;
    src_len = fread(src, 1, src_size, src_file);
    fclose(src_file);

    /* Allocate the dest array. */
    dest_size = encoded_dest_size(src_len, line_len);
    dest = (char *)malloc(dest_size);
    if (!dest) return 3;

    /* Encode data from src (binary) into dest (NULL-terminated base64). */
    clock_start = clock();
    cnt = array_to_base64(src, src_len, dest, dest_size, line_len);
    clock_diff = clock() - clock_start;
    free(src);
    src = NULL;

    printf("Encoded %ld bytes into %ld bytes in %f seconds\n",
           src_len, cnt, ((float)clock_diff) / CLOCKS_PER_SEC);

    /* Write NULL-terminated base64-encoded data from dest to a text file. */
    dest_file = fopen(encoded_file, "w");
    if (!dest_file) return 4;
    fwrite(dest, 1, cnt, dest_file);
    fclose(dest_file);
    free(dest);
    dest = NULL;

    /* Reallocate the src array. */
    src_size = base64_src_size;
    src = (char *)malloc(src_size + 1);
    if (!src) return 5;

    /* Read base64-encoded data from a text file into src. */
    src_file = fopen(encoded_file, "r");
    if (!src_file) return 6;
    src_len = fread(src, 1, src_size, src_file);
    src[src_len] = '\0';
    fclose(src_file);

    /* Reallocate the dest array. */
    dest_size = encoded_dest_size(src_len, line_len);
    dest = (char *)malloc(dest_size);
    if (!dest) return 7;

    /* Decode data from src (NULL-terminated base64) into dest (binary). */
    clock_start = clock();
    cnt = array_from_base64(src, src_len, dest, dest_size);
    clock_diff = clock() - clock_start;
    free(src);
    src = NULL;

    printf("Decoded %ld bytes into %ld bytes in %f seconds\n",
           src_len, cnt, ((float)clock_diff) / CLOCKS_PER_SEC);

    /* Write decoded data from dest to a binary file. */
    dest_file = fopen(decoded_file, "wb");
    if (!dest_file) return 8;
    fwrite(dest, 1, cnt, dest_file);
    fclose(dest_file);
    free(dest);
    dest = NULL;

    return 0;
}

/*
 * encoded_dest_size:  Calculate the minimum required size for a destination
 *                     array to hold the results of encoding to base64.
 */
size_t encoded_dest_size(const size_t src_len, const size_t line_len)
{
	/* 4 times the number of 3-byte blocks in the binary data (rounded up).*/
	size_t dest_size = (size_t)4 * ceil(src_len / 3.0);
	if (line_len) {
	    dest_size += dest_size / line_len; /* 1 LF for each full line. */
	    if (dest_size % line_len)
            ++dest_size; /* 1 LF for the underfull last line. */
    }
    return ++dest_size;  /* 1 byte for the NULL terminator */
}

/*
 * decoded_dest_size:  Calculate the minimum required size for a destination
 *                     array to hold the results of decoding from base64.
 */
size_t decoded_dest_size(const size_t src_len)
{
	/* 3 times the number of 4-byte blocks in the base64 (rounded up). */
	/* TODO: Deduct 1 byte for each of up to 2 padding characters.     */
    return (size_t)3 * ceil(src_len / 4.0);
}

/*
 * sextet_to_base64:  Encode a single sextet to a base64 character.
 */
char sextet_to_base64(const char ch)
{
    if   ( 0 <= ch && ch <= 25) return ch + 65; /* 'A' through 'Z' */
    if   (26 <= ch && ch <= 51) return ch + 71; /* 'a' through 'z' */
    if   (52 <= ch && ch <= 61) return ch - 4;  /* '0' through '9' */
    if   (ch == 62)             return '+';
    else                        return '/';
}

/*
 * array_to_base64:  Encode from src (an array of binary bytes) into dest (a
 *                   NULL-terminated array of characters). If a valid line_len
 *                   is specified, LF linebreaks will be included in dest.
 *                   Returns the number of base64 characters placed into dest,
 *                   or -1 on error.
 */
size_t array_to_base64(const char *src, const size_t src_len,
                       char *dest, const size_t dest_size,
                       const size_t line_len)
{
    int    octet[3];
    char   sextet[4];
    int    i;
    int    j;
    size_t cnt;

    if (src_len < 0 || dest_size < 1 || line_len < 0)
        return -1; /* Bad input. */

    if (line_len % 4 != 0)
        return -1; /* Some decoders require line length to be a multiple of 4. */

    for (i = 0, cnt = 0; i < src_len; i += 3) {
        /* Get next 3 octets. Pad with zeroes as needed. */
        octet[0] = src[i];
        octet[1] = i + 1 < src_len ? src[i + 1] : 0;
        octet[2] = i + 2 < src_len ? src[i + 2] : 0;

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
        dest[cnt++] = sextet_to_base64(sextet[0]);
        dest[cnt++] = sextet_to_base64(sextet[1]);
        dest[cnt++] = i + 1 < src_len ? sextet_to_base64(sextet[2]) : '=';
        dest[cnt++] = i + 2 < src_len ? sextet_to_base64(sextet[3]) : '=';

        if (line_len > 0)
          /* Insert an LF if current line is full, or is the last line. */
          if ((cnt + 1) % (line_len + 1) == 0 || cnt == dest_size - 1)
            dest[cnt++] = '\n';
    }

    dest[cnt] = '\0'; /* The null terminator is appended but not counted. */

    return cnt;
}

/*
 * base64_pair1_to_octet:  Decode the first adjacent pair of base64 characters
 *                         from a 4-byte block.
 */
unsigned char base64_pair1_to_octet(const char ch1, const char ch2)
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
 * base64_pair1_to_octet:  Decode the second adjacent pair of base64 characters
 *                         from a 4-byte block.
 */
unsigned char base64_pair2_to_octet(const char ch2, const char ch3)
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
 * base64_pair1_to_octet:  Decode the third adjacent pair of base64 characters
 *                         from a 4-byte block.
 */
unsigned char base64_pair3_to_octet(const char ch3, const char ch4)
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
 * array_from_base64:  Decode from src (an array of base64 characters) into
 *                     dest (an array of binary bytes). Returns the number of
 *                     bytes that were placed into dest, or -1 on error.
 */
size_t array_from_base64(const char *src, const size_t src_len,
                         char *dest, const size_t dest_size)
{
    int cnt = 0;

    char ch[4];
    int i = 0;
    int j = 0;

    if (dest_size < decoded_dest_size(src_len))
        return -1; /* Caller didn't give us enough space. */

    while (i < src_len) {
        /* Eat CRs and LFs. */
        while ((src[i] == '\r' || src[i] == '\n') && i < src_len) {
            ++i;
        }

        if (i == src_len)
            break; /* done decoding */

        /* Get a 4-character block of base64. Eat embedded CRs and LFs. */
        for (j = 0; j < 4; ++j) {
            do {
                ch[j] = src[i++];
            } while ((ch[j] == '\r' || ch[j] == '\n') && i < src_len);

            if (ch[j] == '\r' || ch[j] == '\n')
                return -1; /* bad data */
        }

        /* Decode the block. Skip padding characters. */
        dest[cnt++] = base64_pair1_to_octet(ch[0], ch[1]);
        if (ch[2] != '=')
            dest[cnt++] = base64_pair2_to_octet(ch[1], ch[2]);
        if (ch[3] != '=')
            dest[cnt++] = base64_pair3_to_octet(ch[2], ch[3]);
    }

    return cnt;
}
