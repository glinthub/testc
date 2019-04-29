#ifndef FILEX_H
#define FILEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dirent.h>
#include <errno.h>

#define EXIT_ON_ERROR(x) do { \
    if (!(x)) { \
    printf ("ERR: %s: %d (%s)\n", __FILE__, __LINE__, strerror(errno)); \
    exit(1); \
    } \
    } while (0)

#define OP_START    1
#define OP_XFER     2
#define OP_STOP     3

#define SERVER_PORT 8888
#define BLOCK_SIZE  16383

#pragma pack(1)
typedef struct {
    uint16_t    len;    //including itself
    uint8_t     op;
} packet_gen;

typedef struct {
    packet_gen  gen;
    uint16_t    blockLen;
    uint16_t    filenameLen;
    uint8_t     filename[0];
} packet_start;

typedef struct {
    packet_gen  gen;
    uint16_t    blockSeq;
    uint8_t     block[0];
} packet_xfer;

typedef struct {
    packet_gen  gen;
} packet_stop;
#pragma pack()

#endif  //FILEX_H
