#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "util.h"

/*
 * send a stream of notifications for every new mail of a Maildir tree:
 * one file per output line
 */

/* maximum number of headrs read from the mail */
#define HEADERS_MAX	64

static int
cmp(void const *v1, void const *v2)
{
	return strcasecmp(*(char **)v1, *(char **)v2);
}

static int
match(void const *v1, void const *v2)
{
	return strncasecmp(v1, *(char **)v2, strlen(v1));
}

void
print_header(char **headers, size_t sz, char *name)
{
	char **hp;

	if ((hp = bsearch(name, headers, sz, sizeof *headers, match)) == NULL)
		return;

	for (char *h = *hp;;) {
		for (; *h != '\r' && *h != '\n'; h++)
			putchar(*h);
		h += (*h == '\r');
		h += (*h == '\n');
		if (*h != ' ' && *h != '\t')
			break;

		putchar(' ');
		h += strcspn(h, " \t");
	}
	putchar('\n');
}

static char *
next_line(char *s)
{
	s += strcspn(s, "\n");
	s += (*s == '\n');
	return s;
}

static size_t
read_mail_headers(char **headers, size_t sz, char *s)
{
	size_t i;

	for (i = 0; i < sz && strchr("\r\n", *s) == NULL; s = next_line(s))
		if (strchr(" \t", *s) == NULL)
			headers[i++] = s;
	qsort(headers, i, sizeof *headers, cmp);
	return i;
}

static void
print_mail_info(char *mail)
{
	struct stat st;
	char *buf, *headers[HEADERS_MAX];
	size_t i;
	int fd;

	if (stat(mail, &st) == -1)
		err(1, "cannot stat %s", mail);
	if ((fd = open(mail, O_RDONLY)) == -1)
		warn("cannot open %s", mail);
	if ((buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == NULL)
		err(1, "cannot read %s", mail);

	i = read_mail_headers(headers, HEADERS_MAX, buf);

	print_header(headers, i, "From:");
	print_header(headers, i, "Subject:");

	munmap(buf, st.st_size);
	close(fd);
}

static void
process_mail(char *mail)
{
	char dest[PATH_MAX];
	char *cp;

	xstrlcpy(dest, mail, sizeof dest);
	cp = strstr(dest, "/new/");
	assert(cp != NULL);
	memcpy(cp, "/cur/", 5);

	xrename(mail, dest);
	printf("%s\n", dest);
	print_mail_info(dest);
	printf("\n");
	fflush(stdout);
}

static void
check_new_mail(char *dir)
{
	DIR *dp;
	char mail[PATH_MAX];
	struct dirent *de;

	dp = xopendir(dir);
	while ((de = readdir(dp)) != NULL) {
		if (de->d_name[0] == '.')
			continue;
		xstrlcpy(mail, dir, sizeof mail);
		xstrlcat(mail, "/", sizeof mail);
		xstrlcat(mail, de->d_name, sizeof mail);
		process_mail(mail);
	}
	closedir(dp);
}

static int
need_to_watch(struct dirent *de)
{
	return de->d_type == DT_DIR &&
	    strcmp(de->d_name, "new") == 0;
}

static int
can_recurse(struct dirent *de)
{
	return de->d_type == DT_DIR &&
	    strcmp(de->d_name, "cur") != 0 &&
	    strcmp(de->d_name, "new") != 0 &&
	    strcmp(de->d_name, "tmp") != 0;
}

static void
scan_maildir(char *dir, size_t sz)
{
	DIR *dp;
	struct dirent *de;

	dp = xopendir(dir);
	while ((de = readdir(dp)) != NULL) {
		if (de->d_name[0] == '.')
			continue;
		xstrlcat(dir, "/", sz);
		xstrlcat(dir, de->d_name, sz);
		if (need_to_watch(de))
			check_new_mail(dir);
		if (can_recurse(de))
			scan_maildir(dir, sz);
		*strrchr(dir, '/') = '\0';
	}
	closedir(dp);
}

static void
mailtubes(char *base)
{
	char dir[PATH_MAX] = "";

	xstrlcpy(dir, base, sizeof dir);
	scan_maildir(dir, sizeof dir);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-v] [-s freq] dir\n", arg0);
	exit(1);
}

int
main(int argc, char **argv)
{
	char	*flag_dir = ".";
	uint8_t	 flag_sec = 1;

	arg0 = *argv;
	for (int c; (c = getopt(argc, argv, "s:v")) != -1;) {
	        switch (c) {
		case 's':
			flag_sec = strint(&optarg, UINT8_MAX, 10);
			if (optarg == NULL)
				err(1, "flag -s: number too large");
			break;
		case 'v':
			printf("%s v"VERSION"\n", arg0);
			exit(0);
			break;
	        default:
	                usage();
	        }
	}
	argv += optind;
	argc -= optind;

	if ((flag_dir = *argv++) == NULL)
		usage();
	if (*argv != NULL)
		usage();

	for (;; sleep(flag_sec))
		mailtubes(flag_dir);

	return 0;
}
