#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "log.h"

typedef struct {
#define LOG_NAME_MAX_LEN 8
    char    name[LOG_NAME_MAX_LEN+1];
    int     level;
} LogUser_t;

#define LOG_USER_MAX_NUM 16
LogUser_t logUsers[LOG_USER_MAX_NUM+1] = {{0}};

#define LOG_TEXT_MAX_LEN 128

int xl4_log_get_id(char *module)
{
    int i = 0;
    int freeIdx = -1;
    for (i=1; i<LOG_USER_MAX_NUM; i++)
    {
        if (logUsers[i].name[0] == '\0')
        {
            freeIdx = i;
            continue;
        }
        if (strncmp(logUsers[i].name, module, LOG_NAME_MAX_LEN) == 0)
            return i;
    }

    if (freeIdx != -1)
    {
        strncpy(logUsers[freeIdx].name, module, LOG_NAME_MAX_LEN);
        logUsers[freeIdx].level = XL4_LOG_INFO;
    }
    else
    {
        printf("No space for log module %s\n", module);
    }

    return freeIdx;
}

void xl4_log_set_level(int id, int logLevel)
{
    if (id > 0 && id <= LOG_USER_MAX_NUM)
        logUsers[id].level = logLevel;
}

void xl4_log_parse(char *string)
{
    char *nameAndLevel = strtok(string, ",");
    while (nameAndLevel)
    {
        char *delimPos = strchr(nameAndLevel, ':');
        *delimPos = '\0';
        char *name = nameAndLevel;
        int level = atoi(delimPos+1);
        int id = xl4_log_get_id(name);
        xl4_log_set_level(id, level);
        nameAndLevel = strtok(NULL, ",");
    }
}

char *mkStrWhere(const char *file, const int line, const char *func) {
    static char str[128];
    snprintf(str, sizeof(str) - 1, "%s:%d:%s(): ", file, line, func);
    return str;
}

char *log_level[] = {
    "DEBUG",
    "PKT",
    "INFO",
    "WARNING",
    "ERROR",
    "CRITICAL"
};

void xl4_log(int userIndex, xl4_log_level_t level, char *fmt, ...)
{
    if (userIndex == 0)
        return;

    if (level < logUsers[userIndex].level)
        return;

    char buf[LOG_TEXT_MAX_LEN+2] = {0};
    int len = 0;
    va_list ap;
    va_start(ap, fmt);

    time_t cur = time(NULL);
    time(&cur);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *curtm = localtime(&cur);

    len += snprintf(buf, LOG_TEXT_MAX_LEN,
                    "%d-%02d-%02d %02d:%02d:%02d.%06d ",
                    curtm->tm_year+1900, curtm->tm_mon+1, curtm->tm_mday,
                    curtm->tm_hour, curtm->tm_min, curtm->tm_sec,
                    tv.tv_usec);

    len += snprintf(buf + len, (size_t)(LOG_TEXT_MAX_LEN-len),
                    "%s [%s] ",
                    log_level[level], logUsers[userIndex].name);
    len += vsnprintf(buf + len, (size_t)(LOG_TEXT_MAX_LEN-len),
                     fmt,
                     ap);
    va_end(ap);

    if (len > LOG_TEXT_MAX_LEN)
        strcpy(buf+LOG_TEXT_MAX_LEN-12, "(truncated)\n");

    printf("%s", buf);
}

void xl4_log_pkt(int userId, unsigned char * buf_ptr, uint32_t length, uint8_t send)
{
    if (userId == 0)
        return;

    if (XL4_LOG_PKT < logUsers[userId].level)
        return;

    char buf[LOG_TEXT_MAX_LEN + 4];
    int len = 0;
    int i = 0;

    time_t cur = time(NULL);
    time(&cur);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *curtm = localtime(&cur);

    len += snprintf(buf, LOG_TEXT_MAX_LEN,
                    "%d-%02d-%02d %02d:%02d:%02d.%06d ",
                    curtm->tm_year+1900, curtm->tm_mon+1, curtm->tm_mday,
                    curtm->tm_hour, curtm->tm_min, curtm->tm_sec,
                    tv.tv_usec);

    len += snprintf(buf + len, (size_t)(LOG_TEXT_MAX_LEN-len),
                    "%s [%s] %s %d bytes:",
                    log_level[XL4_LOG_PKT], logUsers[userId].name,
                    send ? "Send" : "Recv", length);

    while (len < LOG_TEXT_MAX_LEN - 3 && i < length)
    {
        len += sprintf(buf+len, " %02x", *(buf_ptr+i));
        i++;
    }
    printf("%s\n", buf);
}

