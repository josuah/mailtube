#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "util.h"
#include "mailtube.h"

char notification[1024];

/*
 * format the feed of notifications to the file $XNOTIFY_FIFO
 */

void
send_notification_path(char *mail)
{
	(void)mail;
}

void
send_notification_header(char *header)
{
	if (notification[0] != '\0')
		strlcat(notification, "\n", sizeof notification);
	strlcat(notification, header, sizeof notification);
}

void
send_notification_end(void)
{
	switch (fork()) {
	case 0:
		execlp("herbe", "herbe", notification, NULL);
		err(1, "could not execute herbe");
	case -1:
		err(1, "could not fork");
	}
	notification[0] = '\0';
}

void
init_notifications(void)
{
	notification[0] = '\0';
}
