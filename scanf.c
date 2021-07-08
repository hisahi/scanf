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

/* =============================== *
 *        defines &  checks        *
 * =============================== */

/* C99/C++11 */
#ifndef SCANF_C99
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
    || (defined(__cplusplus) && __cplusplus >= 201103L)
#define SCANF_C99 1
#else
#define SCANF_C99 0
#endif
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

/* maximum precision floating point type */
#ifndef floatmax_t
#define floatmax_t long double
#endif

#ifndef INLINE
#if SCANF_C99
#define INLINE static inline
#else
#define INLINE static
#endif
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
#ifndef SCANF_FREESTANDING
#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 0
#define SCANF_FREESTANDING 1
#else
#define SCANF_FREESTANDING 0
#endif
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

/* this is technically UB, as the character encodings used by the preprocessor
   and the actual compiled code might be different. in practice this is hardly 
   ever an issue, and if it is, you can just define SCANF_ASCII manually */
#ifndef SCANF_ASCII
#if 'B' - 'A' ==  1 && 'K' - 'A' == 10 && 'Z' - 'A' == 25                      \
 && 'a' - 'A' == 32 && 'n' - 'N' == 32 && 'v' - 'V' == 32 && 'z' - 'Z' == 32   \
 && '3' - '0' ==  3 && '9' - '0' ==  9
#define SCANF_ASCII 1
#else
#define SCANF_ASCII 0
#endif
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

/* wide setup */
#ifndef SCANF_WIDE
#define SCANF_WIDE 0
#endif

#if SCANF_WIDE < 0 || SCANF_WIDE > 3
#error invalid value for SCANF_WIDE (valid: 0, 1, 2, 3)
#endif

#if SCANF_WIDE == 3
#define SCANF_WIDE_CONVERT 1
#endif

#define SCANF_NODEFINE 1
#if SCANF_WIDE
/* also includes wide.h */
#include "wscanf.h"
#else
#include "scanf.h"
#endif

/* include more stuff */

#if !SCANF_INTERNAL_CTYPE
#if SCANF_WIDE
#include <wctype.h>
#define isdigitw_ iswdigit
#define isspacew_ iswspace
#define isalnumw_ iswalnum
#define isalphaw_ iswalpha
#define tolowerw_ towlower
#else
#include <ctype.h>
#endif
#endif

#ifdef SCANF_REPEAT
#undef C_
#undef S_
#undef F_
#undef CHAR
#undef UCHAR
#undef CINT
#endif

#if SCANF_WIDE
/* character */
#ifdef MAKE_WCHAR
#define C_(x) MAKE_WCHAR(x)
#else
#define C_(x) L##x
#endif
/* string */
#ifdef MAKE_WSTRING
#define S_(x) MAKE_WSTRING(x)
#else
#define S_(x) L##x
#endif
/* function */
#define F_(x) x##w_
#define CHAR WCHAR
#define UCHAR WCHAR
#define CINT WINT
#else
/* character */
#define C_(x) x
/* string */
#define S_(x) x
/* function */
#define F_(x) x
#define CHAR char
#define UCHAR unsigned char
#define CINT int
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

/* WEOF from wide.h */

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

/* this is intentionally after the previous check */
#ifndef SIZE_MAX
#define SIZE_MAX (size_t)(-1)
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

#ifdef UINTPTR_MAX
#define INT_TO_PTR(x) ((void*)(uintptr_t)(uintmax_t)(x))
#else
#define INT_TO_PTR(x) ((void*)(uintmax_t)(x))
#endif

/* =============================== *
 *        digit  conversion        *
 * =============================== */

INLINE_IF_ASCII int F_(ctodn_)(CINT c) {
#if SCANF_ASCII
    return c - C_('0');
#else
    switch (c) {
    case C_('0'): return 0;
    case C_('1'): return 1;
    case C_('2'): return 2;
    case C_('3'): return 3;
    case C_('4'): return 4;
    case C_('5'): return 5;
    case C_('6'): return 6;
    case C_('7'): return 7;
    case C_('8'): return 8;
    case C_('9'): return 9;
    default:      return -1;
    }
#endif
}

INLINE_IF_ASCII int F_(ctoon_)(CINT c) {
#if SCANF_ASCII
    return c - C_('0');
#else
    switch (c) {
    case C_('0'): return 0;
    case C_('1'): return 1;
    case C_('2'): return 2;
    case C_('3'): return 3;
    case C_('4'): return 4;
    case C_('5'): return 5;
    case C_('6'): return 6;
    case C_('7'): return 7;
    default:      return -1;
    }
#endif
}

static int F_(ctoxn_)(CINT c) {
#if SCANF_ASCII
    if (c >= C_('a'))
        return c - C_('a') + 10;
    else if (c >= C_('A'))
        return c - C_('A') + 10;
    return c - C_('0');
#else
    switch (c) {
    case C_('0'): return 0;
    case C_('1'): return 1;
    case C_('2'): return 2;
    case C_('3'): return 3;
    case C_('4'): return 4;
    case C_('5'): return 5;
    case C_('6'): return 6;
    case C_('7'): return 7;
    case C_('8'): return 8;
    case C_('9'): return 9;
    case C_('A'): case C_('a'): return 10;
    case C_('B'): case C_('b'): return 11;
    case C_('C'): case C_('c'): return 12;
    case C_('D'): case C_('d'): return 13;
    case C_('E'): case C_('e'): return 14;
    case C_('F'): case C_('f'): return 15;
    default:      return -1;
    }
#endif
}

