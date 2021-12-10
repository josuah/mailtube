NAME = mailtube
VERSION = 0.1
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/man

CPPFLAGS = -D_POSIX_C_SOURCE=200811L -D_BSD_SOURCE -DVERSION='"${VERSION}"'
CFLAGS = -g -Wall -Wextra -std=c99 -pedantic
LDFLAGS = -static

HDR = util.h mailtube.h
OBJ = util.o
BIN = mailtube-server
CLIENTS = mailtube-xnotify mailtube-herbe
MAN1 = mailtube.1

all: ${BIN} ${CLIENTS}

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

${OBJ:.o=.c} ${BIN:=.c} ${CLIENTS:=.c}: Makefile ${HDR}

${BIN}: $@.o ${OBJ}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJ} $@.o ${LIB}

${CLIENTS}: $@.o ${OBJ} client.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJ} client.o $@.o ${LIB}

clean:
	rm -rf *.o ${BIN} *.tgz

install: ${BIN} ${CLIENTS}
	mkdir -p ${DESTDIR}$(PREFIX)/bin
	cp -f $(BIN) ${CLIENTS} ${DESTDIR}$(PREFIX)/bin
	mkdir -p ${DESTDIR}$(MANPREFIX)/man1
	cp -f ${MAN1} ${DESTDIR}$(MANPREFIX)/man1

dist:
	git archive v${VERSION} --prefix=${NAME}-${VERSION}/ \
	| gzip >${NAME}-${VERSION}.tgz
