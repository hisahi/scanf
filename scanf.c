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
#undef SCANF_DISABLE_SUPPORT_LONG_LONG
#define SCANF_DISABLE_SUPPORT_LONG_LONG 1
#endif

#if SCANF_C99 || SCANF_STDINT
#include <stdint.h>
#ifndef INTMAX_MAX
#define INTMAX_MAX LLONG_MAX
#endif
#ifndef UINTMAX_MAX
#define UINTMAX_MAX ULLONG_MAX
#endif
#else
#if SCANF_C99
#define intmax_t long long int
#define uintmax_t unsigned long long int
#ifndef INTMAX_MAX
#define INTMAX_MAX LLONG_MAX
#endif
#ifndef UINTMAX_MAX
#define UINTMAX_MAX ULLONG_MAX
#endif
#else
#define intmax_t long int
#define uintmax_t unsigned long int
#ifndef INTMAX_MAX
#define INTMAX_MAX LONG_MAX
#endif
#ifndef UINTMAX_MAX
#define UINTMAX_MAX ULONG_MAX
#endif
#endif
#endif

#define maxfloat_t long double

#if SCANF_C99
#define INLINE static inline
#else
#define INLINE static
#endif

#ifndef SCANF_ATON_BUFFER_SIZE
#define SCANF_ATON_BUFFER_SIZE 32
#endif

#ifndef SCANF_ATOF_BUFFER_SIZE
#define SCANF_ATOF_BUFFER_SIZE 32
#endif

#ifndef SCANF_ASCII
#define SCANF_ASCII 1
#endif

#ifndef SCANF_SATURATE
#define SCANF_SATURATE 0
#endif

#ifndef SCANF_FAST_SCANSET
#if CHAR_BIT == 8
#define SCANF_FAST_SCANSET 1
#else
#define SCANF_FAST_SCANSET 0
#endif
#endif

#ifndef SCANF_NOMATH
#define SCANF_NOMATH 1
#endif

#ifndef SCANF_INTERNAL_CTYPE
#define SCANF_INTERNAL_CTYPE 0
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

#ifndef EOF
#define EOF -1
#endif

