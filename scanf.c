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

/* =============================== *
 *        defines &  checks        *
 * =============================== */

/* C99/C++11 */
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
    || (defined(__cplusplus) && __cplusplus >= 201103L)
#define SCANF_C99 1
#else
#define SCANF_C99 0
#endif

/* stdint.h? */
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
/* (u)intmax_t, (U)INTMAX_(MIN_MAX) */
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

/* long long? */
#ifndef LLONG_MAX
#undef SCANF_DISABLE_SUPPORT_LONG_LONG
#define SCANF_DISABLE_SUPPORT_LONG_LONG 1
#endif

/* define uintptr_t if not already defined */
#if !defined(UINTPTR_MAX)
#if sizeof(void *) <= sizeof(unsigned char)
#define uintptr_t unsigned char
#elif sizeof(void *) <= sizeof(unsigned short)
#define uintptr_t unsigned short
#elif sizeof(void *) <= sizeof(unsigned int)
#define uintptr_t unsigned int
#elif sizeof(void *) <= sizeof(unsigned long)
#define uintptr_t unsigned long
#endif

#if !defined(uintptr_t) && defined(ULLONG_MAX)
#if sizeof(void *) <= sizeof(unsigned long long)
#define uintptr_t unsigned long long
#endif
#endif

#ifndef uintptr_t
#define uintptr_t uintmax_t
#endif
#endif

/* maximum precision floating point type */
#define floatmax_t long double

#if SCANF_C99
#define INLINE static inline
#else
#define INLINE static
#endif

/* boolean type */
#ifndef BOOL
#if defined(__cplusplus)
#define BOOL bool
#elif SCANF_C99
#define BOOL _Bool
#else
#define BOOL int
#endif
#endif

/* freestanding? */
#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 0
#define SCANF_FREESTANDING 1
#else
#define SCANF_FREESTANDING 0
#endif

#ifndef SCANF_NOMATH
#define SCANF_NOMATH SCANF_FREESTANDING
#endif

#ifndef SCANF_INTERNAL_CTYPE
#define SCANF_INTERNAL_CTYPE SCANF_FREESTANDING
#endif

#ifndef SCANF_BINARY
#define SCANF_BINARY 1
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

#ifndef SCANF_LOGN_POW
#define SCANF_LOGN_POW 1
#endif

/* include more stuff */

#if !SCANF_INTERNAL_CTYPE
#include <ctype.h>
#endif

#if !SCANF_DISABLE_SUPPORT_FLOAT
#include <float.h>
#endif

#if !SCANF_NOMATH
#include <math.h>
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

/* try to map size_t to unsigned long long, unsigned long, or int */
#if defined(SIZE_MAX)
#if defined(ULLONG_MAX) && SIZE_MAX == ULLONG_MAX
#define SIZET_ALIAS ll
#elif defined(ULONG_MAX) && SIZE_MAX == ULONG_MAX
#define SIZET_ALIAS l
#elif defined(UINT_MAX) && SIZE_MAX == UINT_MAX
#define SIZET_ALIAS
/* intentionally empty -> maps to int */
#endif
#endif

/* try to map ptrdiff_t to long long, long, or int */
#if defined(PTRDIFF_MAX)
#if defined(LLONG_MAX) && PTRDIFF_MAX == LLONG_MAX && PTRDIFF_MIN == LLONG_MIN
#define PTRDIFFT_ALIAS ll
#elif defined(LONG_MAX) && PTRDIFF_MAX == LONG_MAX && PTRDIFF_MIN == LONG_MIN
#define PTRDIFFT_ALIAS l
#elif defined(INT_MAX) && PTRDIFF_MAX == INT_MAX && PTRDIFF_MIN == INT_MIN
#define PTRDIFFT_ALIAS
/* intentionally empty -> maps to int */
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

/* check if short == int */
#if defined(INT_MIN) && defined(SHRT_MIN) && INT_MIN == SHRT_MIN               \
 && defined(INT_MAX) && defined(SHRT_MAX) && INT_MAX == SHRT_MAX               \
 && defined(UINT_MAX) && defined(USHRT_MAX) && UINT_MAX == USHRT_MAX
#define SHORT_IS_INT 1
#endif

/* check if long == int */
#if defined(INT_MIN) && defined(LONG_MIN) && INT_MIN == LONG_MIN               \
 && defined(INT_MAX) && defined(LONG_MAX) && INT_MAX == LONG_MAX               \
 && defined(UINT_MAX) && defined(ULONG_MAX) && UINT_MAX == ULONG_MAX
#define LONG_IS_INT 1
#endif

