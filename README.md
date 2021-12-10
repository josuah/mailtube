mailtube
========
While YouTube gives you an uninterrupted flow of distraction,
mailltube gives you a continuous stream of focus.

The [xnotify](https://github.com/phillbush/xnotify) project
gives a sane approach to desktop notifications using a FIFO
for communication, and lets the integration up to the user,
as easy as `echo "message" >$XNOTIFY_FIFO`.

The mailtube suite generates xnotify notifications from a remote
Maildir on a server, through SSH for instance, or from a local
Maildir directly.

The `mailtube-server` binary needs to be present on the same machine
where the Maildirs are, and an `mailtube-xnotify` binary present on
the local machine with xnotify running, and with $XNOTIFY_FIFO set to
the path to xnotify(1)'s FIFO.

For instance, something similar to this can be set on the .xinitrc:

```
# start xnotify with a FIFO
export XNOTIFY_FIFO="$HOME/.cache/xnotify.fifo"
rm -f "$XNOTIFY_FIFO"
mkfifo "$XNOTIFY_FIFO"
xnotify <>$XNOTIFY_FIFO &

# run the mail notification daemon
mailtube-xnotify ssh user@example.com mailtube-server Maildir &
```

Straight after this, each new mail received on the server will
generate a local desktop notification.

If mailtube is killed or the local computer powered off, the
notification will not be lost, and when running mailtube again,
the pending notifications will be displayed again.

All thanks to Maildir having separate`new` and `cur` directory.

See the mailtube(1) manpage in this repo for more.
