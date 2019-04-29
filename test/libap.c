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

int libap_main(int argc, char** argv)
{
	char buf[1024];
	char type[128];
	char url[128];
	char product[128];
	int i,j,m,n;
	char *cp1, *cp2;
	FILE* fp = fopen("libap.html", "r");
	if (!fp)
	{
		printf("file(\"liba.txt\") open error\n");
		return -1;
	}
	i = j = m = n = 0;                    

	while(fgets(buf, 1023, fp))
	{
		n++;
		cp1 = strstr(buf, "<td colspan=5 height=25 valign=middle>");
		if (cp1)
		{
			m++;
			cp1 += strlen("<td colspan=5 height=25 valign=middle>");
			while (*cp1 == '<' || !strncmp(cp1, "&nbsp;", strlen("&nbsp;")))
			{
				if (*cp1 == '<')
					cp1 = strchr(cp1, '>') + 1;
				else
					cp1 += strlen("&nbsp;");
			}
			cp2 = strchr(cp1, '<');
			memcpy(type, cp1, cp2-cp1);
			type[cp2-cp1] = '\0';
			//printf("\n%s\n", type);
			continue;
		}
		
		cp1 = strstr(buf, "<td><a href=\"./product_quotation_down.php");
		if (!cp1)
			continue;
		
		i++;
		cp1 += strlen("<td><a href=\"./");
		cp2 = strchr(cp1, '\"');
		memcpy(url, cp1, cp2-cp1);
		url[cp2-cp1] = '\0';
		//printf("%s", url);

		cp1 = strchr(cp2, '>') + 1;
		while (*cp1 == '<' || !strncmp(cp1, "&nbsp;", strlen("&nbsp;")))
		{
			if (*cp1 == '<')
				cp1 = strchr(cp1, '>') + 1;
			else
				cp1 += strlen("&nbsp;");
		}
		cp2 = strchr(cp1, '<');
		memcpy(product, cp1, cp2-cp1);
		product[cp2-cp1] = '\0';
		//printf(" %s\n", product);
		
		printf("<a href=http://www.liba.com/product/%s>%s-%s</a><br>\n", url, type, product);
//if (i>10) break;
	}
	printf("%d product, %d types fetched in %d lines.\n", i, m, n);
	fclose(fp);

	return 0;
}
 
