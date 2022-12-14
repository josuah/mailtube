#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "util.h"
#include "mailtube.h"

FILE *xnotify;

/*
 * format the feed of notifications to the file $XNOTIFY_FIFO
 */

void
send_notification_path(char *mail)
{
	fprintf(xnotify, "CMD:mail %s", mail);
}

void
send_notification_header(char *header)
{
	fprintf(xnotify, "\t%s", header);
}

void
send_notification_end(void)
{
	fputc('\n', xnotify);
	fflush(xnotify);

	if (ferror(xnotify))
		err(1, "error writing to xnotify");
}

void
init_notifications(void)
{
	char *fifo;

	if ((fifo = getenv("XNOTIFY_FIFO")) == NULL)
		err(1, "missing environment variable: $XNOTIFY_FIFO");

	if ((xnotify = fopen(fifo, "w")) == NULL)
		err(1, "cannot open $XNOTIFY_FIFO (%s) for writing", fifo);
}