#if SCANF_BINARY
INLINE_IF_ASCII int F_(ctobn_)(CINT c) {
#if SCANF_ASCII
    return c - C_('0');
#else
    switch (c) {
    case C_('0'): return 0;
    case C_('1'): return 1;
    default:      return -1;
    }
#endif
}
#endif

static int F_(ctorn_)(CINT c, int b) {
    switch (b) {
    case 8:
        return F_(ctoon_)(c);
    case 16:
        return F_(ctoxn_)(c);
#if SCANF_BINARY
    case 2:
        return F_(ctobn_)(c);
#endif
    default: /* 10 */
        return F_(ctodn_)(c);
    }
}

/* =============================== *
 *         character checks        *
 * =============================== */

#if SCANF_INTERNAL_CTYPE
static int F_(isspace)(CINT c) {
    switch (c) {
    case C_(' '):
    case C_('\t'):
    case C_('\n'):
    case C_('\v'):
    case C_('\f'):
    case C_('\r'):
        return 1;
    }
    return 0;
}

INLINE int F_(isdigit)(CINT c) {
#if SCANF_ASCII
    return C_('0') <= c && c <= C_('9');
#else
    return F_(ctodn_)(c) >= 0;
#endif
}

#if SCANF_INFINITE
#if SCANF_ASCII
INLINE int F_(isalpha)(CINT c) {
    return (C_('A') <= c && c <= C_('Z')) || (C_('a') <= c && c <= C_('z'));
}

INLINE int F_(isalnum)(CINT c) {
    return F_(isdigit)(c) || F_(isalpha)(c);
}

INLINE CINT F_(tolower)(CINT c) {
    return F_(isalpha)(c) ? c | 0x20 : c;
}
#else
#error SCANF_INTERNAL_CTYPE currently not fully implemented for SCANF_ASCII=0
#endif /* SCANF_ASCII */
#endif /* SCANF_INFINITE */

#endif /* SCANF_INTERNAL_CTYPE */

INLINE int F_(isdigo_)(CINT c) {
#if SCANF_ASCII
    return C_('0') <= c && c <= C_('7');
#else
    return F_(ctoon_)(c) >= 0;
#endif
}

INLINE int F_(isdigx_)(CINT c) {
#if SCANF_ASCII
    return F_(isdigit)(c) || (C_('A') <= c && c <= C_('F'))
                          || (C_('a') <= c && c <= C_('f'));
#else
    return F_(ctoxn_)(c) >= 0;
#endif
}

#if SCANF_BINARY
INLINE int F_(isdigb_)(CINT c) {
    return c == C_('0') || c == C_('1');
}
#endif

INLINE int F_(isdigr_)(CINT c, int b) {
    switch (b) {
    case 8:
        return F_(isdigo_)(c);
    case 16:
        return F_(isdigx_)(c);
#if SCANF_BINARY
    case 2:
        return F_(isdigb_)(c);
#endif
    default: /* 10 */
        return F_(isdigit)(c);
    }
}

/* =============================== *
 *       floating point math       *
 * =============================== */

#ifndef SCANF_REPEAT

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

#endif

/* =============================== *
 *        extension support        *
 * =============================== */

#if SCANF_EXTENSIONS
#if SCANF_WIDE
struct wscanf_ext_tmp {
    WINT (*getch)(void *data);
    void *data;
    size_t len;
};

int scnwext_(WINT (*getwch)(void *data), void *data, const WCHAR **format,
             WINT *buffer, int length, int nostore, void *destination);

WINT wscanf_ext_getch_(void *data) {
    struct wscanf_ext_tmp *st = (struct wscanf_ext_tmp *)data;
    if (!st->len)
        return WEOF;
    else {
        --st->len;
        return st->getch(st->data);
    }
}
#else /* SCANF_WIDE */
struct scanf_ext_tmp {
    int (*getch)(void *data);
    void *data;
    size_t len;
};

int scnext_(int (*getch)(void *data), void *data, const char **format,
            int *buffer, int length, int nostore, void *destination);

int scanf_ext_getch_(void *data) {
    struct scanf_ext_tmp *st = (struct scanf_ext_tmp *)data;
    if (!st->len)
        return -1;
    else {
        --st->len;
        return st->getch(st->data);
    }
}
#endif /* SCANF_WIDE */
#endif /* SCANF_EXTENSIONS */

/* =============================== *
 *        scanset functions        *
 * =============================== */

#if !SCANF_FAST_SCANSET || SCANF_WIDE

BOOL F_(inscan_)(const UCHAR *begin, const UCHAR *end, UCHAR c) {
    BOOL found = 0, hyphen = 0;
    UCHAR prev = 0, f;
    const UCHAR *p = begin;
    while (p < end) {
        f = *p++;
        if (hyphen) {
            if (prev <= c && c <= f) {
                found = 1;
                break;
            }
            hyphen = 0;
            prev = f;
        } else if (f == C_('-') && prev) {
            hyphen = 1;
        } else if (f == c) {
            found = 1;
            break;
        } else
            prev = f;
    }
    if (hyphen && c == C_('-')) found = 1;
    return found;
}

