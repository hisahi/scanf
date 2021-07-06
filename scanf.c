/*

scanf implementation
Copyright (C) 2021 Sampo Hippeläinen (hisahi)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <limits.h>
#include <stddef.h>

#include "scanf.h"

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
    || (defined(__cplusplus) && __cplusplus >= 201103L)
#define SCANF_C99 1
#else
#define SCANF_C99 0
#endif

#ifndef SCANF_STDINT
#if SCANF_C99
#define SCANF_STDINT 1
#elif HAVE_STDINT_H
#define SCANF_STDINT 1
#elif defined(__has_include)
#if __has_include(<stdint.h>)
#define SCANF_STDINT 1
#endif
#endif
#endif

#if SCANF_STDINT
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif
#include <stdint.h>
#else
#if SCANF_C99
#ifndef intmax_t
#define intmax_t long long int
#endif
#ifndef uintmax_t
#define uintmax_t unsigned long long int
#endif
#ifndef INTMAX_MIN
#define INTMAX_MIN LLONG_MIN
#endif
#ifndef INTMAX_MAX
#define INTMAX_MAX LLONG_MAX
#endif
#ifndef UINTMAX_MAX
#define UINTMAX_MAX ULLONG_MAX
#endif
#else
#ifndef intmax_t
#define intmax_t long int
#endif
#ifndef uintmax_t
#define uintmax_t unsigned long int
#endif
#ifndef INTMAX_MIN
#define INTMAX_MIN LONG_MIN
#endif
#ifndef INTMAX_MAX
#define INTMAX_MAX LONG_MAX
#endif
#ifndef UINTMAX_MAX
#define UINTMAX_MAX ULONG_MAX
#endif
#endif
#endif

#ifndef LLONG_MAX
#undef SCANF_DISABLE_SUPPORT_LONG_LONG
#define SCANF_DISABLE_SUPPORT_LONG_LONG 1
#endif

#define maxfloat_t long double

#if SCANF_C99
#define INLINE static inline
#else
#define INLINE static
#endif

#ifndef BOOL
#if defined(__cplusplus)
#define BOOL bool
#elif SCANF_C99
#define BOOL _Bool
#else
#define BOOL int
#endif
#endif

#ifndef SCANF_NOMATH
#define SCANF_NOMATH 0
#endif

#ifndef SCANF_INTERNAL_CTYPE
#define SCANF_INTERNAL_CTYPE 0
#endif

#ifndef SCANF_BINARY
#define SCANF_BINARY 0
#endif

#ifndef SCANF_ATON_BUFFER_SIZE
#if SCANF_BINARY
#define SCANF_ATON_BUFFER_SIZE 72
#else
#define SCANF_ATON_BUFFER_SIZE 32
#endif
#endif

#ifndef SCANF_ATOD_BUFFER_SIZE
#define SCANF_ATOD_BUFFER_SIZE 32
#endif

#ifndef SCANF_ATOF_BUFFER_SIZE
#define SCANF_ATOF_BUFFER_SIZE 48
#endif

#ifndef SCANF_ASCII
#define SCANF_ASCII 1
#endif

#ifndef SCANF_FAST_SCANSET
#if CHAR_BIT == 8
#define SCANF_FAST_SCANSET 1
#else
#define SCANF_FAST_SCANSET 0
#endif
#endif

#ifndef SCANF_NOPOW
#define SCANF_NOPOW 1
#endif

#if !SCANF_ASCII
#undef SCANF_INTERNAL_CTYPE
#define SCANF_INTERNAL_CTYPE 0
#endif

#if !SCANF_NOMATH
#include <math.h>
#endif

#if !SCANF_DISABLE_SUPPORT_FLOAT
#include <float.h>
#endif

#if !SCANF_INTERNAL_CTYPE
#include <ctype.h>
#endif

#ifndef SCANF_INFINITE
#if SCANF_C99 && !SCANF_NOMATH && defined(NAN) && defined(INFINITY)
#define SCANF_INFINITE 1
#else
#define SCANF_INFINITE 0
#endif
#endif

#if SCANF_ASCII
#define INLINE_IF_ASCII INLINE
#else
#define INLINE_IF_ASCII static
#endif

#ifndef EOF
#define EOF -1
#endif

/* try to map size_t to unsigned long long or unsigned long */
#if defined(SIZE_MAX)
#if defined(ULLONG_MAX) && SIZE_MAX == ULLONG_MAX
#define SIZET_ALIAS ll
#elif defined(ULONG_MAX) && SIZE_MAX == ULONG_MAX
#define SIZET_ALIAS l
#endif
#endif

