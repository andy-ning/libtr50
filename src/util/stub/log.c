/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 ILS Technology, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <tr50/util/log.h>
#include <tr50/util/memory.h>
#include <tr50/util/platform.h>

int g_log_filter_level = LOG_TYPE_IMPORTANT_INFO;

void log_filter_maximum_log_level(int level) {
	g_log_filter_level = level;
}

void _log_this_internal(int type, const char *file, int line, const char *msg, va_list args) {
#if defined _WIN32
	const char *short_file = strrchr(file, '\\');
#else
	const char *short_file = strrchr(file, '/');
#endif
	if (type > g_log_filter_level) {
		return;
	}

	printf("\n%d -%d- [%s(%d)] ", ((int)time(NULL)) % 86400, type, short_file ? short_file : file, line);
	vprintf(msg, args);
}

void _log_this(int type, const char *file, int line, const char *msg, ...) {
	va_list args;

	va_start(args, msg);
	_log_this_internal(type, file, line, msg, args);
	va_end(args);
}

void log_recurring(int type, const char *filename, int line, int minutes_between_logs, int check_for_changes, const char *msg, ...) {
	va_list args;

	va_start(args, msg);
	_log_this_internal(type, file, line, msg, args);
	va_end(args);
}

char *_log_hexdump(const void *data, int len) {
	unsigned char *buf = (unsigned char *)data;
	int indx;
	int ix2;
	char *buffer;
	int buffer_len;
	int buffer_size;

	if (data == NULL) {
		return NULL;
	}

	buffer_size = sizeof(char) * (len / 16) * 80 + 160;
	if ((buffer = _memory_malloc(buffer_size)) == NULL) {
		return NULL;
	}
	buffer_len = 0;

	buffer_len += snprintf(buffer, buffer_size, "HEXDUMP AT [0x%p] (%d bytes)\n", data, len);

	for (indx = 0; indx < len; ++indx) {
		if ((indx % 16) == 0) {
			buffer_len += snprintf(buffer + buffer_len, buffer_size - buffer_len, "  %4.4x:", indx);
		}
		buffer_len += snprintf(buffer + buffer_len, buffer_size - buffer_len, " %2.2x", buf[indx]);
		if ((indx % 16) == 7) {
			buffer[buffer_len++] = ' ';
			buffer[buffer_len++] = '-';
		} else if ((indx % 16) == 15) {
			buffer[buffer_len++] = ' ';
			buffer[buffer_len++] = ' ';
			for (ix2 = indx & -16; ix2 <= indx; ++ix2) {
				buffer[buffer_len++] = isprint(buf[ix2]) ? buf[ix2] : '.';
			}
			buffer[buffer_len++] = '\n';
		}
	}

	if ((indx % 16) != 0) {
		buffer_len += snprintf(buffer + buffer_len, buffer_size - buffer_len, "%*s  ", ((16 - (indx % 16)) * 3) + ((1 - ((indx % 16) / 8)) * 2), "");
		for (ix2 = indx & -16; ix2 < indx; ++ix2) {
			buffer[buffer_len++] = isprint(buf[ix2]) ? buf[ix2] : '.';
		}
		buffer[buffer_len] = '\0';
	} else {
		buffer[buffer_len - 1] = '\0';
	}
	return buffer;
}
