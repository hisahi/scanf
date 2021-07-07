/*

scanf implementation (example scnext_ for IPv4 addresses)
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

#include "../scanf.h"
#include <stdint.h>

/* see EXTENSIONS for more details. this is an example implementation of a
   custom format specifiers for reading an IPv4 address and returning it as
   a uint32_t. */

struct rescanf_tmp {
    void *udata;
    int (*getch)(void *data);
    int next;
};

int rescanf_getch_(void *data) {
    struct rescanf_tmp *rsf = (struct rescanf_tmp *)data;
    if (rsf->next >= 0) {
        int c = rsf->next;
        rsf->next = -1;
        return c;
    }
    return rsf->getch(rsf->udata);
}

void rescanf_ungetch_(int ch, void *data) {
    struct rescanf_tmp *rsf = (struct rescanf_tmp *)data;
    rsf->next = ch;
}

/* for calling scanf within scnext_ */
int rescanf(int (*getch)(void *data), void *data, int *next, 
            const char* format, ...) {
    int r;
    va_list va;
    struct rescanf_tmp tmp;
    va_start(va, format);
    tmp.udata = data;
    tmp.getch = getch;
    tmp.next = *next;
    r = vfctscanf(&rescanf_getch_, &rescanf_ungetch_, &tmp, format, va);
    va_end(va);
    *next = tmp.next;
    return r;
}

/* IPv4: %!I4 */
int scnext_(int (*getch)(void *data), void *data, const char **format,
            int *buffer, int length, int nostore, void *destination) {
    int next = *buffer;
    int ret = 0;
    const char *f = *format;

    if (f[0] == 'I' && f[1] == '4') {
        unsigned char i1, i2, i3, i4;
        int r = rescanf(getch, data, &next, "%hhu.%hhu.%hhu.%hhu",
                        &i1, &i2, &i3, &i4);
        f += 2;
        if (r < 0)
            ret = r; /* input error */
        else if (r < 4)
            ret = 1; /* matching error */
        else if (!nostore) {
            uint32_t ip = (i1 << 24) | (i2 << 16) | (i3 << 8) | (i4);
            *(uint32_t *)destination = ip;
            ret = 0; /* OK */
        }
    } else
        ret = 1; /* unknown specifier, matching error */

    *format = f;
    *buffer = next;
    return ret;
}
