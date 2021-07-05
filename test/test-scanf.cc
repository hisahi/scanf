/*

scanf implementation test program
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

#include <algorithm>
#include <any>
#include <iomanip>
#include <iostream>
#include <tuple>

#include <cctype>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "../scanf.h"

namespace test {
#include "../scanf.c"
extern "C" {
int getch_() {
    return -1;
}

void ungetch_(int) { }
}
};

class TestCase {
public:
    virtual bool run() = 0;
    virtual const std::string& name() const = 0;
    virtual void whats_wrong(std::ostream& os) = 0;
};

template <std::size_t I>
struct buf_char {
    char s[I];  
    buf_char() {
        std::strncpy(s, "", I);
    }
    buf_char(const char* c) {
        std::strncpy(s, c, I);
    }
    bool operator==(const buf_char<I>& other) const {
        return 0 == std::strncmp(s, other.s, I);
    }
};

template <std::size_t I>
struct buf_str {
    char s[I];
    buf_str() {
        std::strncpy(s, "", I);
    }
    buf_str(const char* c) {
        if (std::strlen(c) > I)
            std::terminate();
        std::strncpy(s, c, I);
    }
    bool operator==(const buf_str<I>& other) const {
        return 0 == std::strncmp(s, other.s, I);
    }
};

template <std::size_t I>
std::ostream& operator<<(std::ostream& os, const buf_char<I>& buf) {
    return os << std::string(buf.s, I);
}

template <std::size_t I>
std::ostream& operator<<(std::ostream& os, const buf_str<I>& buf) {
    return os << buf.s;
}

template <typename I, typename... Ts>
class ScanfTestCase_ : public TestCase {
    template <std::size_t J>
    bool column_ok() const {
        return J >= readin_ || std::get<J>(ret_) == std::get<J>(comp_);
    }

    template <std::size_t J>
    bool column_ok_or_print(std::ostream& os) const {
        bool ok = std::get<J>(ret_) == std::get<J>(comp_);
        if (!ok) {
            os << "Input:  <" << input_ << ">" << std::endl;
            os << "Format: <" << format_ << ">" << std::endl;
            os << "Value #" << (J + 1) << " was wrong" << std::endl;
            os << "Type: " << typeid(typename std::tuple_element<J,
                        decltype(ret_)>::type).name() << std::endl;
            os << "Expected value: <" << std::get<J>(ret_)
                                      << ">" << std::endl;
            os << "     Got value: <" << std::get<J>(comp_)
                                      << ">" << std::endl;
        }
        return ok;
    }

    template <std::size_t... Is>
    bool whats_wrong_column(std::ostream& os,
                            std::index_sequence<Is...>) const {
        return (column_ok_or_print<Is>(os) && ...);
    }

    template <std::size_t... Is>
    bool run_(std::index_sequence<Is...>) {
        const char *s = input_.c_str();
        const char *f = format_.c_str();
        int n = test::sscanf_(s, f, &std::get<Is>(comp_)...);
        readin_ = n;
        return readin_ == fields_ && (column_ok<Is>() && ...);
    }

public:
    bool run() {
        return run_(I{});
    }

    const std::string& name() const {
        return name_;
    }
    
    void whats_wrong(std::ostream& os) {
        if (readin_ != fields_) {
            os << "Expected " << fields_ << " field(s) to be read OK,\n" <<
                  "but the return value was " << readin_ << std::endl;
        } else {
            whats_wrong_column(os, I{});
        }
    }

    ScanfTestCase_(const std::string& name, const std::string& input, int n,
                const std::string& fmt, const std::tuple<Ts...>& returns)
        : name_(name), input_(input), fields_(n), format_(fmt), ret_(returns),
          readin_(0), comp_() { }

private:
    std::string name_;
    std::string input_;
    int fields_;
    std::string format_;
    std::tuple<Ts...> ret_;
    int readin_;
    std::tuple<Ts...> comp_;
};

template <typename... Ts>
class ScanfTestCase :
    public ScanfTestCase_<std::make_index_sequence<sizeof...(Ts)>, Ts...> {
    using base = ScanfTestCase_<std::make_index_sequence<sizeof...(Ts)>, Ts...>;
    using base::ScanfTestCase_;
};

template <typename... Ts>
bool run_test(const std::string& name, int n, const std::string& input,
            const std::string& fmt, Ts&&... values) {
    auto test = ScanfTestCase<Ts...>(name, input, n, fmt,
            std::make_tuple(std::forward<Ts>(values)...));
    bool flag = test.run();
    std::cout << std::left << std::setw(70) << name
                           << std::setw(0) << std::flush;
    if (!flag) {
        std::cout << "FAIL" << std::endl;
        test.whats_wrong(std::cout);
        return false;
    }
    std::cout << "OK  " << std::endl;
    return flag;
}

#define TRY_TEST(...) do {                                                     \
        if (!run_test(__VA_ARGS__))                                            \
            return false;                                                      \
    } while (0);

/* 1, "42", "%d", int(42)... =
    {
        int t0;
        1 == sscanf("42", "%d", &t0) && t0 == 42;
    }
*/
bool tests_ok() {
    std::cout << "Running scanf tests..." << std::endl;

    TRY_TEST("standard %d",
        1, "42", "%d", int(42));

    TRY_TEST("standard %d with preceding whitespace",
        1, "  42", "%d", int(42));

    TRY_TEST("standard %d with preceding newline",
        1, "\n42", "%d", int(42));

    TRY_TEST("%d%c without any whitespace",
        2, "64c", "%d%c", int(64), char('c'));

    TRY_TEST("%d%c with whitespace in text but not in format",
        2, "64 c", "%d%c", int(64), char(' '));

    TRY_TEST("%d%c with whitespace in format but not in text",
        2, "64c", "%d %c", int(64), char('c'));

    TRY_TEST("%d%c with whitespace in both",
        2, "64 c", "%d %c", int(64), char('c'));

    TRY_TEST("early EOF",
        -1, "", "%d", int(0));

    TRY_TEST("exact char match 1",
        1, "abc9", "abc%d", int(9));

    TRY_TEST("exact char match 0",
        0, "abd9", "abc%d", int(9));

    TRY_TEST("standard %u",
        1, "42", "%u", (unsigned int)(42));

    TRY_TEST("decimal %i",
        1, "42", "%i", int(42));

    TRY_TEST("hexadecimal %i lowercase",
        1, "0x2a", "%i", int(42));

    TRY_TEST("hexadecimal %i uppercase",
        1, "0X2A", "%i", int(42));

    TRY_TEST("octal %i",
        1, "052", "%i", int(42));

    TRY_TEST("hexadecimal %x lowercase",
        1, "2a", "%x", int(42));

    TRY_TEST("hexadecimal %x uppercase",
        1, "2A", "%x", int(42));

    TRY_TEST("hexadecimal %x 0x lowercase",
        1, "0x2a", "%x", int(42));

    TRY_TEST("hexadecimal %x 0x uppercase",
        1, "0X2A", "%x", int(42));

    TRY_TEST("octal %o",
        1, "52", "%o", int(42));

    TRY_TEST("hexadecimal edge g",
        2, "0x2ag", "%x%c", int(42), char('g'));

    TRY_TEST("%d with no valid chars 1",
        0, "e", "%d", int(0));

    TRY_TEST("%d with no valid chars 2",
        0, "abc", "%d", int(0));

    TRY_TEST("%d max width",
        2, "1234", "%2d%2d", int(12), int(34));

    TRY_TEST("%*d",
        1, "1 2", "%*d%d", int(2));

    TRY_TEST("%d with no valid chars other than sign",
        0, "+e", "%d", int(0));

    TRY_TEST("%d with no valid chars followed by matching",
        0, "+e", "%d%c", int(0), char(0));

    TRY_TEST("%d zero",
        1, "0", "%d", int(0));

    TRY_TEST("%i zero",
        1, "0", "%i", int(0));

    TRY_TEST("%x zero",
        1, "0", "%x", int(0));

    TRY_TEST("%i 0x with garbage",
        1, "0xg", "%i", int(0));

    TRY_TEST("%x 0x with garbage",
        1, "0xg", "%x", int(0));

    TRY_TEST("%i failure mode",
        2, "09", "%i%d", int(0), int(9));

    TRY_TEST("%c",
        3, "abc", "%c%c%c", char('a'), char('b'), char('c'));

    TRY_TEST("%2c",
        1, "abc", "%2c", buf_char<2>("ab"));

    TRY_TEST("%2c%c",
        2, "abc", "%2c%c", buf_char<2>("ab"), char('c'));

    TRY_TEST("%*c",
        1, "ab", "%*c%c", char('b'));

    TRY_TEST("%3c",
        1, "ab\ncd", "%3c", buf_char<3>("ab\n"));

    TRY_TEST("%s",
        1, "abc", "%s", buf_str<20>("abc"));

    TRY_TEST("%s preceding whitespace",
        1, "  abc", "%s", buf_str<20>("abc"));

    TRY_TEST("%s preceding whitespace 2",
        2, "  abc  def", "%s%s", buf_str<20>("abc"), buf_str<20>("def"));

    TRY_TEST("%s preceding whitespace 3",
        2, "abc  def", "%s%s", buf_str<20>("abc"), buf_str<20>("def"));

    TRY_TEST("%5s",
        2, "abcdef   ghi", "%5s%s", buf_str<20>("abcde"), buf_str<20>("f"));

    TRY_TEST("scanset found",
        1, "abc", "%[abc]", buf_str<20>("abc"));

    TRY_TEST("scanset found rev",
        1, "abc", "%[bca]", buf_str<20>("abc"));

    TRY_TEST("scanset found with two sets",
        2, "abcde", "%[abc]%[de]",
            buf_str<20>("abc"), buf_str<20>("de"));

    TRY_TEST("inverted scansets",
        1, "bbba", "%[^a]", buf_str<20>("bbb"));

    TRY_TEST("scansets combined",
        3, "abbadededeabba", "%[ab]%[^ab]%[ab]",
            buf_str<20>("abba"), buf_str<20>("dedede"),
            buf_str<20>("abba"));

    TRY_TEST("scansets starting with hyphen",
        1, "-a0", "%[-a]", buf_str<20>("-a"));

    TRY_TEST("scansets ending with hyphen",
        1, "-a0", "%[a-]", buf_str<20>("-a"));

    TRY_TEST("inverted scansets starting with hyphen",
        1, "2b-a0", "%[^-a]", buf_str<20>("2b"));

    TRY_TEST("inverted scansets ending with hyphen",
        1, "2b-a0", "%[^a-]", buf_str<20>("2b"));

    TRY_TEST("scanset range",
        1, "01239a", "%[0-9]a", buf_str<20>("01239"));

    TRY_TEST("scanset range one char",
        1, "555a", "%[5-5]a", buf_str<20>("555"));

    TRY_TEST("inverted scanset range",
        1, "abc0def", "%[^0-9]", buf_str<20>("abc"));

    TRY_TEST("scanset maxlen",
        2, "39abc", "%2[0-9]%2[a-b]", buf_str<20>("39"), buf_str<20>("ab"));

    TRY_TEST("%f",
        1, "2.5", "%f", float(2.5f));

    TRY_TEST("%e",
        1, "2.5", "%e", float(2.5f));

    TRY_TEST("%g",
        1, "2.5", "%g", float(2.5f));

    TRY_TEST("%a",
        1, "2.5", "%a", float(2.5f));

    TRY_TEST("%f multidots 1",
        1, "2.5.2", "%f", float(2.5f));

    TRY_TEST("%f multidots 2",
        1, "2..2", "%f", float(2.0f));

    TRY_TEST("%f startdot",
        0, ".2", "%f", float(0.0f));

    TRY_TEST("%f plus",
        1, "+2.5", "%f", float(2.5f));

    TRY_TEST("%f minus",
        1, "-2.5", "%f", float(-2.5f));

    TRY_TEST("%f enddot",
        2, "-2.a", "%f%c", float(-2.0f), char('a'));

    TRY_TEST("%f pi",
        1, "3.14159", "%f", float(3.14159f));

    TRY_TEST("%f exp+",
        1, "100.5e+3", "%f", float(100500.0f));

    TRY_TEST("%f exp-",
        1, "12.5e-2", "%f", float(0.125f));

    TRY_TEST("%f hex",
        1, "0x0.3p10", "%f", float(192.0f));

    TRY_TEST("%p",
        1, "0x01ff", "%p", (void *)0x01ff);

    TRY_TEST("literal",
        1, "abc3", "abc%d", int(3));

    TRY_TEST("literal with whitespace on input only",
        0, " abc3", "abc%d", int(3));

    TRY_TEST("literal with whitespace on both",
        1, " abc3", " abc%d", int(3));

    TRY_TEST("%n 0",
        1, "a", "%n%c", int(0), char('a'));

    TRY_TEST("%n 1",
        2, "ab", "%c%n%c", char('a'), int(1), char('b'));

    TRY_TEST("%n after %s",
        2, "abc d", "%s%n%c", buf_str<3>("abc"), int(3), char('d'));

    TRY_TEST("%n after %s and whitespace",
        2, "  abcd", "%3s%n%c", buf_str<3>("abc"), int(5), char('d'));

    TRY_TEST("%n after %s with trailing whitespace",
        4, "  abc      2bb", "%s%n%d%n%c%c",
            buf_str<3>("abc"), int(5), int(2), int(12), char('b'), char('b'));

    TRY_TEST("%n after %f",
        2, "-75e-2q", "%f%n%c", float(-0.75f), int(6), char('q'));

    TRY_TEST("hours:minutes:seconds",
        3, "02:50:09", "%d:%d:%d", int(2), int(50), int(9));

    TRY_TEST("C99 example 1",
        3, "25 54.32E-1 thompson", "%d%f%s",
            int(25), float(5.432F), buf_str<50>("thompson"));

    TRY_TEST("C99 example 2",
        3, "56789 0123 56a72", "%2d%f%*d %[0123456789]",
            int(56), float(789), buf_str<50>("56"));

    TRY_TEST("C99 example 3",
        2, "-12.8degrees Celsius", "%f%20s of %20s",
            float(-12.8), buf_str<21>("degrees"), buf_str<21>());

    TRY_TEST("C99 example 4",
        0, "lots of luck", "%f%20s of %20s",
            float(0), buf_str<21>(), buf_str<21>());

    TRY_TEST("C99 example 5",
        3, "10.0LBS\t of \t fertilizer", "%f%20s of %20s",
            float(10.0), buf_str<21>("LBS"),
                         buf_str<21>("fertilizer"));

    TRY_TEST("C99 example 6",
        0, "100ergs of energy", "%f%20s of %20s",
            float(0), buf_str<21>(), buf_str<21>());

    TRY_TEST("whitespace test 1",
        3, "abc de 123", "%3c%3c%d",
            buf_char<3>("abc"), buf_char<3>(" de"), int(123));

    TRY_TEST("whitespace test 2",
        2, "a b", "%c\n\n%c", char('a'), char('b'));

    /* finally */
    const char *s = "abc", *sp = s;
    char buf[4];
    if (0 != test::spscanf_(&sp, "%d"))
        std::terminate();
    if (1 != test::spscanf_(&sp, "%4s", buf))
        std::terminate();
    if (0 != std::strcmp(buf, s)) {
        std::cout << "unget properly! " << buf << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    return tests_ok() ? EXIT_SUCCESS : EXIT_FAILURE;
}
