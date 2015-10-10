#ifndef MACROS_H
#define MACROS_H
#include <stdio.h>
#include <string>

/* Standard macros that are used basically everywhere */

#define UNUSED __attribute__ ((unused))
#define RELEASE_UNUSED __attribute__ ((unused))

#ifdef DEBUG_MODE
#  define LOG(...) DEBUG_REAL(__FILE__, __LINE__, __PRETTY_FUNCTION__, "\x1b[42;30m[LOG]\x1b[39;49m", __VA_ARGS__)
#  define ERR(...) DEBUG_REAL(__FILE__, __LINE__, __PRETTY_FUNCTION__, "\x1b[41;30m[ERR]\x1b[39;49m", __VA_ARGS__)
#  define DEBUG(...) DEBUG_REAL(__FILE__, __LINE__, __PRETTY_FUNCTION__, "\x1b[43;30m[DBG]\x1b[39;49m", __VA_ARGS__)
#else
#  define LOG(...) 
#  define ERR(...) 
#  define DEBUG(...) 
#endif

void DEBUG_REAL(std::string filename, int line, std::string function_name, std::string prefix, std::string format, ...);

#endif
