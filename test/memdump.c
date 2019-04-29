#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>

#include <time.h>
#include <locale.h>

void mem_dump(uint8* buf_ptr, int len) {
	printf("memory of address 0x%X to 0x%X, %d bytes\n", (int)buf_ptr, (int)((char*)buf_ptr + len - 1), len);
	uint8* cp1 = (uint8*)((int)((uint8*)buf_ptr) / 16 * 16);
	uint8* cp2 = (uint8*)((int)((uint8*)buf_ptr + len - 1) / 16 * 16 + 15);
	uint8* cp;
	int rows = (cp2 - cp1 + 1) / 16;
	int i, j;
	for(i = 0; i < rows; i++) {
		printf("0x%08X\t", (int)(cp1 + 16*i));
		for(j = 0; j < 16; j++) {
			cp = cp1 + 16*i + j;
			if(cp < buf_ptr || cp >= buf_ptr + len)
				printf("  ");
			else
				printf("%02x", (uint8)*cp);

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

