#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "util.h"

/*
 * spawn the server command specified and read and format the feed
 * of notifications to the file $XNOTIFY_FIFO
 */

void
from_server_to_xnotify(FILE *server, FILE *xnotify)
{
	char *line;
	size_t sz;

	line = NULL;
	sz = 0;
	while (getline(&line, &sz, server) > 0) {
		strchomp(line);
		fprintf(xnotify, "CMD:mail %s", line);

		while (getline(&line, &sz, server) > 0) {
			strchomp(line);
			if (*line == '\0')
				break;
			fprintf(xnotify, "\t%s", line);
		}
		fputc('\n', xnotify);
		fflush(xnotify);
	}
	if (ferror(server))
		err(1, "error reading from server");
	if (ferror(xnotify))
		err(1, "error writing to xnotify");
}

pid_t
spawn_reading_pipe(char **argv, int *readfd)
{
	int pipefd[2];
	pid_t pid;

	if (pipe(pipefd) == -1)
		return -1;

	switch ((pid = fork())) {
	case 0:
		/* the child only writes, no read */
		close(pipefd[0]);

		/* overwrite stdout with the pipe */
		dup2(pipefd[1], 1);

		/* does not return */
		execvp(argv[0], argv);
		return -1;
	case -1:
		return -1;
		break;
	default:
		/* the parent only reads, no write */
		close(pipefd[1]);
	}

	*readfd = pipefd[0];
	return pid;
}

void
mailtubes(char *fifo, char **argv)
{
	FILE *server, *xnotify;
	pid_t pid;
	int fd;

	if ((pid = spawn_reading_pipe(argv, &fd)) == -1)
		err(1, "calling child: ");

	if ((xnotify = fopen(fifo, "w")) == NULL)
		err(1, "cannot open $XNOTIFY_FIFO (%s) for writing", fifo);

	if ((server = fdopen(fd, "r")) == NULL)
		err(1, "opening server pipe for reading");

	from_server_to_xnotify(server, xnotify);
}

void
usage(void)
{
	fprintf(stderr, "usage: %s [-v] servercmd [args...]\n", arg0);
	exit(1);
}

int
main(int argc, char **argv)
{
	char *flag_dir;
	char *flag_fifo;
	uint8_t flag_sec;

	arg0 = *argv;
	for (int c; (c = getopt(argc, argv, "d:s:v")) != -1;) {
	        switch (c) {
	        case 'd':
	                flag_dir = optarg;
	                break;
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

	if (*argv == NULL)
		usage();

	if ((flag_fifo = getenv("XNOTIFY_FIFO")) == NULL)
		err(1, "missing environment variable: $XNOTIFY_FIFO");

	mailtubes(flag_fifo, argv);
	return 0;
}
