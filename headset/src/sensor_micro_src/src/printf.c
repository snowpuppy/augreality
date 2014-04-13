/*
 * Cornell Cup 2014 - Augmented Reality Simulator
 *
 * Stephen Carlson
 * Steven Ellis
 * Thor Smith
 * Dr. Mark C. Johnson
 *
 * Optimized printf/sprintf/snprintf for ARM platforms (works on x86 too)
 * Fully reentrant and ISR safe, no dynamic memory allocation on the heap
 * The exact semantics of some format strings differ from the industry standard
 * printf() family to speed up operation. See the API documentation for details
 */

#include <printf.h>
#include <stdarg.h>
#include <limits.h>

#ifdef PRINTF_FLOAT
#include <math.h>
#endif

// Lots of data goes into the padding flags
// Left-align
#define PAD_RIGHT 1
// Zero pad
#define PAD_ZERO 2
// Display as signed (not applicable for double)
#define PAD_SIGNED 4
// Always display leading sign
#define PAD_LEADING_PLUS 8
// Number is negative (internal for printi)
#define PAD_NEGATIVE 16
// After decimal point (internal for format)
#define PAD_AFTER_DECIMAL 32

#define IS_DIGIT(x) ((x) >= '0' && (x) <= '9')
#define PRINT_BUF_LEN 12

// Data for snprintf() and sprintf()
typedef struct {
	char *buffer;
	uint32_t count;
} _sPrintfData;

// Data for fprintf()
typedef struct {
	FILE *stream;
	uint32_t count;
} _fPrintfData;

#ifdef PRINTF_FLOAT
// Round numbers off by adding LSBs to them
static const double round_nums[9] = {
	5e-1, 5e-2, 5e-3, 5e-4, 5e-5, 5e-6, 5e-7, 5e-8
};

// Multipliers for extracting before and after decimal point
static const double multipliers[9] = {
	1e+0, 1e+1, 1e+2, 1e+3, 1e+4, 1e+5, 1e+6, 1e+7, 1e+8
};
#endif

// Prints a character to debug output
static void _printToStream(void* in, char c) {
	_fPrintfData *data = (_fPrintfData*)in;
	if (c) {
		fputc((int)c, data->stream);
		data->count++;
	}
}

// Prints a character to an output buffer
static void _printToBuffer(void* in, char c) {
	_sPrintfData *data = (_sPrintfData*)in;
	*(data->buffer)++ = c;
	if (c)
		data->count++;
}

// Prints a character to an output buffer with limit
static void _printNToBuffer(void* in, char c) {
	_sPrintfData *data = (_sPrintfData*)in;
	if (!c)
		*(data->buffer)++ = '\0';
	else if (data->count) {
		*(data->buffer)++ = c;
		data->count--;
	}
}

// Quick and easy string length method
static uint32_t _strlen(char *str) {
	uint32_t slen = 0;
	if (str != NULL)
		while (*str != 0) {
			slen++;
			str++;
		}
	return slen;
}

// Prints a string with optional zero left/right padding to at least the required width
static void prints(void (*outputFn)(void*, char), void *data, const char *string,
		uint32_t width, uint32_t pad) {
	char padchar = ' ';
	if (width > 0) {
		uint32_t len = _strlen((char*)string);
		// If length greater than field width, print all
		if (len >= width)
			width = 0;
		else
			width -= len;
		// Zero-pad?
		if (pad & PAD_ZERO)
			padchar = '0';
	}
	// If aligning to the right, pad leading
	if (!(pad & PAD_RIGHT))
		for (; width > 0; width--)
			outputFn(data, padchar);
	// Output data
	for (; *string; ++string)
		outputFn(data, *string);
	// Output trailing pad if aligning left
	for (; width > 0; --width)
		outputFn(data, padchar);
}

