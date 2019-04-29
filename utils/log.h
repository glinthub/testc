#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

typedef enum {
    XL4_LOG_DEBUG,
    XL4_LOG_PKT,
    XL4_LOG_INFO,
    XL4_LOG_WARNING,
    XL4_LOG_ERROR,
    XL4_LOG_CRITICAL,
    XL4_LOG_DISABLED
} xl4_log_level_t;

int xl4_log_get_id(char *module);
void xl4_log_set_level(int logId, int logLevel);
void xl4_log_parse(char *string);
void xl4_log(int logId, xl4_log_level_t level, char *fmt, ...);
void xl4_log_pkt(int logId, unsigned char * buf_ptr, uint32_t len, uint8_t send);

#define HERE printf(">>>>>>>> HERE: %s: %d\n", __FUNCTION__, __LINE__)

#define XL4_TO_DO() printf("!!!! %s not implement yet !!!!\n", __FUNCTION__);
#define XL4_TO_DEL() printf("!!!! %s To be deleted !!!!\n", __FUNCTION__)
#endif  //UTILS_LOG_H
