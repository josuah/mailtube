#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

char *arg0;

static void
_log(char const *fmt, va_list va, int err)
{
	if (arg0 != NULL)
		fprintf(stderr, "%s: ", arg0);
	vfprintf(stderr, fmt, va);
	fprintf(stderr, err ? ": %s\n" : "\n", strerror(err));
	fflush(stderr);
}

void
err(int e, char const *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	_log( fmt, va, errno);
	exit(e);
}

void
errx(int e, char const *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	_log( fmt, va, 0);
	exit(e);
}

void
warn(char const *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	_log(fmt, va, errno);
}

void
warnx(char const *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	_log(fmt, va, 0);
}

size_t
strlcpy(char *d, char const *s, size_t sz)
{
	size_t len;

	len = strlen(s);
	if (sz == 0)
		return len;
	memcpy(d, s, (len + 1 > sz) ? sz : len + 1);
	d[sz - 1] = '\0';
	return len;
}

size_t
strlcat(char *d, char const *s, size_t dsz)
{
	size_t dlen;

	dlen = strlen(d);
	if (dlen >= dsz)
		return dlen + strlen(s);
	return dlen + strlcpy(d + dlen, s, dsz - dlen);
}

uint64_t
strint(char **sp, uint64_t max, uint8_t b)
{
	uint64_t n;
	char *s, *p, *digits = "0123456789ABCDEF";
	char c;

	n = 0;
	for (s = *sp ;; s++) {
		c = toupper(*s);
		p = strchr(digits, c);
		if (p == NULL || p >= digits + b)
			break;

		if (n > max / b)
			goto err;
		n *= b;

		if (n > max - (p - digits))
			goto err;
		n += p - digits;
	}
	if (*sp == s)
		goto err;
	*sp = s;
	return n;
err:
	*sp = NULL;
	return 0;
}
