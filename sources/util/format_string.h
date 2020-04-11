#pragma once
#include <string>
#include <cstdarg>

std::string format_string(const char *fmt, ...);
std::string formatv_string(const char *fmt, va_list ap);
