#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
/*
#include <semaphore.h>
#include <pthread.h>
*/
#include <ctype.h>
#include <sys/types.h>

#include <time.h>
#include <locale.h>

#ifdef _WIN32
#include <winsock2.h>
#include <wininet.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

#define uint8	unsigned char
#define uint16	unsigned short
#define uint32	unsigned int

#define HERE printf("%d\n", __LINE__)

void str_pre_process(char *buf)
{
	char *cp = buf + strlen(buf) - 1;
	
	while (*cp == '\n' || *cp == '\r')
		cp--;

	while (*cp == '\t' || *cp == ' ')
		cp--;

	*(cp + 1) = '\0';
	return;
}

int liba_main(int argc, char** argv)
{
	char buf[1024];
	int i,j,m,n;
	FILE* fp = fopen("liba.txt", "r");
	if (!fp)
	{
		printf("file(\"liba.txt\") open error\n");
		return -1;
	}
	i = j = m = n = 0;                    

	while(fgets(buf, 1023, fp))
	{
		n++;
		str_pre_process(buf);
		if (!strchr(buf, '\t'))
		{
			m++;
			printf("\n%-20s *********************/\n", buf);
		}
		else if (strstr(buf, "“À…Ω"))
		{
			i++;
			printf("   %s\n", buf);
		}
		else if (strstr(buf, "∫Ï–«") && strstr(buf, "’Ê±±"))
		{
			j++;
			printf("   !! %s\n", buf);
		}
		else
		{
			continue;
		}
	}
	printf("%d ysl, %d mkl, %d types fetched in %d lines.\n", i, j, m, n);
	fclose(fp);

	return 0;
}
 
