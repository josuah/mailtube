#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#ifndef __OpenBSD__
#define pledge(...) 0
#define unveil(...) 0
#endif

extern char *arg0;

void	err(int e, char const *fmt, ...);
void	errx(int e, char const *fmt, ...);
void	warn(char const *fmt, ...);
void	warnx(char const *fmt, ...);
size_t	strlcpy(char *d, char const *s, size_t sz);
size_t	strlcat(char *d, char const *s, size_t dsz);
uint64_t strint(char **sp, uint64_t max, uint8_t b);

static inline void
strchomp(char *line)
{
	size_t len;

	len = strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[--len] = '\0';
	if (len > 0 && line[len - 1] == '\r')
		line[--len] = '\0';
}

static inline void
xstat(char *path, struct stat *st)
{
	if (stat(path, st) == -1)
		err(1, "reading %s status", path);
}

static inline void
xstrlcpy(char *dst, char const *src, size_t sz)
{
	if (strlcpy(dst, src, sz) >= sz)
		err(1, "string too long: %s", src);
}

static inline void
xstrlcat(char *dst, char *src, size_t sz)
{
	if (strlcat(dst, src, sz) >= sz)
		err(1, "string too long: %s", dst);
}

static inline void
xrename(char const *from, char const *to)
{
	if (rename(from, to) == -1)
		err(1, "cannot rename %s to %s", from, to);
}

static inline DIR *
xopendir(char const *path)
{
	DIR *dp;

	if ((dp = opendir(path)) == NULL)
		err(1, "cannot open directory %s", path);
	return dp;
}
