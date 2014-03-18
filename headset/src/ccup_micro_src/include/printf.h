#ifndef PRINTF_H_
#define PRINTF_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "comm.h"

// Begin C++ extern to C
#ifdef __cplusplus
extern "C" {
#endif

// FILE is an integer referring to a stream; we use (invalid) pointers for compability
typedef int FILE;
// Standard input and output streams use the PC debug terminal
#define stdout ((FILE *)SERIAL_PORT_USB)
#define stdin ((FILE *)SERIAL_PORT_USB)
// GPS
#define gps ((FILE *)SERIAL_PORT_GPS)
// Spare
#define spare ((FILE *)SERIAL_PORT_SPARE)

// fcount - Return number of characters available to read on the specified stream
int fcount(FILE *stream);
// fgetc - Reads and returns one character from the specified stream, blocking until complete
int fgetc(FILE *stream);
// getchar - Reads and returns one character from "stdin"
int getchar();
// fputc - Writes one character to the specified stream and returns the input value
int fputc(int value, FILE *stream);
// putchar - Writes one character to "stdout" and returns the input value
int putchar(int value);
// print - Prints the simple string to debug terminal
void print(const char *string);
// puts - Same as "print" function, with trailing newline
int puts(const char *string);
// fprint - Prints the simple string to the specified stream
void fprint(const char *string, FILE *stream);
// fputs - Same as "fprint" function, with trailing newline
int fputs(const char *string, FILE *stream);

// printf - Prints the formatted string to the debug stream (the PC terminal)
int printf(const char *formatString, ...);
// fprintf - Prints the formatted string to the specified output stream
int fprintf(FILE *stream, const char *formatString, ...);
// sprintf - Prints the formatted string to the string buffer, which must be big enough
int sprintf(char *buffer, const char *formatString, ...);
// snprintf - Prints the formatted string to the string buffer with the specified length limit
// The length limit, as per the C standard, includes the trailing null character
int snprintf(char *buffer, size_t limit, const char *formatString, ...);

// Variadic macro variations of the above, which are only useful in a handful of cases
// vfprintf - Prints the formatted string to the specified output stream
int vfprintf(FILE *stream, const char *formatString, va_list arguments);
// vsprintf - Prints the formatted string to a string buffer
int vsprintf(char *buffer, const char *formatString, va_list arguments);
// vsnprintf - Prints the formatted string to a string buffer with the given length limit
int vsnprintf(char *buffer, size_t limit, const char *formatString, va_list arguments);

// End C++ extern to C
#ifdef __cplusplus
}
#endif

#endif
