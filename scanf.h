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

#ifndef SCANF_H
#define SCANF_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int scanf_(const char* format, ...);
int sscanf_(const char *s, const char* format, ...);
int spscanf_(const char **sp, const char* format, ...);
int fctscanf_(int (*getch)(void* data), void (*ungetch)(int c, void* data), void* data, const char* format, ...);

int vscanf_(const char* format, va_list arg);
int vsscanf_(const char *s, const char* format, va_list arg);
int vspscanf_(const char **sp, const char* format, va_list arg);
int vfctscanf_(int (*getch)(void* data), void (*ungetch)(int c, void* data), void* data, const char* format, va_list arg);

int getch_(void);
void ungetch_(int);

#define scanf scanf_
#define sscanf sscanf_
#define spscanf spscanf_
#define fctscanf fctscanf_
#define vscanf vscanf_
#define vsscanf vsscanf_
#define vspscanf vspscanf_
#define vfctscanf vfctscanf_

#ifdef __cplusplus
}
#endif

#endif /* SCANF_H */
