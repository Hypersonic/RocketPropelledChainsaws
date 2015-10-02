#include <stdarg.h>
#include <stdio.h>
#include "macros.h"

void DEBUG_REAL(std::string filename, int line, std::string function_name, std::string format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "[%s, %i] %s:\n\t", filename.c_str(), line, function_name.c_str());
    vfprintf(stderr, format.c_str(), args);
    va_end(args);
}
