#ifndef LOG_H__
#define LOG_H__

#include <string.h>
#include <errno.h>

#include "compiler.h"

#define LOG_UNSET	(-1)
#define LOG_MSG		(0) /* Print message regardless of log level */
#define LOG_ERROR	(1) /* Errors only, when we're in trouble */
#define LOG_WARN	(2) /* Warnings, dazen and confused but trying to continue */
#define LOG_INFO	(3) /* Informative, everything is fine */
#define LOG_DEBUG	(4) /* Debug only */

extern void print_on_level(unsigned int loglevel, const char *format, ...)
	__attribute__ ((__format__ (__printf__, 2, 3)));

#ifndef LOG_PREFIX
# define LOG_PREFIX
#endif

#define pr_msg(fmt, ...)						\
	print_on_level(LOG_MSG,						\
		       fmt, ##__VA_ARGS__)

#define pr_info(fmt, ...)						\
	print_on_level(LOG_INFO,					\
		       LOG_PREFIX fmt, ##__VA_ARGS__)

#define pr_err(fmt, ...)						\
	print_on_level(LOG_ERROR,					\
		       "Error (%s:%d): " LOG_PREFIX fmt,		\
		       __FILE__, __LINE__, ##__VA_ARGS__)

#define pr_err_once(fmt, ...)						\
	do {								\
		static bool __printed;					\
		if (!__printed) {					\
			pr_err(fmt, ##__VA_ARGS__);			\
			__printed = 1;					\
		}							\
	} while (0)

#define pr_warn(fmt, ...)						\
	print_on_level(LOG_WARN,					\
		       "Warn  (%s:%d): " LOG_PREFIX fmt,		\
		       __FILE__, __LINE__, ##__VA_ARGS__)

#define pr_warn_once(fmt, ...)						\
	do {								\
		static bool __printed;					\
		if (!__printed) {					\
			pr_warn(fmt, ##__VA_ARGS__);			\
			__printed = 1;					\
		}							\
	} while (0)

#define pr_debug(fmt, ...)						\
	print_on_level(LOG_DEBUG,					\
		       LOG_PREFIX fmt, ##__VA_ARGS__)

#define pr_perror(fmt, ...)						\
	pr_err(fmt ": %s\n", ##__VA_ARGS__, strerror(errno))

#endif /* LOG_H__ */
