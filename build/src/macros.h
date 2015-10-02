#ifndef MACROS_H
#define MACROS_H
#include <stdio.h>
#include <string>

/* Standard macros that are used basically everywhere */

#define UNUSED __attribute__ ((unused))

#ifdef DEBUG_MODE
#  define DEBUG(...) DEBUG_REAL(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
#else
#  define DEBUG(...)
#endif

#define LOG(...) DEBUG_REAL(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
void DEBUG_REAL(std::string filename, int line, std::string function_name, std::string format, ...);

#endif