/* try to map ptrdiff_t to long long or long */
#if defined(PTRDIFF_MAX)
#if defined(LLONG_MAX) && PTRDIFF_MAX == LLONG_MAX && PTRDIFF_MIN == LLONG_MIN
#define PTRDIFFT_ALIAS ll
#elif defined(LONG_MAX) && PTRDIFF_MAX == LONG_MAX && PTRDIFF_MIN == LONG_MIN
#define PTRDIFFT_ALIAS l
#endif
#endif

/* try to map intmax_t to unsigned long long or unsigned long */
#if defined(INTMAX_MAX) && defined(UINTMAX_MAX)
#if (defined(LLONG_MAX) && INTMAX_MAX == LLONG_MAX && INTMAX_MIN == LLONG_MIN) \
    && (defined(ULLONG_MAX) && UINTMAX_MAX == ULLONG_MAX)
#define INTMAXT_ALIAS ll
#elif (defined(LONG_MAX) && INTMAX_MAX == LONG_MAX && INTMAX_MIN == LONG_MIN)  \
    && (defined(ULONG_MAX) && UINTMAX_MAX == ULONG_MAX)
#define INTMAXT_ALIAS l
#endif
#endif

INLINE_IF_ASCII int ctodn_(int c) {
#if SCANF_ASCII
    return c - '0';
#else
    switch (c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    default:  return -1;
    }
#endif
}

INLINE_IF_ASCII int ctoon_(int c) {
#if SCANF_ASCII
    return c - '0';
#else
    switch (c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    default:  return -1;
    }
#endif
}

static int ctoxn_(int c) {
#if SCANF_ASCII
    if (c >= 'a')
        return c - 'a' + 10;
    else if (c >= 'A')
        return c - 'A' + 10;
    return c - '0';
#else
    switch (c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'A': case 'a': return 10;
    case 'B': case 'b': return 11;
    case 'C': case 'c': return 12;
    case 'D': case 'd': return 13;
    case 'E': case 'e': return 14;
    case 'F': case 'f': return 15;
    default:  return -1;
    }
#endif
}

#if SCANF_BINARY
INLINE_IF_ASCII int ctobn_(int c) {
#if SCANF_ASCII
    return c - '0';
#else
    switch (c) {
    case '0': return 0;
    case '1': return 1;
    default:  return -1;
    }
#endif
}
#endif

static int ctorn_(int c, int b) {
    switch (b) {
    case 8:
        return ctoon_(c);
    case 16:
        return ctoxn_(c);
#if SCANF_BINARY
    case 2:
        return ctobn_(c);
#endif
    default: /* 10 */
        return ctodn_(c);
    }
}

#if SCANF_INTERNAL_CTYPE
static int isspace(int c) {
    switch (c) {
    case ' ':
    case '\t':
    case '\n':
    case '\v':
    case '\f':
    case '\r':
        return 1;
    }
    return 0;
}

INLINE int isdigit(int c) {
    return '0' <= c && c <= '9';
}