INLINE int ctodn_(const char c) {
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

INLINE int ctoon_(const char c) {
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

INLINE int ctoxn_(const char c) {
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

INLINE int ctobn_(const char c, int b) {
    if (b == 8)
        return ctoon_(c);
    else if (b == 16)
        return ctoxn_(c);
    else /* if (b == 10) */
        return ctodn_(c);
}

#if SCANF_INTERNAL_CTYPE
INLINE int isspace(int c) {
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

INLINE int isdigo_(const char c) {
#if SCANF_ASCII
    return '0' <= c && c <= '7';
#else
    return ctoon_(c) >= 0;
#endif
}

INLINE int isdigx_(const char c) {
#if SCANF_ASCII
    return isdigit(c) || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
#else
    return ctoxn_(c) >= 0;
#endif
}

INLINE int isdigb_(const char c, int b) {
    if (b == 8)
        return isdigo_(c);
    else if (b == 16)
        return isdigx_(c);
    else /* if (b == 10) */
        return isdigit(c);
}

static size_t atodz_(const char* s) {
    char c;
    size_t r = 0;
#if SCANF_SATURATE
    size_t pr = 0;
#endif
    while ((c = *s++)) {
        r *= 10;  
#if SCANF_SATURATE
        if (r < pr)
            return SIZE_MAX;
        pr = r;
#endif
        r += ctodn_(c);
    }
    return r;
}

static uintmax_t atobn_(const char* s, int b) {
    char c;
    uintmax_t r = 0;
#if SCANF_SATURATE
    uintmax_t pr = 0;
#endif
    while ((c = *s++)) {
        r *= b;
#if SCANF_SATURATE
        if (r < pr)
            return UINTMAX_MAX;
        pr = r;
#endif
        r += ctobn_(c, b);
    }
    return r;
}

#if !SCANF_DISABLE_SUPPORT_FLOAT

#if !SCANF_NOMATH
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

static maxfloat_t atolf_(const char* s, int negative, intmax_t exp) {
    maxfloat_t r = 0;
    while (isdigit(*s))
        r = r * 10 + ctodn_(*s++);
    if (*s == '.') {
        ++s;
        while (isdigit(*s))
            r = r * 10 + ctodn_(*s++), --exp;
    }

    if (exp > 0) {
#ifdef INFINITY
        if (exp > LDBL_MAX_10_EXP)
            r = INFINITY;
        else
#endif
            r *= pow10_(exp);
    } else if (exp < 0) {
        if (exp < LDBL_MIN_10_EXP)
            r = 0;
        else
            r /= pow10_(-exp);
    }
    if (negative) r = -r;
    return r;
}

static maxfloat_t atoxlf_(const char* s, int negative, intmax_t exp) {
    maxfloat_t r = 0;
    while (isdigit(*s))
        r = r * 16 + ctoxn_(*s++);
    if (*s == '.') {
        ++s;
        while (isdigit(*s))
            r = r * 16 + ctoxn_(*s++), exp -= 4;
    }

    if (exp > 0) {
#ifdef INFINITY
        if (exp > LDBL_MAX_EXP)
            r = INFINITY;
        else
#endif
            r *= pow2_(exp);
    } else if (exp < 0) {
        if (exp < LDBL_MIN_EXP)
            r = 0;
        else
            r /= pow2_(-exp);
    }
    if (negative) r = -r;
    return r;
}

#endif

enum dlength { LN_, LN_hh, LN_h, LN_l, LN_ll, LN_j, LN_z, LN_t, LN_L };

#define IS_EOF(c) ((c) < 0)
#define GOT_EOF() (IS_EOF(next))

static int iscanf_(int (*getch)(void* p), void (*ungetch)(int c, void* p),
                   void* p, const char* ff, va_list va) {
    /* fields = number of fields successfully read; this is the return value */
    /* next = the "next" character to be processed, initially -1 */
    int fields = 0, next = -1;
    /* total characters read, returned by %n */
    size_t read_chars = 0;
    const unsigned char *f = (const unsigned char *)ff;
    unsigned char c;
    while ((c = *f++)) {
        if (IS_EOF(next)) {
            /* read and cache first character */
            next = getch(p);
            if (GOT_EOF()) return EOF;
            /* ++read_chars; intentionally left out, otherwise %n is off by 1 */
        }
        if (c == '%') {
            /* nostore is %*, prevents a value from being stored */
            int nostore = 0;
            /* nowread = characters read for this format specifier
               maxlen = maximum number of characters to be read "field width" */
            size_t nowread = 0, maxlen = 0;
            /* still characters to read? (not EOF and width not exceeded) */
#define KEEP_READING() (nowread < maxlen && !GOT_EOF())
            /* length specifier (l, ll, h, hh...) */
            enum dlength dlen = LN_;
            
            /* nostore */
            if (*f == '*') {
                nostore = 1;
                ++f;
            }

            /* width specifier => maxlen */
            if (isdigit(*f)) {
                char aton[SCANF_ATON_BUFFER_SIZE], *di = aton;
                int k;
                while (*f == '0')
                    ++f;
                for (k = 0; isdigit(*f) && k < SCANF_ATON_BUFFER_SIZE - 1; ++k)
                    *di++ = *f++;
                *di++ = 0;
                maxlen = atodz_(aton);
            }

            /* length specifier */
            switch (*f) {
            case 'h':
                if (*++f == 'h')
                    dlen = LN_hh, ++f;
                else
                    dlen = LN_h;
                break;
            case 'l':
#if !SCANF_DISABLE_SUPPORT_LONG_LONG
                if (*++f == 'l')
                    dlen = LN_ll, ++f;
                else
#endif
                    dlen = LN_l;
                break;
            case 'j':
                dlen = LN_j, ++f;
                break;
            case 't':
                dlen = LN_t, ++f;
                break;
            case 'z':
                dlen = LN_z, ++f;
                break;
#if !SCANF_DISABLE_SUPPORT_FLOAT
            case 'L':
                dlen = LN_L, ++f;
                break;
#endif
            }

            switch (*f) {
            /* do not skip whitespace for... */
            case '[':
            case 'c':
            case 'n':
            /* case '%': */
                break;
            default:
                /* skip whitespace. include in %n, but not elsewhere */
                while (!GOT_EOF() && isspace(next)) {
                    next = getch(p);
                    ++read_chars;
                }
            }

            if (GOT_EOF())
                goto read_failure;

            /* format */
            switch (*f) {
            case '%': 
                /* literal % */
                if (next != '%')
                    goto read_failure;
                next = getch(p);
                ++read_chars;
                break;
            case 'd': /* signed decimal integer */
            case 'u': /* unsigned decimal integer */ 
            case 'i': /* signed decimal/hex/binary integer */
            case 'o': /* signed octal integer */
            case 'x': /* signed hexadecimal integer */
            case 'X':
            {
                intmax_t r = 0;
                /* had minus? */
                int negative = 0;
                /* allow empty = zero? */
                int zero = 0;
                /* decimal, hexadecimal, binary */
                int base;
                /* unsigned? */
                int unsign;
                char aton[SCANF_ATON_BUFFER_SIZE], *di = aton;
                int k;
                if (!maxlen) maxlen = (size_t)-1;
                c = *f;

                switch (c) {
                case 'o':
                    base = 8;
                    unsign = 1;
                    break;
                case 'x':
                    base = 16;
                    unsign = 1;
                    break;
                default:
                    base = 10;
                    unsign = c == 'u';
                }

                /* sign, read even for %u */
                switch (next) {
                case '-':
                    negative = 1;
                case '+':
                    next = getch(p);
                    ++nowread;
                }
                if (c == 'i') { /* detect base from string */
                    if (KEEP_READING() && next == '0') {
                        zero = 1;
                        base = 8;
                        next = getch(p);
                        ++nowread;
                        if (KEEP_READING() && (next == 'x'
                                                 || next == 'X')) {
                            base = 16;
                            next = getch(p);
                            ++nowread;
                        }
                    }
                } else if (c == 'x') { /* skip 0x for %x */
                    if (KEEP_READING() && next == '0') {
                        zero = 1;
                        next = getch(p);
                        ++nowread;
                        if (KEEP_READING() && (next == 'x'
                                                 || next == 'X')) {
                            next = getch(p);
                            ++nowread;
                        }
                    }
                }
                /* skip initial zeros */
                while (KEEP_READING() && next == '0') {
                    next = getch(p);
                    ++nowread;
                    zero = 1;
                }
                /* read numbers into buffer */
                for (k = 0; KEEP_READING() && isdigb_(next, base); ++k) {
                    if (k < SCANF_ATON_BUFFER_SIZE - 1)
                        *di++ = next;
                    next = getch(p);
                    ++nowread;
                }
                *di++ = 0;
                /* if buffer empty, could not read */
                if (!*aton && !zero)
                    goto read_failure;
                else if (!*aton)
                    r = 0;
                /* too many digits, overflow! */
                else if (k >= SCANF_ATON_BUFFER_SIZE - 1)
                    r = (intmax_t)UINTMAX_MAX;
                else
                    r = (intmax_t)atobn_(aton, base);
                if (negative) r *= -1;
                if (!nostore) {
                    ++fields;
                    if (unsign) {
                        switch (dlen) {
                        case LN_hh:
                            *(va_arg(va, unsigned char *)) = (unsigned char)r;
                            break;
                        case LN_h:
                            *(va_arg(va, unsigned short *)) = (unsigned short)r;
                            break;
                        case LN_l:
                            *(va_arg(va, unsigned long *)) = (unsigned long)r;
                            break;
                        case LN_j:
                            *(va_arg(va, uintmax_t *)) = (uintmax_t)r;
                            break;
                        case LN_z:
                            *(va_arg(va, size_t *)) = (size_t)r;
                            break;
                        case LN_t:
                            *(va_arg(va, ptrdiff_t *)) = (ptrdiff_t)r;
                            break;
#if !SCANF_DISABLE_SUPPORT_LONG_LONG
                        case LN_ll:
                            *(va_arg(va, unsigned long long *))
                                        = (unsigned long long)r;
                            break;
#endif
                        default:
                            *(va_arg(va, unsigned *)) = (unsigned)r;
                        }
                    } else {
                        switch (dlen) {
                        case LN_hh:
                            *(va_arg(va, signed char *)) = (signed char)r;
                            break;
                        case LN_h:
                            *(va_arg(va, short *)) = (short)r;
                            break;
                        case LN_l:
                            *(va_arg(va, long *)) = (long)r;
                            break;
                        case LN_j:
                            *(va_arg(va, intmax_t *)) = r;
                            break;
                        case LN_z:
                            *(va_arg(va, size_t *)) = (size_t)r;
                            break;
                        case LN_t:
                            *(va_arg(va, ptrdiff_t *)) = (ptrdiff_t)r;
                            break;
#if !SCANF_DISABLE_SUPPORT_LONG_LONG
                        case LN_ll:
                            *(va_arg(va, long long *)) = (long long)r;
                            break;
#endif
                        default:
                            *(va_arg(va, int *)) = (int)r;
                        }
                    }
                }
            }
                break;
            case 'e': case 'E': /* scientific format float */
            case 'f': case 'F': /* decimal format float */
            case 'g': case 'G': /* decimal/scientific format float */
            case 'a': case 'A': /* hex format float */
                /* all treated equal by scanf, but not by printf */
#if SCANF_DISABLE_SUPPORT_FLOAT
                /* no support here */
                goto read_failure;
#else
            {
                maxfloat_t r;
                char atof[SCANF_ATOF_BUFFER_SIZE], *di = atof;
                intmax_t exp = 0;
                int negative = 0, base = 10, dot = 0, zero = 0, k;
                if (!maxlen) maxlen = (size_t)-1;
                
                switch (next) {
                case '-':
                    negative = 1;
                case '+':
                    next = getch(p);
                    ++nowread;
                }

#if SCANF_INFINITE
                if (KEEP_READING() && next == 'n' || next == 'N') {
                    int k;
                    const char *rest = "AN";
                    next = getch(p);
                    ++nowread;
                    for (k = 0; k < 2; ++k) {
                        if (!KEEP_READING() ||
                                (next != rest[k] && next != tolower(rest[k])))
                            goto read_failure;
                        next = getch(p);
                        ++nowread;
                    }
                    if (KEEP_READING() && next == '(') {
                        while (KEEP_READING()) {
                            if (next == ')')
                                break;
                            else if (next != '_' && !isalnum(next))
                                goto read_failure;
                            next = getch(p);
                            ++nowread;
                        }
                        if (next == ')') {
                            next = getch(p);
                            ++nowread;
                        }
                    }

                    r = (negative ? -1 : 1) * NAN;
                    goto got_f_result;
                } else if (KEEP_READING() && next == 'i' || next == 'I') {
                    int k;
                    const char *rest = "NF";
                    next = getch(p);
                    ++nowread;
                    for (k = 0; k < 2; ++k) {
                        if (!KEEP_READING() ||
                                (next != rest[k] && next != tolower(rest[k])))
                            goto read_failure;
                        next = getch(p);
                        ++nowread;
                    }
                    r = (negative ? -1 : 1) * INFINITY;
                    goto got_f_result;
                }
#endif

                /* 0x for hex floats */
                if (KEEP_READING() && next == '0') {
                    next = getch(p);
                    ++nowread;
                    if (KEEP_READING() && (next == 'x' || next == 'X')) {
                        base = 16;
                        next = getch(p);
                        ++nowread;
                    }
                }

                /* float cannot start with a decimal point! */
                if (next == '.')
                    goto read_failure;
                while (KEEP_READING() && next == '0') {
                    next = getch(p);
                    ++nowread;
                    zero = 1;
                }
                /* copy strings of digits + decimal point to buffer */
                for (k = 0; KEEP_READING() &&
                        (isdigb_(next, base) || (!dot && next == '.')); ++k) {
                    if (k < SCANF_ATOF_BUFFER_SIZE - 1)
                        *di++ = next;
                    dot |= next == '.';
                    next = getch(p);
                    ++nowread;
                }
                *di++ = 0;

                /* if buffer empty, could not read */
                if (!*atof && !zero)
                    goto read_failure;

                /* exponent? */
                if (KEEP_READING()
                             && ((base == 16 && (next == 'p' || next == 'P'))
                             || (base == 10 && (next == 'e' || next == 'E')))) {
                    uintmax_t re = 0;
                    int eneg = 0;
                    char aton[SCANF_ATON_BUFFER_SIZE], *edi = aton;
                    next = getch(p);
                    ++nowread;
                    if (KEEP_READING() && (next == '+' || next == '-')) {
                        eneg = next == '-';
                        next = getch(p);
                        ++nowread;
                    }

                    /* skip initial zeros */
                    while (KEEP_READING() && next == '0') {
                        next = getch(p);
                        ++nowread;
                    }
                    /* read into buffer */
                    for (k = 0; KEEP_READING() && isdigit(next); ++k) {
                        if (k < SCANF_ATOF_BUFFER_SIZE - 1)
                            *edi++ = next;
                        next = getch(p);
                        ++nowread;
                    }
                    *edi++ = 0;
                    if (!*aton)
                        goto read_failure;
                    /* overflow? */
                    if (k >= SCANF_ATON_BUFFER_SIZE - 1)
                        re = (intmax_t)UINTMAX_MAX;
                    else
                        re = (intmax_t)atobn_(aton, 10);
                    exp = (intmax_t)re;
                    if (eneg) exp *= -1;
                }

                /* conversion */
                if (!*atof)
                    r = 0;
                else if (base == 16)
                    r = atoxlf_(atof, negative, exp);
                else /* base == 10 */
                    r = atolf_(atof, negative, exp);

#if SCANF_INFINITE
got_f_result:
#endif
                if (!nostore) {
                    ++fields;
                    switch (dlen) {
                    case LN_l:
                        *(va_arg(va, double *)) = (double)r;
                        break;
                    case LN_L:
                        *(va_arg(va, long double *)) = (long double)r;
                        break;
                    default:
                        *(va_arg(va, float *)) = (float)r;
                    }
                }
            }
                break;
#endif
            case 'c': 
            {
                char *outp = nostore ? NULL : va_arg(va, char *);
                if (!maxlen) maxlen = 1;
                while (KEEP_READING()) {
                    if (!nostore) *outp++ = next;
                    next = getch(p);
                    ++nowread;
                }
                if (nowread < maxlen) goto read_failure;
                if (!nostore) ++fields;
            }
                break;
            case 's': 
            {
                char *outp = nostore ? NULL : va_arg(va, char *);
                if (!maxlen)
#if SCANF_SECURE
                    goto read_failure;
#else
                    maxlen = (size_t)-1;
#endif

                while (KEEP_READING() && !isspace(next)) {
                    if (!nostore) *outp++ = next;
                    next = getch(p);
                    ++nowread;
                }
                if (!nowread) goto read_failure;
                if (!nostore) {
                    *outp++ = 0;
                    ++fields;
                }
            }
                break;
            case '[':
            {
                char *outp = nostore ? NULL : va_arg(va, char *);
                int scan_r = 0, hyphen = 0;
                char invert = 0, prev = 0;
                unsigned char c;
#if SCANF_FAST_SCANSET
                char mention[UCHAR_MAX + 1] = { 0 };
#else
                const unsigned char *set, *settmp;
#endif
                if (!maxlen)
#if SCANF_SECURE
                    goto read_failure;
#else
                    maxlen = (size_t)-1;
#endif
                ++f;
                if (*f == '^')
                    invert = 1, ++f;
#if SCANF_FAST_SCANSET
                while ((c = *f) && (!scan_r || c != ']')) {
                    if (hyphen) {
                        int k;
                        for (k = prev; k <= c; ++k)
                            mention[k] = 1;
                        hyphen = 0;
                    } else if (c == '-' && prev)
                        hyphen = 1;
                    else
                        mention[c] = 1, prev = c;
                    ++f;
                    ++scan_r;
                }
                if (hyphen)
                    mention['-'] = 1;
#else
                set = f;
                while ((c = *f) && (!scan_r || c != ']')) {
                    ++f;
                    ++scan_r;
                }
#endif
                while (KEEP_READING()) {
#if SCANF_FAST_SCANSET
                    if (mention[next] == invert) break;
#else
                    int found = 0;
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
                    next = getch(p);
                    ++nowread;
                }
                if (!nostore) {
                    *outp++ = 0;
                    ++fields;
                }
            }
                break;
            case 'p': 
            {
                uintmax_t r = 0;
                char aton[SCANF_ATON_BUFFER_SIZE], *di = aton;
                int k;
                if (!maxlen) maxlen = (size_t)-1;

                if (!KEEP_READING() || next != '0')
                    goto read_failure;
                next = getch(p);
                ++nowread;
                if (!KEEP_READING() || (next != 'x' && next != 'X'))
                    goto read_failure;
                next = getch(p);
                ++nowread;

                while (KEEP_READING() && next == '0') {
                    next = getch(p);
                    ++nowread;
                }
                for (k = 0; KEEP_READING() && isdigx_(next); ++k) {
                    if (k < SCANF_ATON_BUFFER_SIZE - 1)
                        *di++ = next;
                    next = getch(p);
                    ++nowread;
                }
                *di++ = 0;
                if (!*aton)
                    goto read_failure;
                if (k >= SCANF_ATON_BUFFER_SIZE - 1)
                    r = UINTMAX_MAX;
                else
                    r = atobn_(aton, 16);
                if (!nostore) {
                    *(va_arg(va, void **)) = (void *)r;
                    ++fields;
                }
            }
                break;
            case 'n':
                if (!nostore) {
                    switch (dlen) {
                    case LN_hh:
                        *(va_arg(va, signed char *)) = (signed char)read_chars;
                        break;
                    case LN_h:
                        *(va_arg(va, short *)) = (short)read_chars;
                        break;
                    case LN_l:
                        *(va_arg(va, long *)) = (long)read_chars;
                        break;
                    case LN_j:
                        *(va_arg(va, intmax_t *)) = (intmax_t)read_chars;
                        break;
                    case LN_z:
                        *(va_arg(va, size_t *)) = read_chars;
                        break;
                    case LN_t:
                        *(va_arg(va, ptrdiff_t *)) = (ptrdiff_t)read_chars;
                        break;
#if !SCANF_DISABLE_SUPPORT_LONG_LONG
                    case LN_ll:
                        *(va_arg(va, long long *)) = (long long)read_chars;
                        break;
#endif
                    default:
                        *(va_arg(va, int *)) = (int)read_chars;
                    }
                }
                break;
            default:
                goto read_failure;
            }

            ++f;
            read_chars += nowread;
        } else if (isspace(c)) {
            /* skip 0-N whitespace */
            while (isspace(next)) {
                if ((next = getch(p)) < 0) break;
                ++read_chars;
            }
        } else {
            /* must match literal character */
            if (next != c)
                break;
            next = getch(p);
            ++read_chars;
        }
        if (GOT_EOF())
            break;
    }
read_failure:
    if (!GOT_EOF() && ungetch)
        ungetch(next, p);
    return fields;
}

static int getchw_(void* arg) {
    return getch_();
}

static void ungetchw_(int c, void* arg) {
    ungetch_(c);
}

static int sscanw_(void* arg) {
    const char **p = (const char **)arg;
    const char c = *((*p)++);
    return c ? c : -1;
}

static void usscanw_(int c, void* arg) {
    const char **p = (const char **)arg;
    --*p;
}

int scanf_(const char* format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = iscanf_(&getchw_, &ungetchw_, NULL, format, va);
    va_end(va);
    return r;
}

int sscanf_(const char *s, const char* format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = iscanf_(&sscanw_, &usscanw_, &s, format, va);
    va_end(va);
    return r;
}

int spscanf_(const char **s, const char* format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = iscanf_(&sscanw_, &usscanw_, s, format, va);
    va_end(va);
    return r;
}

int fctscanf_(int (*getch)(void* data), void (*ungetch)(int c, void* data),
                void* data, const char* format, ...) {
    int r;
    va_list va;
    va_start(va, format);
    r = iscanf_(getch, ungetch, data, format, va);
    va_end(va);
    return r;
}

int vscanf_(const char* format, va_list arg) {
    return iscanf_(&getchw_, &ungetchw_, NULL, format, arg);
}

int vsscanf_(const char *s, const char* format, va_list arg) {
    return iscanf_(&sscanw_, &usscanw_, &s, format, arg);
}

int vspscanf_(const char **sp, const char* format, va_list arg) {
    return iscanf_(&sscanw_, &usscanw_, sp, format, arg);   
}

int vfctscanf_(int (*getch)(void* data), void (*ungetch)(int c, void* data),
                void* data, const char* format, va_list arg) {
    return iscanf_(getch, ungetch, data, format, arg);
}
