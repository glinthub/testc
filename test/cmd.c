#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <semaphore.h>
#include <pthread.h>
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

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned int
#define uint64 unsigned long long

#define int8 char
#define int16 short
#define int32 int
#define int64 long long

void test()
{
    struct termios new_settings, stored_settings;
    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;

    /* Disable canonical mode, and set buffer size to 1 byte */
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_settings); 
}

#define LINE_MAX_LENGTH 63
int cmd1_main(int argc, char** argv)
{
	char buf[LINE_MAX_LENGTH + 1] = {0};
	char ch;
	int len = 0;
	uint8 bExit = 0;
	uint8 bExecute = 0;
	uint8 bQuery = 0;
	char* strPrompt = "S3224#";

	test();

	printf("%s", strPrompt);
	while(1)
	{
		ch = getchar();

		switch(ch)
		{
			case 0x03:	/*Ctrl+C*/
				bExit = 1;
				break;

			case 0x08:	/*Backspace*/
				if(len)
				{
					buf[--len] = '\0';
					printf("%c", ch);
					printf(" ");
					printf("%c", ch);
				}
				else
				{
					//printf("%c", 0x07);
				}
				break;

			case 0x0d:	/*Carriage return*/
				bExecute = 1;
				break;

			case 0x3f:	/* ? */
				bQuery = 1;
				break;

			default:
				if(isgraph(ch) || ch == 0x20/*white space*/)
				{
					if(len < LINE_MAX_LENGTH)
					{
						buf[len++] = ch;
						printf("%c", ch);
					}
				}
				else
				{
					//printf("%c", 0x07);
					printf("%02x", ch);
					sleep(500);
					printf("%c", 0x08);
					printf("%c", 0x08);
					printf(" ");
					printf(" ");
					printf("%c", 0x08);
					printf("%c", 0x08);
				}
				break;
			
		}

		if(bExit)
			break;

		if(bExecute)
		{
			int i = 0;

			bExecute = 0;
			printf("\n");

			while(buf[i] == 0x20)
			{
				if(++i >= len)
					break;
			}
			if(i<len)
			{
				printf("  Your command is: %s .\n", buf+i);
			}

			len=0;
			buf[0] = '\0';
			printf("%s", strPrompt);
		}

		if(bQuery)
		{
			bQuery = 0;
			printf("\n");

			buf[len]='\0';
			printf("  %.16s -- This command maybe not completed.\n", buf);
			printf("%s%s", strPrompt, buf);
		}

	}

	return 0;
}

