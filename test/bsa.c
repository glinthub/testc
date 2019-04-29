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

#include "test.h"

int arg_lookup(int argc, char **argv, char *arg)
{
	int i;

	for (i = 1; i < argc; i ++)
	{
		if (!strcmp(argv[i], arg))
			return i;
	}
	return 0;
}

#define LINE_WIDTH 1024
char* map_lookup_diab(FILE * map_file, uint32 pc)
{
	static uint32 begin = 0, end = 0;
	static char buffer[LINE_WIDTH] = {0}, buf_tmp[128];
	char *cp;
	uint32 start = 0;
	uint32 size = 0;

	if (!begin && !end)
	{
		char *prefix = "		.text           00010000	";
		fseek(map_file, 0, SEEK_SET);
		while (fgets(buffer, LINE_WIDTH, map_file))
		{
			if (strncmp(buffer, prefix, strlen(prefix)))
				continue;
			sscanf(buffer + strlen(prefix), "%08x %s", &size, buf_tmp);
			if (size && !strcmp(buf_tmp, "vxWorks.tmp"))
			{
				begin = ftell(map_file);
				break;
			}
		}
		if (!begin)
			return "analyse fail: 0";
		while (fgets(buffer, LINE_WIDTH, map_file))
		{
			if (strlen(buffer) < 2 * strlen("12345678"))
			{
				end = ftell(map_file);
				break;
			}
		}
		if (!end)
			return "analyse fail: 1";
	}


	fseek(map_file, begin, SEEK_SET);
	while (fgets(buffer, LINE_WIDTH, map_file))
	{
		if (ftell(map_file) >= end)
			return "not found !-_-";
		cp = buffer;
		while(*cp == ' ' || *cp == '\t')
			cp++;
		sscanf(cp, "%s %08x\t%08x", buf_tmp, &start, &size);
		if (start <= pc && start + size > pc)
			return buf_tmp;
	}
	return "UNKNOWN !!-_-";
}

char* map_lookup_gnu(FILE * map_file, uint32 pc)
{
	uint32 start = 0, stop = 0;
	static char buffer[LINE_WIDTH] = {0};

	fseek(map_file, 0, SEEK_SET);
	while (fgets(buffer, LINE_WIDTH, map_file))
	{
		buffer[strlen(buffer)-1] = '\0';
		if (strlen(buffer) <= strlen("12345678 t "))
			continue;

		if (*(buffer + 9) != 't' && *(buffer + 9) != 'T')
			continue;

		stop = start;
		sscanf(buffer, "%08x", &start);
		if (stop > pc && start <= pc)
			return buffer + strlen("12345678 t ");
	}
	return "UNKNOWN !!-_-";
}

char* map_lookup(FILE * map_file, uint32 pc, int is_diab)
{
	if (is_diab)
		return map_lookup_diab(map_file, pc);
	else
		return map_lookup_gnu(map_file, pc);
}

int get_stack_line(char* buffer, uint32 *addr, uint32 *stack)
{
	if (strlen(buffer) < strlen("024e9de8: 024e9e00 00463c34 00000000 005f0000"))
		return -1;

	if (sscanf(buffer, "%08x: %08x %08x %08x %08x", addr, stack, stack+1, stack+2, stack+3) != 5)
		return -1;

	return 0;
}

int usage()
{
 	int i;
	uint8 copyright[] = {
		0x7a, 0x69, 0x71, 0x78, 0x7b, 0x6a, 0x74, 0x7a, 0x70, 0x6a,
		0x78, 0x4b, 0x6e, 0x71, 0x71, 0x7e, 0x7d, 0x3f, 0x75, 0x82,
		0x81, 0x43, 0x79, 0x85,
		0x00};
	int len = sizeof(copyright) - 1;
	for (i = 0; i < len; i++)
	{
		copyright[i] = (copyright[i] > i) ?
			(copyright[i]-i) : (copyright[i]+256-i);
		/*printf("%c", name[i]);*/
	}

	printf("BSA(break & stack analyser) version 1.0\n");
	printf("%s\n", copyright);
	printf("=======================================\n");
	printf("usage: bsa -f break_file -m map_file [-s(stack)] [-d(diab)]\n");
	return 0;
}

#define LINE_WIDTH 1024

int bsa_main(int argc, char** argv)
{
	int i = 0, is_stack = 0, is_diab = 0;
	char *func_name;
	char buffer[LINE_WIDTH] = {0};
	uint32 pc;
	FILE *fp, *fp_map;

	if (arg_lookup(argc, argv, "--help") || arg_lookup(argc, argv, "/?"))
	{
		return usage();
	}

	i = arg_lookup(argc, argv, "-f");
	if (i == 0 || i >= argc - 1)
	{
		printf("no info_file specified\n");
		return usage();
	}
	fp = fopen(argv[i + 1], "r");
	if (!fp)
	{
		printf("bad info_file: %s\n", argv[i + 1]);
		return -1;
	}

	i = arg_lookup(argc, argv, "-m");
	if (i == 0 || i >= argc - 1)
	{
		printf("no map_file specified\n");
		fclose(fp);
		return usage();
	}
	fp_map = fopen(argv[i + 1], "r");
	if (!fp_map)
	{
		printf("bad map_file: %s\n", argv[i + 1]);
		fclose(fp);
		return -1;
	}

	if (arg_lookup(argc, argv, "-s"))
		is_stack = 1;

	if (arg_lookup(argc, argv, "-d"))
		is_diab = 1;

	if (!is_stack)
	{
		while (fgets(buffer, LINE_WIDTH, fp))
		{
			buffer[strlen(buffer)-1] = '\0';
			if (strlen(buffer) < strlen("0x12345678--"))
				continue;

			if (strncmp(buffer, "0x", 2))
				continue;

			if (strncmp(buffer+10, "--", 2))
				continue;

			sscanf(buffer, "0x%08x", &pc);
			printf("0x%08x - ", pc);
			/*printf("%s - ", buffer);*/
			func_name = map_lookup(fp_map, pc, is_diab);
			printf("%s\n", func_name);
		}
	}
	else
	{
		int start = 0, index;
		char *prefix = "Stack";
		uint32 addr, up = 0, lr, info[4];

		while (fgets(buffer, LINE_WIDTH, fp))
		{
			buffer[strlen(buffer)-1] = '\0';
			if (!start)
			{
				if (!strncmp(prefix, buffer, strlen(prefix)))
				{
					printf("%s\n", buffer);
					start = 1;
				}
				continue;
			}

			if (0 != get_stack_line(buffer, &addr, info))
			{
				printf(/*"stack info over, lookup for another task...\n\n"*/"\n");
				start = 0;
				up = 0;
				if (!strncmp(prefix, buffer, strlen(prefix)))
				{
					printf("%s\n", buffer);
					start = 1;
				}
				continue;
			}

			/*printf("0x%08x:  \n", addr);*/
			if (up == 0)
			{
				up = info[0];
				lr = info[1];
				func_name = map_lookup(fp_map, lr, is_diab);
				printf("0x%08x: 0x%08x 0x%08x - %s\n", addr, up, lr, func_name);
			}
			if (addr + 16 <= up)
				continue;
			while (addr + 16 > up && addr <= up)
			{
				index = (up - addr)/sizeof(uint32);
				up = info[index];
				lr = info[index + 1];
				func_name = map_lookup(fp_map, lr, is_diab);
				printf("0x%08x: 0x%08x 0x%08x - %s\n", addr, up, lr, func_name);
			}
		}
	}

	fclose(fp_map);
	fclose(fp);

	return 0;
} 