/* check if long long == long */
#if defined(LONG_MIN) && defined(LLONG_MIN) && LONG_MIN == LLONG_MIN           \
 && defined(LONG_MAX) && defined(LLONG_MAX) && LONG_MAX == LLONG_MAX           \
 && defined(ULONG_MAX) && defined(ULLONG_MAX) && ULONG_MAX == ULLONG_MAX
#define LLONG_IS_LONG 1
#endif

#if !SCANF_DISABLE_SUPPORT_FLOAT
/* check if double == float */
#if defined(FLT_MANT_DIG) && defined(DBL_MANT_DIG)                             \
                  && FLT_MANT_DIG == DBL_MANT_DIG                              \
 && defined(FLT_MIN_EXP) && defined(DBL_MIN_EXP)                               \
                  && FLT_MIN_EXP == DBL_MIN_EXP                                \
 && defined(FLT_MAX_EXP) && defined(DBL_MAX_EXP)                               \
                  && FLT_MAX_EXP == DBL_MAX_EXP
#define DOUBLE_IS_FLOAT 1
#endif

/* check if long double == double */
#if defined(DBL_MANT_DIG) && defined(LDBL_MANT_DIG)                            \
                  && DBL_MANT_DIG == LDBL_MANT_DIG                             \
 && defined(DBL_MIN_EXP) && defined(LDBL_MIN_EXP)                              \
                  && DBL_MIN_EXP == LDBL_MIN_EXP                               \
 && defined(DBL_MAX_EXP) && defined(LDBL_MAX_EXP)                              \
                  && DBL_MAX_EXP == LDBL_MAX_EXP
#define LDOUBLE_IS_DOUBLE 1
#endif
#endif

/* =============================== *
 *         digit conversion        *
 * =============================== */

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

/* =============================== *
 *         character checks        *
 * =============================== */

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
#if SCANF_ASCII
    return '0' <= c && c <= '9';
#else
    return ctodn_(c) >= 0;
#endif
}