#endif

/* =============================== *
 *       main scanf function       *
 * =============================== */

#ifdef SCANF_REPEAT
#undef IS_EOF
#endif

/* EOF check */
#if SCANF_WIDE
#define IS_EOF(c) ((c) == WEOF)
#else
#define IS_EOF(c) ((c) < 0)
#endif

#ifndef SCANF_REPEAT
/* enum for possible data types */
enum dlength { LN_, LN_hh, LN_h, LN_l, LN_ll, LN_L, LN_j, LN_z, LN_t };

#define vLNa_(x) LN_##x
#define vLN_(x) vLNa_(x)
#endif

#undef GOT_EOF
#define GOT_EOF() (IS_EOF(next))

static int F_(iscanf_)(CINT (*getch)(void *p), void (*ungetch)(CINT c, void *p),
                       void *p, const CHAR *ff, va_list va) {
    /* fields = number of fields successfully read; this is the return value */
    int fields = 0;
    /* next = the "next" character to be processed */
    CINT next;
    /* total characters read, returned by %n */
    size_t read_chars = 0;
    /* whether we even tried to convert anything */
    BOOL tryconv = 0;
    /* whether we have failed to convert anything successfully.
       match = 0 means matching failure, match = 1 means EOF / input failure
                                                (before any conversion) */
    BOOL match = 1;
    const UCHAR *f = (const UCHAR *)ff;
    UCHAR c;
#if SCANF_WIDE_CONVERT
    scanf_mbstate_t mbstate;
#endif

    /* empty format string always returns 0 */
    if (!*f) return 0;

    /* read and cache first character */
    next = getch(p);
    /* ++read_chars; intentionally left out, otherwise %n is off by 1 */
    while ((c = *f++)) {
#ifndef SCANF_REPEAT
        /* still characters to read? (not EOF and width not exceeded) */
#define KEEP_READING() (nowread < maxlen && !GOT_EOF())
        /* read next char and increment counter */
#define NEXT_CHAR(counter) (next = getch(p), ++counter)
        /* signal match OK */
#define MATCH_SUCCESS() (match = 0)
        /* signal match failure and exit loop */
#define MATCH_FAILURE() do { if (!GOT_EOF()) match = 0; \
                             goto read_failure; } while (0)
        /* signal input f ailure and exit loop */
#define INPUT_FAILURE() do { goto read_failure; } while (0)
#endif

        if (F_(isspace)(c)) {
            /* skip 0-N whitespace */
            while (!GOT_EOF() && F_(isspace)(next))
                NEXT_CHAR(read_chars);
        } else if (c != C_('%')) {
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
            if (*f == C_('*')) {
                nostore = 1;
                ++f;
                dst = NULL;
            } else
                dst = va_arg(va, void *);
                /* A pointer to any incomplete or object type may be converted
                   to a pointer to void and back again; the result shall compare
                   equal to the original pointer. */ 

            /* width specifier => maxlen */
            if (F_(isdigit)(*f)) {
                BOOL ovf = 0;
                size_t pr = 0;
                while (*f == C_('0'))
                    ++f;
                while (F_(isdigit)(*f)) {
                    if (!ovf) {
                        maxlen *= 10;
                        if (maxlen < pr) {
                            maxlen = SIZE_MAX;
                            ovf = 1;
                        } else {
                            pr = maxlen;
                            maxlen += F_(ctodn_)(*f);
                        }
                    }
                    ++f;
                }
            }

#if SCANF_EXTENSIONS
            if (*f == C_('!')) {
                const CHAR *sf = (const CHAR *)(f + 1);
                BOOL hadlen = maxlen != 0;
                struct scanf_ext_tmp tmp;
                int ok;

                if (!hadlen)
                    maxlen = SIZE_MAX;
                tmp.getch = getch;
                tmp.data = p;
                tmp.len = maxlen;
#if SCANF_WIDE
                ok = scnwext_(&wscanf_ext_getch_, &tmp, &sf, &next,
                              hadlen, nostore, dst);
#else
                ok = scnext_(&scanf_ext_getch_, &tmp, &sf, &next,
                             hadlen, nostore, dst);
#endif
                f = (const UCHAR *)sf;
                if (ok < 0)
                    INPUT_FAILURE();
                else if (ok > 0)
                    MATCH_FAILURE();
                else if (!nostore)
                    ++fields;
                read_chars += maxlen - tmp.len;
                continue;
            }
#endif

            /* length specifier */
            switch (*f++) {
            case C_('h'):
                if (*f == C_('h'))
                    dlen = LN_hh, ++f;
                else
                    dlen = LN_h;
                break;
            case C_('l'):
#if !SCANF_DISABLE_SUPPORT_LONG_LONG
                if (*f == C_('l'))
                    dlen = LN_ll, ++f;
                else
#endif
                    dlen = LN_l;
                break;
            case C_('j'):
#ifdef INTMAXT_ALIAS
                dlen = vLN_(INTMAXT_ALIAS);
#else
                dlen = LN_j;
#endif
                break;
            case C_('t'):
#ifdef PTRDIFFT_ALIAS
                dlen = vLN_(PTRDIFFT_ALIAS);
#else
                dlen = LN_t;
#endif
                break;
            case C_('z'):
#ifdef SIZET_ALIAS
                dlen = vLN_(SIZET_ALIAS);
#else
                dlen = LN_z;
#endif
                break;
            case C_('L'):
                dlen = LN_L;
                break;
            default:
                --f;
            }

            c = *f;
            switch (c) {
            default:
                /* skip whitespace. include in %n, but not elsewhere */
                while (!GOT_EOF() && F_(isspace)(next))
                    NEXT_CHAR(read_chars);
            /* do not skip whitespace for... */
            case C_('['):
            case C_('c'):
                tryconv = 1;
                if (GOT_EOF()) INPUT_FAILURE();
            /* do not even check EOF for... */
            case C_('n'):
                break;
            }

            /* format */
            switch (c) {
            case C_('%'): 
                /* literal % */
                if (next != C_('%')) MATCH_FAILURE();
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
                /* result: i for integers, p for pointers */
                union {
                    intmax_t i;
                    void *p;
                } r;
                
            case C_('p'): /* pointer */
                    isptr = 1;
                    if (next == C_('(')) { /* handle (nil) */
                        int k;
                        const CHAR *rest = S_("nil)");
                        if (!maxlen) maxlen = SIZE_MAX;
                        NEXT_CHAR(nowread);
                        for (k = 0; k < 4; ++k) {
                            if (!KEEP_READING() || next != rest[k])
                                MATCH_FAILURE();
                            NEXT_CHAR(nowread);
                        }
                        MATCH_SUCCESS();
                        if (!nostore) {
                            ++fields;
                            r.p = NULL;
                            goto storeptr;
                        }
                        break;
                    }
                    base = 16, unsign = 0, negative = 0;
                    goto readptr;
            case C_('n'): /* number of characters read */
                    if (nostore)
                        break;
                    r.i = (intmax_t)read_chars;
                    unsign = 0, isptr = 0;
                    goto storenum;
            case C_('o'): /* unsigned octal integer */
                    base = 8, unsign = 1;
                    goto readnum;
            case C_('x'): /* unsigned hexadecimal integer */
            case C_('X'):
                    base = 16, unsign = 1;
                    goto readnum;
#if SCANF_BINARY
            case C_('b'): /* non-standard: unsigned binary integer */
                    base = 2, unsign = 1;
                    goto readnum;
#endif
            case C_('d'): /* signed decimal integer */
            case C_('u'): /* unsigned decimal integer */ 
            case C_('i'): /* signed decimal/hex/binary integer */
                    base = 10, unsign = c == C_('u');
                    /* fall-through */
            readnum:
                    isptr = 0;
                    negative = 0;
                    
                    /* sign, read even for %u */
                    switch (next) {
                    case C_('-'):
                        negative = 1;
                    case C_('+'):
                        NEXT_CHAR(nowread);
                    }
                    /* fall-through */
            readptr:
                {
                    intmax_t pr;
                    /* read digits? overflow? */
                    BOOL digit = 0, ovf = 0;
                    if (!maxlen) maxlen = SIZE_MAX;
                    pr = r.i = 0;

                    /* detect base from string for %i, skip 0x for %x,
                                                     demand 0x for %p */
                    if (c == C_('i') || c == C_('x') || c == C_('X')
                                                     || c == C_('p')) {
                        BOOL notfoundhex = isptr;
                        if (KEEP_READING() && next == C_('0')) {
                            digit = 1;
                            NEXT_CHAR(nowread);
                            if (KEEP_READING() && (next == C_('x') ||
                                                   next == C_('X'))) {
                                base = 16;
                                notfoundhex = 0;
                                NEXT_CHAR(nowread);
                            } else if (c == C_('i')) {
                                base = 8;
                            }
                        }
                        if (notfoundhex)
                            MATCH_FAILURE();
                    }
                    
                    /* skip initial zeros */
                    while (KEEP_READING() && next == C_('0')) {
                        NEXT_CHAR(nowread);
                        digit = 1;
                    }
                    /* read digits and convert to integer */
                    while (KEEP_READING() && F_(isdigr_)(next, base)) {
                        if (!ovf) {
                            r.i *= base;
                            if ((uintmax_t)r.i < (uintmax_t)pr) {
                                ovf = 1;
                            } else {
                                pr = r.i;
                                r.i += F_(ctorn_)(next, base);
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
                            r.i = (intmax_t)UINTMAX_MAX;
                        else if (negative)
                            r.i = -r.i;
                    } else {
                        if (ovf || r.i < 0)
                            r.i = negative ? INTMAX_MIN : INTMAX_MAX;
                        else if (negative) {
                            r.i = -r.i;
                            if (r.i >= 0)
                                r.i = INTMAX_MIN;
                        }
                    }

                    MATCH_SUCCESS();

                    if (nostore)
                        break;
                    ++fields;
                }
            storenum:
                /* store number, either as ptr, unsigned or signed */
                if (isptr) {
                    r.p = INT_TO_PTR(r.i);
            storeptr:
                    STORE_DST(r.p, void *);
                } else {
                    switch (dlen) {
                    case LN_hh:
                        if (unsign) STORE_DST(r.i, unsigned char);
                        else        STORE_DST(r.i, signed char);
                        break;
#if !SHORT_IS_INT
                    case LN_h: /* if SHORT_IS_INT, match fails => default: */
                        if (unsign) STORE_DST(r.i, unsigned short);
                        else        STORE_DST(r.i, short);
                        break;
#endif
#ifndef INTMAXT_ALIAS
                    case LN_j:
                        if (unsign) STORE_DST(r.i, uintmax_t);
                        else        STORE_DST(r.i, intmax_t);
                        break;
#endif
#ifndef SIZET_ALIAS
                    case LN_z:
                        STORE_DST(r.i, size_t);
                        break;
#endif
#ifndef PTRDIFFT_ALIAS
                    case LN_t:
                        STORE_DST(r.i, ptrdiff_t);
                        break;
#endif
#if !SCANF_DISABLE_SUPPORT_LONG_LONG
                    case LN_ll:
#if !LLONG_IS_LONG
                        if (unsign) STORE_DST(r.i, unsigned long long);
                        else        STORE_DST(r.i, long long);
                        break;
#endif
#endif /* SCANF_DISABLE_SUPPORT_LONG_LONG */
                    case LN_l:
#if !LONG_IS_INT
                        if (unsign) STORE_DST(r.i, unsigned long);
                        else        STORE_DST(r.i, long);
                        break;
#endif
                    default:
                        if (unsign) STORE_DST(r.i, unsigned);
                        else        STORE_DST(r.i, int);
                    }
                }
                break;
            } /* =========== READ INT =========== */

            case C_('e'): case C_('E'): /* scientific format float */
            case C_('f'): case C_('F'): /* decimal format float */
            case C_('g'): case C_('G'): /* decimal/scientific format float */
            case C_('a'): case C_('A'): /* hex format float */
                /* all treated equal by scanf, but not by printf */
#if SCANF_DISABLE_SUPPORT_FLOAT
                /* no support here */
                MATCH_FAILURE();
#else
            { /* =========== READ FLOAT =========== */
                floatmax_t r = 0, pr = 0;
                intmax_t off = 0, exp = 0;
                int base = 10, sub = 0;
                UCHAR explc = C_('e'), expuc = C_('E');
                /* negative? allow dot? read >0 digits? overflow? hex mode? */
                BOOL negative = 0, dot = 0, digit = 0, ovf = 0, hex = 0;
                if (!maxlen) maxlen = SIZE_MAX;
                
                switch (next) {
                case C_('-'):
                    negative = 1;
                case C_('+'):
                    NEXT_CHAR(nowread);
                }

#if SCANF_INFINITE
                if (KEEP_READING() && (next == C_('n') || next == C_('N'))) {
                    NEXT_CHAR(nowread);
                    if (!KEEP_READING() || !(next == C_('a')
                                          || next == C_('A')))
                        MATCH_FAILURE();
                    NEXT_CHAR(nowread);
                    if (!KEEP_READING() || !(next == C_('n')
                                          || next == C_('N')))
                        MATCH_FAILURE();
                    NEXT_CHAR(nowread);
                    if (KEEP_READING() && next == C_('(')) {
                        while (KEEP_READING()) {
                            NEXT_CHAR(nowread);
                            if (next == C_(')')) {
                                NEXT_CHAR(nowread);
                                break;
                            } else if (next != C_('_') && !F_(isalnum)(next))
                                MATCH_FAILURE();
                        }
                    }
                    r = (negative ? -1 : 1) * NAN;
                    goto got_f_result;
                } else if (KEEP_READING() && (next == C_('i')
                                           || next == C_('I'))) {
                    NEXT_CHAR(nowread);
                    if (!KEEP_READING() || !(next == C_('n')
                                          || next == C_('N')))
                        MATCH_FAILURE();
                    NEXT_CHAR(nowread);
                    if (!KEEP_READING() || !(next == C_('f')
                                          || next == C_('F')))
                        MATCH_FAILURE();
                    NEXT_CHAR(nowread);
                    /* try reading the rest */
                    if (KEEP_READING()) {
                        int k;
                        const CHAR *rest2 = S_("INITY");
                        for (k = 0; k < 5; ++k) {
                            if (!KEEP_READING() ||
                                    (next != rest2[k] && 
                                     next != F_(tolower)(rest2[k])))
                                break;
                            NEXT_CHAR(nowread);
                        }
                    }
                    r = (negative ? -1 : 1) * INFINITY;
                    goto got_f_result;
                }
#endif /* SCANF_INFINITE */

                /* 0x for hex floats */
                if (KEEP_READING() && next == C_('0')) {
                    NEXT_CHAR(nowread);
                    digit = 1;
                    if (KEEP_READING() && (next == C_('x')
                                        || next == C_('X'))) {
                        base = 16;
                        hex = 1;
                        explc = C_('p'), expuc = C_('P');
                        NEXT_CHAR(nowread);
                    }
                }

                while (KEEP_READING() && next == C_('0')) {
                    NEXT_CHAR(nowread);
                    digit = 1;
                }
                
                /* read digits and convert */
                while (KEEP_READING() && (F_(isdigr_)(next, base) ||
                                         (next == C_('.') && !dot))) {
                    if (next == C_('.'))
                        dot = 1, sub = hex ? 4 : 1;
                    else if (!ovf) {
                        r *= base;
                        if (r > 0 && r == pr) {
                            ovf = 1;
                        } else {
                            pr = r;
                            r += F_(ctorn_)(next, base);
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
                        case C_('-'):
                            eneg = 1;
                        case C_('+'):
                            NEXT_CHAR(nowread);
                        }
                    }

                    /* skip initial zeros */
                    while (KEEP_READING() && next == C_('0')) {
                        NEXT_CHAR(nowread);
                        edigit = 1;
                    }
                    /* read exp from stream */
                    while (KEEP_READING() && F_(isdigit)(next)) {
                        if (!eovf) {
                            exp *= 10;
                            if ((uintmax_t)exp < (uintmax_t)pe) {
                                eovf = 1;
                            } else {
                                pe = exp;
                                exp += F_(ctodn_)(next);
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
            case C_('c'): 
            { /* =========== READ CHAR =========== */
                CHAR *outp;
#if SCANF_WIDE_CONVERT
                BOOL wide = dlen == LN_l;
#elif SCANF_WIDE /* SCANF_WIDE_CONVERT */
                if (dlen != LN_l) /* read narrow but not supported */
                    MATCH_FAILURE();
#else /* SCANF_WIDE_CONVERT */
                if (dlen == LN_l) /* read wide but not supported */
                    MATCH_FAILURE();
#endif /* SCANF_WIDE_CONVERT */
                outp = (CHAR *)dst;
                if (!maxlen) maxlen = 1;
#if SCANF_WIDE_CONVERT
#if SCANF_WIDE
                if (!wide) goto rwc_wn;
#else
                if (wide) goto rwc_nw;
#endif
#endif /* SCANF_WIDE_CONVERT */
                while (KEEP_READING()) {
                    if (!nostore) *outp++ = (CHAR)(UCHAR)next;
                    NEXT_CHAR(nowread);
                }
                if (nowread < maxlen)
                    MATCH_FAILURE();
                if (!nostore) ++fields;
                MATCH_SUCCESS();
                break;
#if SCANF_WIDE_CONVERT
#if SCANF_WIDE
            rwc_wn: /* %c with wc -> mb conversion */
                {
                    char *outc = (char *)dst;
                    size_t mbr;
                    char tmp[MB_LEN_MAX];
                    if (nostore)
                        outc = tmp;
                    mbsetup_(&mbstate);
                    while (KEEP_READING()) {
                        mbr = wcrtomb_(outc, next, &mbstate);
                        if (mbr == (size_t)(-1))
                            MATCH_FAILURE();
                        else if (mbr > 0 && !nostore)
                            outc += mbr;
                        NEXT_CHAR(nowread);
                    }
                    if (!nostore) ++fields;
                    MATCH_SUCCESS();
                    break;
                }
#else /* SCANF_WIDE */
            rwc_nw: /* %c with mb -> wc conversion */
                {
                    WCHAR *outw = (WCHAR *)dst;
                    size_t mbr;
                    WCHAR tmp;
                    if (nostore)
                        outw = &tmp;
                    mbsetup_(&mbstate);
                    while (KEEP_READING()) {
                        char nc = (char)next;
                        mbr = mbrtowc_(outw, &nc, 1, &mbstate);
                        if (mbr == (size_t)(-1))
                            MATCH_FAILURE();
                        else if (mbr != (size_t)(-2) && !nostore)
                            ++outw;
                        NEXT_CHAR(nowread);
                    }
                    if (!nostore) ++fields;
                    MATCH_SUCCESS();
                    break;
                }
#endif /* SCANF_WIDE */
#endif /* SCANF_WIDE_CONVERT */
            } /* =========== READ CHAR =========== */
            case C_('s'): 
            { /* =========== READ STR =========== */
                CHAR *outp;
#if SCANF_WIDE_CONVERT
                BOOL wide = dlen == LN_l;
#elif SCANF_WIDE /* SCANF_WIDE_CONVERT */
                if (dlen != LN_l) /* read narrow but not supported */
                    MATCH_FAILURE();
#else /* SCANF_WIDE_CONVERT */
                if (dlen == LN_l) /* read wide but not supported */
                    MATCH_FAILURE();
#endif /* SCANF_WIDE_CONVERT */
                outp = (CHAR *)dst;
                if (!maxlen)
#if SCANF_SECURE
                    MATCH_FAILURE();
#else
                    maxlen = SIZE_MAX;
#endif
#if SCANF_WIDE_CONVERT
#if SCANF_WIDE
                if (!wide) goto rws_wn;
#else
                if (wide) goto rws_nw;
#endif
#endif /* SCANF_WIDE_CONVERT */

                while (KEEP_READING() && !F_(isspace)(next)) {
                    if (!nostore) *outp++ = (CHAR)(UCHAR)next;
                    NEXT_CHAR(nowread);
                }
                if (!nowread)
                    MATCH_FAILURE();
                if (!nostore) {
                    *outp = C_('\0');
                    ++fields;
                }
                MATCH_SUCCESS();
                break;
#if SCANF_WIDE_CONVERT
#if SCANF_WIDE
            rws_wn: /* %s with wc -> mb conversion */
                {
                    char *outc = (char *)dst;
                    size_t mbr;
                    char tmp[MB_LEN_MAX];
                    if (nostore)
                        outc = tmp;
                    mbsetup_(&mbstate);
                    while (KEEP_READING() && !F_(isspace)(next)) {
                        mbr = wcrtomb_(outc, next, &mbstate);
                        if (mbr == (size_t)(-1))
                            MATCH_FAILURE();
                        else if (mbr > 0 && !nostore)
                            outc += mbr;
                        NEXT_CHAR(nowread);
                    }
                    if (!nostore) {
                        *outc = C_('\0');
                        ++fields;
                    }
                    MATCH_SUCCESS();
                    break;
                }
#else /* SCANF_WIDE */
            rws_nw: /* %s with mb -> wc conversion */
                {
                    WCHAR *outw = (WCHAR *)dst;
                    size_t mbr;
                    WCHAR tmp;
                    if (nostore)
                        outw = &tmp;
                    mbsetup_(&mbstate);
                    while (KEEP_READING() && !F_(isspace)(next)) {
                        char nc = (char)next;
                        mbr = mbrtowc_(outw, &nc, 1, &mbstate);
                        if (mbr == (size_t)(-1))
                            MATCH_FAILURE();
                        else if (mbr != (size_t)(-2) && !nostore)
                            ++outw;
                        NEXT_CHAR(nowread);
                    }
                    if (!nostore) {
                        *outw = C_('\0');
                        ++fields;
                    }
                    MATCH_SUCCESS();
                    break;
                }
#endif /* SCANF_WIDE */
#endif /* SCANF_WIDE_CONVERT */
            } /* =========== READ STR =========== */
            case C_('['):
            { /* =========== READ SCANSET =========== */
                CHAR *outp;
                BOOL invert = 0;
#if SCANF_FAST_SCANSET && !SCANF_WIDE
                BOOL hyphen = 0;
                BOOL mention[UCHAR_MAX + 1] = { 0 };
                UCHAR prev = 0, c;
#else
                const UCHAR *set;
#endif
#if SCANF_WIDE_CONVERT
                BOOL wide = dlen == LN_l;
#elif SCANF_WIDE /* SCANF_WIDE_CONVERT */
                if (dlen != LN_l) /* read narrow but not supported */
                    MATCH_FAILURE();
#else /* SCANF_WIDE_CONVERT */
                if (dlen == LN_l) /* read wide but not supported */
                    MATCH_FAILURE();
#endif /* SCANF_WIDE_CONVERT */
                outp = (CHAR *)dst;
                if (!maxlen)
#if SCANF_SECURE
                    MATCH_FAILURE();
#else
                    maxlen = SIZE_MAX;
#endif
                ++f;
                if (*f == C_('^'))
                    invert = 1, ++f;
                if (*f == C_(']'))
                    ++f;
#if SCANF_FAST_SCANSET && !SCANF_WIDE
                while ((c = *f) && c != C_(']')) {
                    if (hyphen) {
                        int k;
                        for (k = prev; k <= c; ++k)
                            mention[k] = 1;
                        hyphen = 0;
                        prev = c;
                    } else if (c == C_('-') && prev)
                        hyphen = 1;
                    else
                        mention[c] = 1, prev = c;
                    ++f;
                }
                if (hyphen)
                    mention[C_('-')] = 1;
#else /* SCANF_FAST_SCANSET */
                set = f;
                while (*f && *f != C_(']'))
                    ++f;
#endif /* SCANF_FAST_SCANSET */
#if SCANF_WIDE_CONVERT
#if SCANF_WIDE
                if (!wide) goto rwsc_wn;
#else
                if (wide) goto rwsc_nw;
#endif
#endif
                while (KEEP_READING()) {
#if SCANF_FAST_SCANSET && !SCANF_WIDE
                    if (mention[next] == invert) break;
#else
                    if (F_(inscan_)(set, f, next) == invert) break;
#endif
                    if (!nostore) *outp++ = (CHAR)(UCHAR)next;
                    NEXT_CHAR(nowread);
                }
                if (!nostore) {
                    *outp = C_('\0');
                    ++fields;
                }
                MATCH_SUCCESS();
                break;
#if SCANF_WIDE_CONVERT
#if SCANF_WIDE
            rwsc_wn: /* %[ with wc -> mb conversion */
                {
                    char *outc = (char *)dst;
                    size_t mbr;
                    char tmp[MB_LEN_MAX];
                    if (nostore)
                        outc = tmp;
                    mbsetup_(&mbstate);
                    while (KEEP_READING()) {
#if SCANF_FAST_SCANSET && !SCANF_WIDE
                        if (mention[next] == invert) break;
#else
                        if (F_(inscan_)(set, f, next) == invert) break;
#endif
                        mbr = wcrtomb_(outc, next, &mbstate);
                        if (mbr == (size_t)(-1))
                            MATCH_FAILURE();
                        else if (mbr > 0 && !nostore)
                            outc += mbr;
                        NEXT_CHAR(nowread);
                    }
                    if (!nostore) {
                        *outc = C_('\0');
                        ++fields;
                    }
                    MATCH_SUCCESS();
                    break;
                }
#else /* SCANF_WIDE */
            rwsc_nw: /* %[ with mb -> wc conversion */
                {
                    WCHAR *outw = (WCHAR *)dst;
                    size_t mbr;
                    WCHAR tmp;
                    if (nostore)
                        outw = &tmp;
                    mbsetup_(&mbstate);
                    while (KEEP_READING()) {
                        char nc = (char)next;
#if SCANF_FAST_SCANSET && !SCANF_WIDE
                        if (mention[next] == invert) break;
#else
                        if (F_(inscan_)(set, f, next) == invert) break;
#endif
                        mbr = mbrtowc_(outw, &nc, 1, &mbstate);
                        if (mbr == (size_t)(-1))
                            MATCH_FAILURE();
                        else if (mbr != (size_t)(-2) && !nostore)
                            ++outw;
                        NEXT_CHAR(nowread);
                    }
                    if (!nostore) {
                        *outw = C_('\0');
                        ++fields;
                    }
                    MATCH_SUCCESS();
                    break;
                }
#endif /* SCANF_WIDE */
#endif /* SCANF_WIDE_CONVERT */
            } /* =========== READ SCANSET =========== */
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

#if SCANF_WIDE

#if SCANF_SSCANF_ONLY

int vwscanf_(const WCHAR *format, va_list arg) {
    return WEOF;
}

int wscanf_(const WCHAR *format, ...) {
    return WEOF;
}

#else /* SCANF_SSCANF_ONLY */

static WINT getwchw_(void *arg) {
    return getwch_();
}

static void ungetwchw_(WINT c, void *arg) {
    ungetwch_(c);
}

int vwscanf_(const WCHAR *format, va_list arg) {
    return F_(iscanf_)(&getwchw_, &ungetwchw_, NULL, format, arg);
}

int wscanf_(const WCHAR *format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vwscanf_(format, va);
    va_end(va);
    return r;
}

#endif /* SCANF_SSCANF_ONLY */

static WINT wsscanw_(void *arg) {
    const WCHAR **p = (const WCHAR **)arg;
    const WCHAR c = *(*p)++;
    return c ? c : EOF;
}

int vspwscanf_(const WCHAR **sp, const WCHAR *format, va_list arg) {
    int i = F_(iscanf_)(&wsscanw_, NULL, sp, format, arg);
    --*sp; /* back up by one character, even if it was EOF we want the pointer
              at the null terminator */
    return i;
}

int spwscanf_(const WCHAR **sp, const WCHAR *format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vspwscanf_(sp, format, va);
    va_end(va);
    return r;
}

int vswscanf_(const WCHAR *s, const WCHAR *format, va_list arg) {
    return F_(iscanf_)(&wsscanw_, NULL, &s, format, arg);
}

int swscanf_(const WCHAR *s, const WCHAR *format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vswscanf_(s, format, va);
    va_end(va);
    return r;
}

int vfctwscanf_(WINT (*getwch)(void *data),
                void (*ungetwch)(WINT c, void *data),
                void *data, const WCHAR *format, va_list arg) {
    return F_(iscanf_)(getwch, ungetwch, data, format, arg);
}

int fctwscanf_(WINT (*getwch)(void *data),
                void (*ungetwch)(WINT c, void *data),
                void *data, const WCHAR *format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vfctwscanf_(getwch, ungetwch, data, format, va);
    va_end(va);
    return r;
}

#else /* SCANF_WIDE */

#if SCANF_SSCANF_ONLY

int vscanf_(const char *format, va_list arg) {
    return EOF;
}

int scanf_(const char *format, ...) {
    return EOF;
}

#else /* SCANF_SSCANF_ONLY */

static int getchw_(void *arg) {
    return getch_();
}

static void ungetchw_(int c, void *arg) {
    ungetch_(c);
}

int vscanf_(const char *format, va_list arg) {
    return iscanf_(&getchw_, &ungetchw_, NULL, format, arg);
}

int scanf_(const char *format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vscanf_(format, va);
    va_end(va);
    return r;
}

#endif /* SCANF_SSCANF_ONLY */

static int sscanw_(void *arg) {
    const unsigned char **p = (const unsigned char **)arg;
    const unsigned char c = *(*p)++;
    return c ? c : EOF;
}

int vspscanf_(const char **sp, const char *format, va_list arg) {
    int i = iscanf_(&sscanw_, NULL, sp, format, arg);
    --*sp; /* back up by one character, even if it was EOF we want the pointer
              at the null terminator */
    return i;
}

int spscanf_(const char **sp, const char *format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vspscanf_(sp, format, va);
    va_end(va);
    return r;
}

int vsscanf_(const char *s, const char *format, va_list arg) {
    return iscanf_(&sscanw_, NULL, &s, format, arg);
}

int sscanf_(const char *s, const char *format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vsscanf_(s, format, va);
    va_end(va);
    return r;
}

int vfctscanf_(int (*getch)(void *data), void (*ungetch)(int c, void *data),
                void *data, const char *format, va_list arg) {
    return iscanf_(getch, ungetch, data, format, arg);
}

int fctscanf_(int (*getch)(void *data), void (*ungetch)(int c, void *data),
                void *data, const char *format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = vfctscanf_(getch, ungetch, data, format, va);
    va_end(va);
    return r;
}

#endif /* SCANF_WIDE */

#if SCANF_WIDE >= 2
/* reinclude with SCANF_WIDE=0 to get narrow impls */
#undef SCANF_WIDE
#define SCANF_WIDE 0
#define SCANF_REPEAT
#include "scanf.c"
#endif
