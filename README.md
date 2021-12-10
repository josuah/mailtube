mailtube
========
While YouTube gives you an uninterrupted flow of distraction,
mailltube gives you a continuous stream of focus.

The [xnotify](https://github.com/phillbush/xnotify) project
gives a sane approach to desktop notifications using a FIFO
for communication, and lets the integration to the user for
use with scripts.

The mailtube suite generates xnotify notifications from a remote
Maildir on a server, through SSH.

The mailtube-server binary needs to be present on the server, and
an mailtube-xnotify binary present on the local machine, with
xnotify running, and $XNOTIFY_FIFO set to the FIFO file to read.

For instance, something similar to this can be set on the .xinitrc:

```
export XNOTIFY_FIFO="$HOME/.cache/xnotify.fifo"
rm -f "$XNOTIFY_FIFO"
mkfifo "$XNOTIFY_FIFO"
xnotify <>$XNOTIFY_FIFO &

mailtube-xnotify ssh user@example.com mailtube-server Maildir &
```

Straight after this, each new mail received on the server will
generate a local desktop notification.

If mailtube is killed or the local computer powered off, the
notification will not be lost, and when running mailtube again,
the pending notifications will be displayed again.

All thanks to Maildir having separate`new` and `cur` directory.