#if SCANF_INFINITE
INLINE int isalpha(int c) {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

INLINE int isalnum(int c) {
    return isdigit(c) || isalpha(c);
}

INLINE int tolower(int c) {
    return isalpha(c) ? c | 0x20 : c;
}
#endif

#endif

INLINE int isdigo_(int c) {
#if SCANF_ASCII
    return '0' <= c && c <= '7';
#else
    return ctoon_(c) >= 0;
#endif
}

INLINE int isdigx_(int c) {
#if SCANF_ASCII
    return isdigit(c) || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
#else
    return ctoxn_(c) >= 0;
#endif
}

#if SCANF_BINARY
INLINE int isdigb_(int c) {
    return c == '0' || c == '1';
}
#endif

INLINE int isdigr_(int c, int b) {
    switch (b) {
    case 8:
        return isdigo_(c);
    case 16:
        return isdigx_(c);
#if SCANF_BINARY
    case 2:
        return isdigb_(c);
#endif
    default: /* 10 */
        return isdigit(c);
    }
}

/* convert unsigned integer in s in base b to integer r.
   returns 0 if OK, returns 1 if overflow (r not changed).
   should return 0 if string empty */
static int atobui_(const unsigned char* s, int b, uintmax_t* r) {
    char c;
    uintmax_t tr = 0, pr = 0;
    while ((c = *s++)) {
        tr *= b;
        if (tr < pr)
            return 1;
        pr = tr;
        tr += ctorn_(c, b);
    }
    *r = tr;
    return 0;
}

/* convert string to unsigned int.
   b = radix, ovf = maximum value (overflow) */
INLINE uintmax_t atobu_(const unsigned char* s, int b, uintmax_t ovf) {
    uintmax_t r;
    return atobui_(s, b, &r) ? ovf : r;
}

/* convert string to signed int. negative flag controls if int is negative.
   b = radix, uvf = minimum value (underflow), ovf = maximum value (overflow) */
INLINE intmax_t atobn_(const unsigned char* s, BOOL negative, int b,
                        intmax_t uvf, intmax_t ovf) {
    uintmax_t r;
    if (atobui_(s, b, &r) || r > (uintmax_t)ovf)
        return negative ? uvf : ovf;
    else
        return negative ? -(intmax_t)r : (intmax_t)r;
}

#if !SCANF_DISABLE_SUPPORT_FLOAT

#if !SCANF_NOPOW
INLINE maxfloat_t pow10_(intmax_t y) {
    return pow(10, y);
}
INLINE maxfloat_t pow2_(intmax_t y) {
    return pow(2, y);
}
#else
INLINE maxfloat_t pow10_(intmax_t y) {
    maxfloat_t r = (maxfloat_t)1;
    for (; y > 0; --y)
        r *= 10;
    return r;
}
INLINE maxfloat_t pow2_(intmax_t y) {
    maxfloat_t r = (maxfloat_t)1;
    for (; y > 0; --y)
        r *= 2;
    return r;
}
#endif

/* s to floating point. negative controls flag, exp is E(+/-), hex = dec/hex..
   should return 0 if string empty */
static maxfloat_t atolf_(const unsigned char* s, BOOL negative, intmax_t exp,
                        BOOL hex) {
    maxfloat_t r = 0;
    int base = hex ? 16 : 10;
    while (isdigit(*s))
        r = r * base + ctodn_(*s++);
    if (*s == '.') {
        int sub = hex ? 4 : 1;
        ++s;
        while (isdigit(*s))
            r = r * base + ctodn_(*s++), exp -= sub;
    }

    if (r != 0) {
        if (exp > 0) {
#ifdef INFINITY
            if (exp > (hex ? LDBL_MAX_EXP : LDBL_MAX_10_EXP))
                r = INFINITY;
            else
#endif
                r *= (hex ? pow2_(exp) : pow10_(exp));
        } else if (exp < 0) {
            if (exp < (hex ? LDBL_MIN_EXP : LDBL_MIN_10_EXP))
                r = 0;
            else
                r /= (hex ? pow2_(-exp) : pow10_(-exp));
        }
    }
    if (negative) r = -r;
    return r;
}

#endif

enum dlength { LN_, LN_hh, LN_h, LN_l, LN_ll, LN_L, LN_j, LN_z, LN_t };
#define vLNa_(x) LN_##x
#define vLN_(x) vLNa_(x)

#define IS_EOF(c) ((c) < 0)
#define GOT_EOF() (IS_EOF(next))

static int iscanf_(int (*getch)(void* p), void (*ungetch)(int c, void* p),
                   void* p, const char* ff, va_list va) {
    /* fields = number of fields successfully read; this is the return value */
    /* next = the "next" character to be processed, initially -1 */
    int fields = 0, next;
    /* total characters read, returned by %n */
    size_t read_chars = 0;
    /* whether we even tried to convert anything */
    BOOL tryconv = 0;
    /* whether we have failed to convert anything successfully.
       match = 0 means matching failure, match = 1 means EOF / input failure
                                                (before any conversion) */
    BOOL match = 1;
    const unsigned char *f = (const unsigned char *)ff;
    unsigned char c;

    /* empty format string always returns 0 */
    if (!*f) return 0;

    /* read and cache first character */
    next = getch(p);
    /* ++read_chars; intentionally left out, otherwise %n is off by 1 */
    while ((c = *f++)) {
        /* still characters to read? (not EOF and width not exceeded) */
#define KEEP_READING() (nowread < maxlen && !GOT_EOF())
        /* read next char and increment counter */
#define NEXT_CHAR(counter) (next = getch(p), ++counter)
        /* signal match OK */
#define MATCH_SUCCESS() (match = 0)
        /* signal match failure and exit loop */
#define MATCH_FAILURE() do { if (!GOT_EOF()) match = 0; \
                             goto read_failure; } while (0)
        /* signal input failure and exit loop */
#define INPUT_FAILURE() do { goto read_failure; } while (0)
        /* store value to ptr with cast */
#define STORE_VALUE(ptr, value, T) (*(T *)(ptr) = (T)(value))

        if (isspace(c)) {
            /* skip 0-N whitespace */
            while (!GOT_EOF() && isspace(next))
                NEXT_CHAR(read_chars);
        } else if (c != '%') {
            if (GOT_EOF()) break;
            /* must match literal character */
            if (next != c) {
                MATCH_FAILURE();
                break;
            }
            NEXT_CHAR(read_chars);
        } else { /* % */
            /* nostore is %*, prevents a value from being stored */
            BOOL nostore = 0;
            /* nowread = characters read for this format specifier
               maxlen = maximum number of characters to be read "field width" */
            size_t nowread = 0, maxlen = 0;
            /* length specifier (l, ll, h, hh...) */
            enum dlength dlen = LN_;
            
            /* nostore */
            if (*f == '*') {
                nostore = 1;
                ++f;
            }

            /* width specifier => maxlen */
            if (isdigit(*f)) {
                unsigned char aton[SCANF_ATOD_BUFFER_SIZE], *di = aton;
                int k;
                while (*f == '0')
                    ++f;
                for (k = 0; isdigit(*f) && k < SCANF_ATOD_BUFFER_SIZE - 1; ++k)
                    *di++ = *f++;
                *di = 0;
                maxlen = (size_t)atobu_(aton, 10, UINTMAX_MAX);
            }

            /* length specifier */
            switch (*f++) {
            case 'h':
                if (*f == 'h')
                    dlen = LN_hh, ++f;
                else
                    dlen = LN_h;
                break;
            case 'l':
#if !SCANF_DISABLE_SUPPORT_LONG_LONG
                if (*f == 'l')
                    dlen = LN_ll, ++f;
                else
#endif
                    dlen = LN_l;
                break;
            case 'j':
#ifdef INTMAXT_ALIAS
                dlen = vLN_(INTMAXT_ALIAS);
#else
                dlen = LN_j;
#endif
                break;
            case 't':
#ifdef PTRDIFFT_ALIAS
                dlen = vLN_(PTRDIFFT_ALIAS);
#else
                dlen = LN_t;
#endif
                break;
            case 'z':
#ifdef SIZET_ALIAS
                dlen = vLN_(SIZET_ALIAS);
#else
                dlen = LN_z;
#endif
                break;
            case 'L':
                dlen = LN_L;
                break;
            default:
                --f;
            }

            c = *f;
            switch (c) {
            default:
                /* skip whitespace. include in %n, but not elsewhere */
                while (!GOT_EOF() && isspace(next))
                    NEXT_CHAR(read_chars);
            /* do not skip whitespace for... */
            case '[':
            case 'c':
                tryconv = 1;
                if (GOT_EOF()) INPUT_FAILURE();
            /* do not even check EOF for... */
            case 'n':
                break;
            }

            /* format */
            switch (c) {
            case '%': 
                /* literal % */
                if (next != '%') MATCH_FAILURE();
                NEXT_CHAR(nowread);
                break;
            { /* =========== READ INT =========== */
                /* variables for reading ints */
                /* decimal, hexadecimal, binary */
                int base;
                /* unsigned? %p? */
                BOOL unsign, isptr;
                /* is negative? */
                BOOL negative;
                /* result */
                intmax_t r;
                
            case 'p': /* pointer */
                    isptr = 1;
                    if (next == '(') { /* handle (nil) */
                        int k;
                        const char *rest = "nil)";
                        if (!maxlen) maxlen = (size_t)-1;
                        NEXT_CHAR(nowread);
                        for (k = 0; k < 4; ++k) {
                            if (!KEEP_READING() || next != rest[k])
                                MATCH_FAILURE();
                            NEXT_CHAR(nowread);
                        }
                        MATCH_SUCCESS();
                        if (!nostore) {
                            ++fields;
                            r = (intmax_t)NULL;
                            goto storenum;
                        }
                        break;
                    }
                    base = 16, unsign = 0, negative = 0;
                    goto readptr;
            case 'n': /* number of characters read */
                    if (nostore)
                        break;
                    r = (intmax_t)read_chars;
                    unsign = 0, isptr = 0;
                    goto storenum;
            case 'o': /* unsigned octal integer */
                    base = 8, unsign = 1;
                    goto readnum;
            case 'x': /* unsigned hexadecimal integer */
            case 'X':
                    base = 16, unsign = 1;
                    goto readnum;
#if SCANF_BINARY
            case 'b': /* non-standard: unsigned binary integer */
                    base = 2, unsign = 1;
                    goto readnum;
#endif
            case 'd': /* signed decimal integer */
            case 'u': /* unsigned decimal integer */ 
            case 'i': /* signed decimal/hex/binary integer */
                    base = 10, unsign = c == 'u';
                    /* fall-through */
            readnum:
                    isptr = 0;
                    
                    /* sign, read even for %u */
                    switch (next) {
                    case '-':
                        negative = 1;
                    case '+':
                        NEXT_CHAR(nowread);
                    default:
                        negative = 0;
                    }
                    /* fall-through */
            readptr:
                {
                    /* allow empty = zero? */
                    BOOL zero = 0;
                    unsigned char aton[SCANF_ATON_BUFFER_SIZE], *di = aton;
                    int k;
                    if (!maxlen) maxlen = (size_t)-1;

                    /* detect base from string for %i, skip 0x for %x,
                                                     demand 0x for %p */
                    if (c == 'i' || c == 'x' || c == 'X' || c == 'p') {
                        BOOL notfoundhex = isptr;
                        if (KEEP_READING() && next == '0') {
                            zero = 1;
                            NEXT_CHAR(nowread);
                            if (KEEP_READING() && (next == 'x' ||
                                                   next == 'X')) {
                                base = 16;
                                notfoundhex = 0;
                                NEXT_CHAR(nowread);
                            } else if (c == 'i') {
                                base = 8;
                            }
                        }
                        if (notfoundhex)
                            MATCH_FAILURE();
                    }
                    /* skip initial zeros */
                    while (KEEP_READING() && next == '0') {
                        NEXT_CHAR(nowread);
                        zero = 1;
                    }
                    /* read numbers into buffer */
                    for (k = 0; KEEP_READING() && isdigr_(next, base);) {
                        if (k < SCANF_ATON_BUFFER_SIZE - 1)
                            *di++ = next, ++k;
                        NEXT_CHAR(nowread);
                    }
                    *di = 0;

                    /* if buffer empty, could not read */
                    if (!*aton && !zero)
                        MATCH_FAILURE();
                    /* too many digits, overflow! */
                    else if (k >= SCANF_ATON_BUFFER_SIZE - 1)
                        r = negative ? INTMAX_MIN : INTMAX_MAX;
                    else
                        r = atobn_(aton, negative, base,
                                   INTMAX_MIN, INTMAX_MAX);
                    MATCH_SUCCESS();

                    if (nostore)
                        break;
                    ++fields;
                }
            storenum:
                {
                    /* store number, either as ptr, unsigned or signed */
                    void *dst = va_arg(va, void *);
                    if (isptr)
                        STORE_VALUE(dst, r, void *);
                    else if (unsign) {
                        switch (dlen) {
                        case LN_hh:
                            STORE_VALUE(dst, r, unsigned char);
                            break;
                        case LN_h:
                            STORE_VALUE(dst, r, unsigned short);
                            break;
                        case LN_l:
                            STORE_VALUE(dst, r, unsigned long);
                            break;
#ifndef INTMAXT_ALIAS
                        case LN_j:
                            STORE_VALUE(dst, r, uintmax_t);
                            break;
#endif
#ifndef SIZET_ALIAS
                        case LN_z:
                            STORE_VALUE(dst, r, size_t);
                            break;
#endif
#ifndef PTRDIFFT_ALIAS
                        case LN_t:
                            STORE_VALUE(dst, r, ptrdiff_t);
                            break;
#endif
#if !SCANF_DISABLE_SUPPORT_LONG_LONG
                        case LN_ll:
                            STORE_VALUE(dst, r, unsigned long long);
                            break;
#endif
                        default:
                            STORE_VALUE(dst, r, unsigned);
                        }
                    } else {
                        switch (dlen) {
                        case LN_hh:
                            STORE_VALUE(dst, r, signed char);
                            break;
                        case LN_h:
                            STORE_VALUE(dst, r, short);
                            break;
                        case LN_l:
                            STORE_VALUE(dst, r, long);
                            break;
#ifndef INTMAXT_ALIAS
                        case LN_j:
                            STORE_VALUE(dst, r, intmax_t);
                            break;
#endif
#ifndef SIZET_ALIAS
                        case LN_z:
                            STORE_VALUE(dst, r, size_t);
                            break;
#endif
#ifndef PTRDIFFT_ALIAS
                        case LN_t:
                            STORE_VALUE(dst, r, ptrdiff_t);
                            break;
#endif
#if !SCANF_DISABLE_SUPPORT_LONG_LONG
                        case LN_ll:
                            STORE_VALUE(dst, r, long long);
                            break;
#endif
                        default:
                            STORE_VALUE(dst, r, int);
                        }
                    }
                }
                break;
            } /* =========== READ INT =========== */

            case 'e': case 'E': /* scientific format float */
            case 'f': case 'F': /* decimal format float */
            case 'g': case 'G': /* decimal/scientific format float */
            case 'a': case 'A': /* hex format float */
                /* all treated equal by scanf, but not by printf */
#if SCANF_DISABLE_SUPPORT_FLOAT
                /* no support here */
                MATCH_FAILURE();
#else
            { /* =========== READ FLOAT =========== */
                maxfloat_t r;
                unsigned char atof[SCANF_ATOF_BUFFER_SIZE], *di = atof;
                intmax_t exp = 0;
                int base = 10, k;
                unsigned char explc = 'e', expuc = 'E';
                size_t leftover = 0;
                /* negative? found zero? allow dot? */
                BOOL negative = 0, zero = 0, dot = 0;
                if (!maxlen) maxlen = (size_t)-1;
                
                switch (next) {
                case '-':
                    negative = 1;
                case '+':
                    NEXT_CHAR(nowread);
                }

#if SCANF_INFINITE
                if (KEEP_READING() && (next == 'n' || next == 'N')) {
                    NEXT_CHAR(nowread);
                    if (!KEEP_READING() || !(next == 'a' || next == 'A'))
                        MATCH_FAILURE();
                    NEXT_CHAR(nowread);
                    if (!KEEP_READING() || !(next == 'n' || next == 'N'))
                        MATCH_FAILURE();
                    NEXT_CHAR(nowread);
                    if (KEEP_READING() && next == '(') {
                        while (KEEP_READING()) {
                            NEXT_CHAR(nowread);
                            if (next == ')') {
                                NEXT_CHAR(nowread);
                                break;
                            } else if (next != '_' && !isalnum(next))
                                MATCH_FAILURE();
                        }
                    }
                    r = (negative ? -1 : 1) * NAN;
                    goto got_f_result;
                } else if (KEEP_READING() && (next == 'i' || next == 'I')) {
                    NEXT_CHAR(nowread);
                    if (!KEEP_READING() || !(next == 'n' || next == 'N'))
                        MATCH_FAILURE();
                    NEXT_CHAR(nowread);
                    if (!KEEP_READING() || !(next == 'f' || next == 'F'))
                        MATCH_FAILURE();
                    NEXT_CHAR(nowread);
                    /* try reading the rest */
                    if (KEEP_READING()) {
                        const char *rest2 = "INITY";
                        for (k = 0; k < 5; ++k) {
                            if (!KEEP_READING() ||
                                    (next != rest2[k] && 
                                     next != tolower(rest2[k])))
                                break;
                            NEXT_CHAR(nowread);
                        }
                    }
                    r = (negative ? -1 : 1) * INFINITY;
                    goto got_f_result;
                }
#endif

                /* 0x for hex floats */
                if (KEEP_READING() && next == '0') {
                    NEXT_CHAR(nowread);
                    if (KEEP_READING() && (next == 'x' || next == 'X')) {
                        base = 16;
                        explc = 'p', expuc = 'P';
                        NEXT_CHAR(nowread);
                    }
                }

                while (KEEP_READING() && next == '0') {
                    NEXT_CHAR(nowread);
                    zero = 1;
                }
                /* copy strings of digits + decimal point to buffer */
                for (k = 0; KEEP_READING() && (isdigr_(next, base)
                                    || (next == '.' && !dot));) {
                    if (k < SCANF_ATOF_BUFFER_SIZE - 1)
                        *di++ = next, ++k;
                    else if (!dot)
                        ++leftover;
                    if (next == '.')
                        dot = 1;
                    NEXT_CHAR(nowread);
                }
                *di = 0;
                /* if buffer empty, could not read */
                if (!*atof && !zero)
                    MATCH_FAILURE();
                /* float cannot just be a decimal point! */
                if (dot && k == 1)
                    MATCH_FAILURE();

                /* exponent? */
                if (KEEP_READING() && (next == explc || next == expuc)) {
                    BOOL eneg = 0, ezero = 0;
                    unsigned char aton[SCANF_ATOD_BUFFER_SIZE], *edi = aton;
                    NEXT_CHAR(nowread);
                    if (KEEP_READING()) {
                        switch (next) {
                        case '-':
                            eneg = 1;
                        case '+':
                            NEXT_CHAR(nowread);
                        }
                    }

                    /* skip initial zeros */
                    while (KEEP_READING() && next == '0') {
                        NEXT_CHAR(nowread);
                        ezero = 1;
                    }
                    /* read into buffer */
                    for (k = 0; KEEP_READING() && isdigit(next);) {
                        if (k < SCANF_ATOF_BUFFER_SIZE - 1)
                            *edi++ = next, ++k;
                        NEXT_CHAR(nowread);
                    }
                    *edi = 0;
                    if (!*aton && !ezero)
                        MATCH_FAILURE();
                    /* overflow? */
                    else if (k >= SCANF_ATOD_BUFFER_SIZE - 1)
                        exp = eneg ? INTMAX_MIN : INTMAX_MAX;
                    else
                        exp = atobn_(aton, eneg, 10, INTMAX_MIN, INTMAX_MAX);
                }

                if (leftover) {
                    intmax_t oexp = exp;
                    exp += leftover * (base == 16 ? 4 : 1);
                    if (exp < oexp) exp = INTMAX_MAX; /* overflow protection */
                }

                /* conversion */
                r = atolf_(atof, negative, exp, base == 16);

#if SCANF_INFINITE
got_f_result:
#endif
                MATCH_SUCCESS();
                if (nostore)
                    break;
                ++fields;
                {
                    void *dst = va_arg(va, void *);
                    switch (dlen) {
                    case LN_l:
                        STORE_VALUE(dst, r, double);
                        break;
                    case LN_L:
                        STORE_VALUE(dst, r, long double);
                        break;
                    default:
                        STORE_VALUE(dst, r, float);
                    }
                }
            } /* =========== READ FLOAT =========== */
                break;
#endif
            case 'c': 
            { /* =========== READ CHAR =========== */
                char *outp = nostore ? (char *)NULL : va_arg(va, char *);
                if (!maxlen) maxlen = 1;
                while (KEEP_READING()) {
                    if (!nostore) *outp++ = next;
                    NEXT_CHAR(nowread);
                }
                if (nowread < maxlen)
                    MATCH_FAILURE();
                if (!nostore) ++fields;
                MATCH_SUCCESS();
            } /* =========== READ CHAR =========== */
                break;
            case 's': 
            { /* =========== READ STR =========== */
                char *outp = nostore ? (char *)NULL : va_arg(va, char *);
                if (!maxlen)
#if SCANF_SECURE
                    MATCH_FAILURE();
#else
                    maxlen = (size_t)-1;
#endif

                while (KEEP_READING() && !isspace(next)) {
                    if (!nostore) *outp++ = next;
                    NEXT_CHAR(nowread);
                }
                if (!nowread)
                    MATCH_FAILURE();
                if (!nostore) {
                    *outp++ = 0;
                    ++fields;
                }
                MATCH_SUCCESS();
            } /* =========== READ STR =========== */
                break;
            case '[':
            { /* =========== READ SCANSET =========== */
                char *outp = nostore ? (char *)NULL : va_arg(va, char *);
                BOOL hyphen = 0, invert = 0;
                unsigned char prev = 0, c;
#if SCANF_FAST_SCANSET
                BOOL mention[UCHAR_MAX + 1] = { 0 };
#else
                const unsigned char *set, *settmp;
#endif
                if (!maxlen)
#if SCANF_SECURE
                    MATCH_FAILURE();
#else
                    maxlen = (size_t)-1;
#endif
                ++f;
                if (*f == '^')
                    invert = 1, ++f;
                if (*f == ']')
                    ++f;
#if SCANF_FAST_SCANSET
                while ((c = *f) && c != ']') {
                    if (hyphen) {
                        int k;
                        for (k = prev; k <= c; ++k)
                            mention[k] = 1;
                        hyphen = 0;
                        prev = c;
                    } else if (c == '-' && prev)
                        hyphen = 1;
                    else
                        mention[c] = 1, prev = c;
                    ++f;
                }
                if (hyphen)
                    mention['-'] = 1;
#else
                set = f;
                while (*f && *f != ']')
                    ++f;
#endif
                while (KEEP_READING()) {
#if SCANF_FAST_SCANSET
                    if (mention[next] == invert) break;
#else
                    char found = 0;
                    settmp = set;
                    prev = 0;
                    hyphen = 0;
                    while (settmp < f) {
                        c = *settmp++;
                        if (hyphen) {
                            if (prev <= next && next <= c) {
                                found = 1;
                                break;
                            }
                            hyphen = 0;
                            prev = c;
                        } else if (c == '-' && prev) {
                            hyphen = 1;
                        } else if (c == next) {
                            found = 1;
                            break;
                        } else
                            prev = c;
                    }
                    if (hyphen && next == '-') found = 1;
                    if (found == invert) break;
#endif
                    if (!nostore) *outp++ = next;
                    NEXT_CHAR(nowread);
                }
                if (!nostore) {
                    *outp++ = 0;
                    ++fields;
                }
                MATCH_SUCCESS();
            } /* =========== READ SCANSET =========== */
                break;
            default:
                /* unrecognized specification */
                MATCH_FAILURE();
            }

            ++f; /* next fmt char */
            read_chars += nowread;
        }
    }
read_failure:
    /* if we have a leftover character, put it back into the stream */
    if (!GOT_EOF() && ungetch)
        ungetch(next, p);
    return (tryconv && match) ? EOF : fields;
}

