#include lib/Makefile
include mk.defines

PROGS = client server

all: ${PROGS}

server:
	${CC} server.c ${CFLAGS} ${LDFLAGS} -o server
	
client:
	${CC} client.c ${CFLAGS} ${LDFLAGS} -o client

clean:
	rm -f ${PROGS} ${OBJS}
	
