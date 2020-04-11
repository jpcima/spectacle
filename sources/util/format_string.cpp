#include "format_string.h"
#include <new>
#include <memory>
#include <cstdio>

std::string format_string(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    std::string s = formatv_string(fmt, ap);
    va_end(ap);
    return s;
}

std::string formatv_string(const char *fmt, va_list ap)
{
    char *p = nullptr;
    if (vasprintf(&p, fmt, ap) == -1)
        throw std::bad_alloc();
    std::unique_ptr<char[], void (*)(void *)> pp(p, &free);
    return std::string(p);
}
