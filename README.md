mailtube
========
`git://git.josuah.net/mailtube` - [0.2](/tool/mailtube/mailtube-0.2.tgz)

While YouTube gives you an uninterrupted flow of distraction,
mailltube gives you a continuous stream of focus.

The [xnotify](https://github.com/phillbush/xnotify) and
[herbe](https://github.com/dudik/herbe) projects give a sane approach
to desktop notifications using a FIFO or just a command line for
communication, and lets the integration up to the user, as easy as
`echo "message" >$XNOTIFY_FIFO` or `herbe "message"` from a shell.

How it works
------------
The mailtube suite generates xnotify or herbe notifications from a
remote Maildir on a server, through SSH for instance, or from a local
Maildir directly.

The `mailtube-server` binary needs to be present on the same machine
where the Maildirs are, and an `mailtube-$client` binary present on
the local machine.

Complete example with xnotify
-----------------------------
Start xnotify with a FIFO, and then run the mail notification daemon
streaming messages to it.

```
export XNOTIFY_FIFO="$HOME/.cache/xnotify.fifo"
rm -f "$XNOTIFY_FIFO"
mkfifo "$XNOTIFY_FIFO"
xnotify <>$XNOTIFY_FIFO &

mailtube-xnotify ssh user@example.com mailtube-server Maildir &
```

Straight after this, each new mail received on the server will
generate a local desktop notification.

Complete example with herbe
------------------------------
Run the mail notification daemon and spawn herbe on each incoming
notification:

```
mailtube-herbe ssh user@example.com mailtube-server Maildir &
```

Complete example in a terminal
------------------------------
Only show the notifications on-screen in the terminal:

```
ssh user@example.com mailtube-server Maildir &
```

Notification history
--------------------
If mailtube is killed or the local computer powered off, the
notification will not be lost, and when running mailtube again,
the pending notifications will be displayed again.

All thanks to Maildir having separate `new` and `cur` directory.

If notifications are missed, their content can of course be seen
again by opening the list of email with the mail client.