// Prints an integer in the specified numeric base
static void printi(void (*outputFn)(void*, char), void *data, int32_t num, uint32_t base,
		uint32_t width, uint32_t pad) {
	char print_buf[PRINT_BUF_LEN], *s;
	uint32_t remain, value = (uint32_t)num;
	if ((pad & PAD_SIGNED) && num < 0) {
		pad |= PAD_NEGATIVE;
		value = (uint32_t)(-num);
	}
	// Make sure print_buf is NULL-term
	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';
	do {
		remain = value % base;
		// Divide down to next place
		value /= base;
		// Store value
		if (remain >= 10)
			remain += ('A' - 10);
		else
			remain += '0';
		*--s = remain;
	} while (value);
	if (pad & PAD_NEGATIVE) {
		// Re-add the negative sign
		if (width && (pad & PAD_ZERO)) {
			outputFn(data, '-');
			width--;
		} else
			*--s = '-';
	} else if (pad & PAD_LEADING_PLUS) {
		if (width && (pad & PAD_ZERO)) {
			outputFn(data, '+');
			width--;
		} else
			*--s = '+';
	}
	prints(outputFn, data, s, width, pad);
}

#ifdef PRINTF_FLOAT
// Prints a floating point number with the given precision
static void printd(void (*outputFn)(void*, char), void *data, double value,
		uint32_t widthTotal, uint32_t widthAfter, uint32_t pad) {
	uint32_t ipart, fpart, widthBefore;
	const char *inf;
	// Force widthAfter down to 0..8
	if (widthAfter > 8)
		widthAfter = 8;
	// Adjust width if the constraint cannot be met
	// There must be at least one digit before and the decimal point
	if (widthTotal <= widthAfter + 2) {
		widthBefore = 0;
		widthTotal = widthAfter + 1;
	} else
		widthBefore = widthTotal - widthAfter - 1;
	if (isnan(value))
		prints(outputFn, data, "NaN", widthTotal, pad);
	else if (isinf(value)) {
		if (value > 0.)
			inf = "+Inf";
		else
			inf = "-Inf";
		prints(outputFn, data, inf, widthTotal, pad);
	} else {
		// Handle negatives
		if (value < 0.0) {
			pad |= PAD_NEGATIVE;
			value = -value;
		}
		// Left align
		if (pad & PAD_RIGHT) {
			widthTotal = widthBefore;
			widthBefore = 0;
		}
		// Round to nearest LSB as needed
		value += round_nums[widthAfter];
		// Divide into parts
		ipart = (uint32_t)value;
		value -= (double)ipart;
		fpart = (uint32_t)(value * multipliers[widthAfter]);
		// Output the front part
		printi(outputFn, data, ipart, 10, widthBefore, pad &
			(PAD_LEADING_PLUS | PAD_ZERO | PAD_NEGATIVE));
		outputFn(data, '.');
		// Use our re-entrancy
		printi(outputFn, data, fpart, 10, widthAfter, PAD_ZERO);
		if (pad & PAD_RIGHT)
			prints(outputFn, data, "", widthTotal, 0);
	}
}
#endif

