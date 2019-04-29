#ifndef UTILS_MEM_H
#define UTILS_MEM_H

void mem_dump(unsigned char * buf_ptr, int len, char *desc);
void pkt_dump(unsigned char * buf_ptr, int len, char *desc);

void *util_malloc(unsigned long sz);
void util_free(void *ptr);

#define UTIL_MALLOC(sz) \
    ({void *ptr=util_malloc(sz);\
    printf("%s line %d: malloc %d bytes at %p\n", __FILE__, __LINE__, sz, ptr);\
    ptr;\
    })

#define UTIL_FREE(ptr) \
    do {printf("%s line %d: free %p\n", __FILE__, __LINE__, ptr);util_free(ptr);} while (0)

#endif  //UTILS_MEM_H