static int getchw_(void* arg) {
    return getch_();
}

static void ungetchw_(int c, void* arg) {
    ungetch_(c);
}

static int sscanw_(void* arg) {
    const unsigned char **p = (const unsigned char **)arg;
    const unsigned char c = *(*p)++;
    return c ? c : EOF;
}

int vscanf_(const char* format, va_list arg) {
    return iscanf_(&getchw_, &ungetchw_, NULL, format, arg);
}

int scanf_(const char* format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vscanf_(format, va);
    va_end(va);
    return r;
}

int vspscanf_(const char **sp, const char* format, va_list arg) {
    int i = iscanf_(&sscanw_, NULL, sp, format, arg);
    --*sp; /* back up by one character, even if it was EOF we want the pointer
              at the null terminator */
    return i;
}

int spscanf_(const char **sp, const char* format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vspscanf_(sp, format, va);
    va_end(va);
    return r;
}

int vsscanf_(const char *s, const char* format, va_list arg) {
    return iscanf_(&sscanw_, NULL, &s, format, arg);
}

int sscanf_(const char *s, const char* format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vsscanf_(s, format, va);
    va_end(va);
    return r;
}

int vfctscanf_(int (*getch)(void* data), void (*ungetch)(int c, void* data),
                void* data, const char* format, va_list arg) {
    return iscanf_(getch, ungetch, data, format, arg);
}

int fctscanf_(int (*getch)(void* data), void (*ungetch)(int c, void* data),
                void* data, const char* format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vfctscanf_(getch, ungetch, data, format, va);
    va_end(va);
    return r;
}