// Formats text to the specified output buffer, or to the debug stream if out is NULL
static void format(void (*outputFn)(void*, char), void *data, const char *fmt, va_list args) {
	uint32_t widthBefore, widthAfter, pad;
	// Used in some cases
	const char *s;
#ifdef PRINTF_FLOAT
	double dval;
#endif
	for (; *fmt; fmt++) {
		if (*fmt == '%') {
			// Default widths
			widthBefore = pad = 0;
			widthAfter = 6;
			fmt++;
			if (*fmt == '\0')
				break;
			if (*fmt == '%')
				// %%
				outputFn(data, *fmt);
			else {
				// Left-align
				if (*fmt == '-') {
					fmt++;
					pad = PAD_RIGHT;
				}
				// Always add sign
				if (*fmt == '+') {
					fmt++;
					pad |= PAD_LEADING_PLUS;
				}
				// Zero pad
				if (*fmt == '0') {
					fmt++;
					pad |= PAD_ZERO;
				}
				// Determine number of digits before and after decimal point
				while (*fmt == '.' || IS_DIGIT(*fmt))
					if (*fmt == '.') {
						pad |= PAD_AFTER_DECIMAL;
						widthAfter = 0;
						fmt++;
					} else {
						// Compile widths before and after decimal point
						if (pad & PAD_AFTER_DECIMAL)
							widthAfter = (widthAfter * 10) + (*fmt - '0');
						else
							widthBefore = (10 * widthBefore) + (*fmt - '0');
						fmt++;
					}
				// Long-double and long-decimal is implied on a 32-bit platform
				if (*fmt == 'l')
					fmt++;
				switch (*fmt) {
				case 's':
					// String
					s = (const char*)va_arg(args, char*);
					prints(outputFn, data, s ? s : "NULL", widthBefore, pad);
					break;
				case 'd':
					// Signed decimal
					printi(outputFn, data, va_arg(args, int), 10, widthBefore, pad | PAD_SIGNED);
					break;
				case 'u':
					// Unsigned decimal
					printi(outputFn, data, va_arg(args, unsigned int), 10, widthBefore, pad);
					break;
				case 'x':
				case 'X':
					// Hexadecimal (unfortunately, always uppercase)
					printi(outputFn, data, va_arg(args, unsigned int), 16, widthBefore, pad);
					break;
				case 'p':
					// Pointer address
					printi(outputFn, data, va_arg(args, unsigned int), 16, 8, pad | PAD_ZERO);
					break;
				case 'c':
					// Character
					outputFn(data, (char)va_arg(args, int));
					break;
#ifdef PRINTF_FLOAT
				case 'f':
					// Floating point double
					dval = va_arg(args, double);
					printd(outputFn, data, dval, widthBefore, widthAfter, pad);
					break;
#else
				case 'f':
					// Floating point ignore to avoid breaking %f format strings and faulting
					(void)(va_arg(args, uint64_t));
					break;
#endif
				case '\0':
					// End of string
					break;
				default:
					// Invalid, print character
					outputFn(data, '%');
					outputFn(data, *fmt);
					break;
				}
			}
		} else
			outputFn(data, *fmt);
	}
	// Zero-terminate
	outputFn(data, '\0');
}

// The next few functions do exactly as one might believe they should as declared in <stdio.h>

void fprint(const char *str, FILE *stream) {
	char c;
	while ((c = *str++))
		fputc(c, stream);
}

void print(const char *str) {
	fprint(str, stdout);
}

int fputs(const char *str, FILE *stream) {
	char c;
	uint32_t count = 0;
	while ((c = *str++)) {
		fputc((int)c, stream);
		count++;
	}
	fputc('\n', stream);
	// Required for library compatibility
	return (int)count;
}

int puts(const char *str) {
	return fputs(str, stdout);
}

int vfprintf(FILE *stream, const char *fmt, va_list args) {
	_fPrintfData data;
	data.stream = stream;
	data.count = 0;
	format(_printToStream, (void*)(&data), fmt, args);
	// Required for library compatibility
	return (int)data.count;
}

int vsprintf(char *out, const char *fmt, va_list args) {
	_sPrintfData data;
	data.buffer = out;
	data.count = 0;
	format(_printToBuffer, (void*)(&data), fmt, args);
	// Required for library compatibility
	return (int)data.count;
}

int vsnprintf(char *out, size_t size, const char *fmt, va_list args) {
	int ret = 0;
	_sPrintfData data;
	data.buffer = out;
	// Allow room for null terminator
	if (size > 0) {
		size--;
		data.count = (uint32_t)size;
		// Leave room for null terminator at end
		format(_printNToBuffer, (void*)(&data), fmt, args);
		// Required for library compatibility
		ret = (int)(size - data.count);
	}
	return ret;
}

int fprintf(FILE *stream, const char *fmt, ...) {
	va_list args;
	int count;
	va_start(args, fmt);
	count = vfprintf(stream, fmt, args);
	va_end(args);
	return count;
}

int printf(const char *fmt, ...) {
	va_list args;
	int count;
	va_start(args, fmt);
	count = vfprintf(stdout, fmt, args);
	va_end(args);
	return count;
}

int sprintf(char *out, const char *fmt, ...) {
	int count;
	va_list args;
	va_start(args, fmt);
	count = vsprintf(out, fmt, args);
	va_end(args);
	return (int)count;
}

int snprintf(char *out, size_t size, const char *fmt, ...) {
	int count;
	va_list args;
	va_start(args, fmt);
	count = vsnprintf(out, size, fmt, args);
	va_end(args);
	return count;
}
