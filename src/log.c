#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#include "log.h"

static unsigned int current_loglevel = LOG_DEBUG;

static void __print_on_level(unsigned int loglevel, const char *format, va_list params)
{
	int fd, size, ret, off = 0;
	int __errno = errno;
	char buffer[1024];

	if (unlikely(loglevel == LOG_MSG)) {
		fd = STDOUT_FILENO;
	} else {
		if (loglevel > current_loglevel)
			return;
		fd = STDOUT_FILENO;
	}

	size = vsnprintf(buffer, sizeof(buffer), format, params);

	while (off < size) {
		ret = write(fd, buffer + off, size - off);
		if (ret <= 0)
			break;
		off += ret;
	}
	errno =  __errno;
}

void print_on_level(unsigned int loglevel, const char *format, ...)
{
	va_list params;

	va_start(params, format);
	__print_on_level(loglevel, format, params);
	va_end(params);
}
