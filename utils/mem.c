#ifdef malloc
#undef malloc
#endif

#ifdef free
#undef free
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void mem_dump(unsigned char * buf_ptr, int len, char *desc) {
    printf("%s: buf %p, %d bytes\n", desc, buf_ptr, len);
    if (len > 64)
        len = 64;
    unsigned char* cp1 = (unsigned char*)((long)(buf_ptr) / 16 * 16);
    unsigned char* cp2 = (unsigned char*)((long)(buf_ptr + len - 1) / 16 * 16 + 15);
    unsigned char* cp;
    int rows = (cp2 - cp1 + 1) / 16;
    int i, j;
    for(i = 0; i < rows; i++) {
        printf("%p\t", (cp1 + 16*i));
        for(j = 0; j < 16; j++) {
            cp = cp1 + 16*i + j;
            if(cp < buf_ptr || cp >= buf_ptr + len)
                printf("  ");
            else
                printf("%02x", (unsigned char)*cp);

            if(!((j+1)%4))
                printf(" ");
        }
        printf("\t");
        for(j = 0; j < 16; j++) {
            cp = cp1 + 16*i + j;
            if(cp < buf_ptr || cp > buf_ptr + len - 1)
                printf(" ");
            else if(isgraph(*cp))
                printf("%1c", *cp);
            else
            {
                if(j < 15 && *cp > 0x7f && *(cp+1) > 0x7f)
                {
                    char tmp[3] = {0, 0, 0};
                    tmp[0] = *cp;
                    tmp[1] = *(cp+1);
                    printf("%.2s", tmp);
                    j++;
                }
                else
                    printf(".");
            }
        }
        printf("\n");
    }
}


/**
 * @brief Together with the macro UTIL_MALLOC, UTIL_FREE,
 * and the function util_free,
 * to detect the memory leak.
 * @param sz
 * @return
 */
void *util_malloc(unsigned long sz)
{
    return malloc(sz);
}

/**
 * @brief Together with the macro UTIL_MALLOC, UTIL_FREE,
 * and the function util_malloc,
 * to detect the memory leak.
 * @param ptr
 */
void util_free(void *ptr)
{
    free(ptr);
}
