#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <errno.h>

#define CHECK_ERR(expect, action) do { \
    if (!(expect)) { \
        printf("Error at %s: %d (%s)\n", __FILE__, __LINE__, strerror(errno)); \
        action; \
    } \
} while (0)

#define EXIT_ON_ERR(expect) CHECK_ERR(expect, exit(1))

#define HERE printf(">>>> %s: %d\n", __FILE__, __LINE__)

#endif
