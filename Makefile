NAME = mailtube
VERSION = 0.1
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/man

D = -D_POSIX_C_SOURCE=200811L -D_BSD_SOURCE -DVERSION='"${VERSION}"'
CFLAGS = $D -g #-Wall -Wextra -std=c99 -pedantic
LDFLAGS = -static

OBJ = util.o
BIN = mailtube-xnotify mailtube-server
MAN1 = mailtube.1

all: ${BIN}

.c.o:
	${CC} -c ${CFLAGS} -o $@ $<

${OBJ} ${BIN:=.o}: Makefile ${HDR}

${BIN}: ${OBJ} ${BIN:=.o}
	${CC} ${LDFLAGS} -o $@ $@.o ${OBJ} ${LIB}

clean:
	rm -rf *.o ${BIN} *.tgz

install: ${BIN}
	mkdir -p ${DESTDIR}$(PREFIX)/bin
	cp -f $(BIN) ${DESTDIR}$(PREFIX)/bin
	mkdir -p ${DESTDIR}$(MANPREFIX)/man1
	cp -f ${MAN1} ${DESTDIR}$(MANPREFIX)/man1

dist:
	git archive v${VERSION} --prefix=${NAME}-${VERSION}/ \
	| gzip >${NAME}-${VERSION}.tgz