#if SCANF_INFINITE
#if SCANF_ASCII
INLINE int isalpha(int c) {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

INLINE int isalnum(int c) {
    return isdigit(c) || isalpha(c);
}

INLINE int tolower(int c) {
    return isalpha(c) ? c | 0x20 : c;
}
#else
#error SCANF_INTERNAL_CTYPE currently not fully implemented for SCANF_ASCII=0
#endif /* SCANF_ASCII */
#endif /* SCANF_INFINITE */

#endif /* SCANF_INTERNAL_CTYPE */

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

/* =============================== *
 *       floating point math       *
 * =============================== */

#if !SCANF_DISABLE_SUPPORT_FLOAT

#if !SCANF_NOPOW
INLINE floatmax_t powi_(floatmax_t x, intmax_t y) {
    return pow(x, y);
}
#elif SCANF_LOGN_POW
INLINE floatmax_t powi_(floatmax_t x, intmax_t y) {
    floatmax_t r = (floatmax_t)1;
    for (; y > 0; y >>= 1) {
        if (y & 1) r *= x;
        x *= x;
    }
    return r;
}
#else
INLINE floatmax_t powi_(floatmax_t x, intmax_t y) {
    floatmax_t r = (floatmax_t)1;
    for (; y > 0; --y)
        r *= x;
    return r;
}
#endif

#endif /* !SCANF_DISABLE_SUPPORT_FLOAT */

/* =============================== *
 *       main scanf function       *
 * =============================== */

/* enum for possible data types */
enum dlength { LN_, LN_hh, LN_h, LN_l, LN_ll, LN_L, LN_j, LN_z, LN_t };
#define vLNa_(x) LN_##x
#define vLN_(x) vLNa_(x)

/* EOF check, may be customized later for wide chars whatnot */
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
            /* where the value will be stored */
            void *dst;
            /* store value to dst with cast */
#define STORE_DST(value, T) (*(T *)(dst) = (T)(value))
            
            /* nostore */
            if (*f == '*') {
                nostore = 1;
                ++f;
                dst = NULL;
            } else
                dst = va_arg(va, void *);
                /* A pointer to any incomplete or object type may be converted
                   to a pointer to void and back again; the result shall compare
                   equal to the original pointer. */ 

            /* width specifier => maxlen */
            if (isdigit(*f)) {
                BOOL ovf = 0;
                size_t pr = 0;
                while (*f == '0')
                    ++f;
                while (isdigit(*f)) {
                    if (!ovf) {
                        maxlen *= 10;
                        if (maxlen < pr) {
                            maxlen = (size_t)UINTMAX_MAX;
                            ovf = 1;
                        } else {
                            pr = maxlen;
                            maxlen += ctodn_(*f);
                        }
                    }
                    ++f;
                }
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
                            r = (intmax_t)(uintmax_t)(uintptr_t)NULL;
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
                    negative = 0;
                    
                    /* sign, read even for %u */
                    switch (next) {
                    case '-':
                        negative = 1;
                    case '+':
                        NEXT_CHAR(nowread);
                    }
                    /* fall-through */
            readptr:
                {
                    intmax_t pr;
                    /* read digits? overflow? */
                    BOOL digit = 0, ovf = 0;
                    if (!maxlen) maxlen = (size_t)-1;
                    pr = r = 0;

                    /* detect base from string for %i, skip 0x for %x,
                                                     demand 0x for %p */
                    if (c == 'i' || c == 'x' || c == 'X' || c == 'p') {
                        BOOL notfoundhex = isptr;
                        if (KEEP_READING() && next == '0') {
                            digit = 1;
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
                        digit = 1;
                    }
                    /* read digits and convert to integer */
                    while (KEEP_READING() && isdigr_(next, base)) {
                        if (!ovf) {
                            r *= base;
                            if ((uintmax_t)r < (uintmax_t)pr) {
                                ovf = 1;
                            } else {
                                pr = r;
                                r += ctorn_(next, base);
                            }
                            digit = 1;
                        }
                        NEXT_CHAR(nowread);
                    }
                    
                    /* if no digits read? */
                    if (!digit)
                        MATCH_FAILURE();

                    /* overflow detection, negation, etc. */
                    if (unsign) {
                        if (ovf)
                            r = (intmax_t)UINTMAX_MAX;
                        else if (negative)
                            r = -r;
                    } else {
                        if (ovf || r < 0)
                            r = negative ? INTMAX_MIN : INTMAX_MAX;
                        else if (negative) {
                            r = -r;
                            if (r >= 0)
                                r = INTMAX_MIN;
                        }
                    }

                    MATCH_SUCCESS();

                    if (nostore)
                        break;
                    ++fields;
                }
            storenum:
                /* store number, either as ptr, unsigned or signed */
                if (isptr)
                    STORE_DST((uintptr_t)(uintmax_t)r, void *);
                else {
                    switch (dlen) {
                    case LN_hh:
                        if (unsign) STORE_DST(r, unsigned char);
                        else        STORE_DST(r, signed char);
                        break;
#if !SHORT_IS_INT
                    case LN_h: /* if SHORT_IS_INT, match fails => default: */
                        if (unsign) STORE_DST(r, unsigned short);
                        else        STORE_DST(r, short);
                        break;
#endif
#ifndef INTMAXT_ALIAS
                    case LN_j:
                        if (unsign) STORE_DST(r, uintmax_t);
                        else        STORE_DST(r, intmax_t);
                        break;
#endif
#ifndef SIZET_ALIAS
                    case LN_z:
                        STORE_DST(r, size_t);
                        break;
#endif
#ifndef PTRDIFFT_ALIAS
                    case LN_t:
                        STORE_DST(r, ptrdiff_t);
                        break;
#endif
#if !SCANF_DISABLE_SUPPORT_LONG_LONG
                    case LN_ll:
#if !LLONG_IS_LONG
                        if (unsign) STORE_DST(r, unsigned long long);
                        else        STORE_DST(r, long long);
                        break;
#endif
#endif /* SCANF_DISABLE_SUPPORT_LONG_LONG */
                    case LN_l:
#if !LONG_IS_INT
                        if (unsign) STORE_DST(r, unsigned long);
                        else        STORE_DST(r, long);
                        break;
#endif
                    default:
                        if (unsign) STORE_DST(r, unsigned);
                        else        STORE_DST(r, int);
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
                floatmax_t r = 0, pr = 0;
                intmax_t off = 0, exp = 0;
                int base = 10, sub = 0;
                unsigned char explc = 'e', expuc = 'E';
                /* negative? allow dot? read >0 digits? overflow? hex mode? */
                BOOL negative = 0, dot = 0, digit = 0, ovf = 0, hex = 0;
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
                        int k;
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
#endif /* SCANF_INFINITE */

                /* 0x for hex floats */
                if (KEEP_READING() && next == '0') {
                    NEXT_CHAR(nowread);
                    digit = 1;
                    if (KEEP_READING() && (next == 'x' || next == 'X')) {
                        base = 16;
                        hex = 1;
                        explc = 'p', expuc = 'P';
                        NEXT_CHAR(nowread);
                    }
                }

                while (KEEP_READING() && next == '0') {
                    NEXT_CHAR(nowread);
                    digit = 1;
                }
                
                /* read digits and convert */
                while (KEEP_READING() && (isdigr_(next, base) ||
                                         (next == '.' && !dot))) {
                    if (next == '.')
                        dot = 1, sub = hex ? 4 : 1;
                    else if (!ovf) {
                        r *= base;
                        if (r > 0 && r == pr) {
                            ovf = 1;
                        } else {
                            pr = r;
                            r += ctorn_(next, base);
                            off += sub;
                        }
                        digit = 1;
                    }
                    NEXT_CHAR(nowread);
                }
                /* read no digits? */
                if (!digit)
                    MATCH_FAILURE();

                /* exponent? */
                if (KEEP_READING() && (next == explc || next == expuc)) {
                    BOOL eneg = 0, edigit = 0, eovf = 0;
                    intmax_t pe = 0;
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
                        edigit = 1;
                    }
                    /* read exp from stream */
                    while (KEEP_READING() && isdigit(next)) {
                        if (!eovf) {
                            exp *= 10;
                            if ((uintmax_t)exp < (uintmax_t)pe) {
                                eovf = 1;
                            } else {
                                pe = exp;
                                exp += ctodn_(next);
                            }
                            edigit = 1;
                        }
                        NEXT_CHAR(nowread);
                    }
                    if (!edigit)
                        MATCH_FAILURE();
                    /* overflow detection, negation, etc. */
                    if (eovf || exp < 0)
                        exp = eneg ? INTMAX_MIN : INTMAX_MAX;
                    else if (eneg) {
                        exp = -exp;
                        if (exp >= 0)
                            exp = INTMAX_MIN;
                    }
                }

                if (dot) {
                    intmax_t oexp = exp;
                    exp -= off;
                    if (exp > oexp) exp = INTMAX_MIN; /* overflow protection */
                }

                if (r != 0) {
                    if (exp > 0) {
#ifdef INFINITY
                        if (exp > (hex ? LDBL_MAX_EXP : LDBL_MAX_10_EXP))
                            r = INFINITY;
                        else
#endif
                            r *= (hex ? powi_(2, exp) : powi_(10, exp));
                    } else if (exp < 0) {
                        if (exp < (hex ? LDBL_MIN_EXP : LDBL_MIN_10_EXP))
                            r = 0;
                        else
                            r /= (hex ? powi_(2, -exp) : powi_(10, -exp));
                    }
                }
                if (negative) r = -r;

#if SCANF_INFINITE
got_f_result:
#endif
                MATCH_SUCCESS();
                if (nostore)
                    break;
                ++fields;
                switch (dlen) {
                case LN_L:
#if !LDOUBLE_IS_DOUBLE
                    STORE_DST(r, long double);
                    break;
#endif
                case LN_l:
#if !DOUBLE_IS_FLOAT
                    STORE_DST(r, double);
                    break;
#endif
                default:
                    STORE_DST(r, float);
                }
            } /* =========== READ FLOAT =========== */
                break;
#endif /* SCANF_DISABLE_SUPPORT_FLOAT */
            case 'c': 
            { /* =========== READ CHAR =========== */
                char *outp = (char *)dst;
                if (!maxlen) maxlen = 1;
                while (KEEP_READING()) {
                    if (!nostore) *outp++ = (char)(unsigned char)next;
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
                char *outp = (char *)dst;
                if (!maxlen)
#if SCANF_SECURE
                    MATCH_FAILURE();
#else
                    maxlen = (size_t)-1;
#endif

                while (KEEP_READING() && !isspace(next)) {
                    if (!nostore) *outp++ = (char)(unsigned char)next;
                    NEXT_CHAR(nowread);
                }
                if (!nowread)
                    MATCH_FAILURE();
                if (!nostore) {
                    *outp = 0;
                    ++fields;
                }
                MATCH_SUCCESS();
            } /* =========== READ STR =========== */
                break;
            case '[':
            { /* =========== READ SCANSET =========== */
                char *outp = (char *)dst;
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
#else /* SCANF_FAST_SCANSET */
                set = f;
                while (*f && *f != ']')
                    ++f;
#endif /* SCANF_FAST_SCANSET */
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
#endif /* SCANF_FAST_SCANSET */
                    if (!nostore) *outp++ = (char)(unsigned char)next;
                    NEXT_CHAR(nowread);
                }
                if (!nostore) {
                    *outp = 0;
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

/* =============================== *
 *        wrapper functions        *
 * =============================== */

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
