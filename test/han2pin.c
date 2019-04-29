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

#define uint8	unsigned char
#define uint16	unsigned short
#define uint32	unsigned int

#define debug_printf if (debug) printf

static uint32 debug;

#define HDB_OFFSET	32768
#define HDB_SIZE 	32768
#define PYB_SIZE	64

struct _hdb_rec {
	char pinyin[64];
};

struct _hdb_rec *hdb;
uint16 zchar_begin = 0xffff, zchar_end = 0x8000;

int read_hdb(char *filename)
{
	uint8 buf[PYB_SIZE + 1] = {0};
	FILE *fp;
	uint16 code;
	char *cp1;
	struct _hdb_rec *hdb_unit;

	debug_printf("hdb reading...\n");
	fp = fopen(filename, "r");
	if (!fp)
	{
		printf("open hdb file error\n");
		return -1;
	}

	hdb = malloc(sizeof(struct _hdb_rec) * HDB_SIZE);
	if (!hdb)
	{
		printf("alloc hdb memory fail\n");
		return -2;
	}
	
	memset(hdb, 0, sizeof(struct _hdb_rec) * HDB_SIZE);

	while(fgets(buf, PYB_SIZE, fp))
	{
		if (buf[strlen(buf)-2] == '\r' || buf[strlen(buf)-2] == '\n')
			buf[strlen(buf)-2] = '\0';
		else if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';

		assert(strlen(buf) > 3 && buf[2] == ' ');
		code = buf[0] * 256 + buf[1];
		hdb_unit = &(hdb[code - HDB_OFFSET]);
		//printf("hdb_unit=%p (%c) %d\n", hdb_unit, hdb_unit[0][0], sizeof(pinyin));
		cp1 = buf + 3;
		strcpy(hdb_unit->pinyin, cp1);
		if (code == 0x8000) printf("8000!!!!!\n");
		if (zchar_begin > code)
			zchar_begin = code;
		if (zchar_end < code)
			zchar_end = code;
	}
	fclose(fp);

	/**
	for (i=1; i<HDB_SIZE; i++)
	{
		hdb_unit = &(hdb[i]);
		if (*(hdb_unit->pinyin))
			printf("%c%c %s\n",(i+HDB_SIZE)/256, (i+HDB_SIZE)%256, hdb_unit->pinyin);
	}
	**/

	debug_printf("hdb read successfully, begin %04x, end %04x\n", zchar_begin, zchar_end);
	
	return 0;
}

int cold_num;
char * convert_zchar(uint16 zch)
{
	static char pinyin[64] = {0};
	struct _hdb_rec *hdb_unit;
	char *cp;

	debug_printf("convert zchar(0x%04x)\n", zch);
	if (zch > zchar_end || zch < zchar_begin || *(hdb[zch - HDB_OFFSET].pinyin) == '\0')
	{
		cold_num++;
		sprintf(pinyin, "<%c%c>", zch>>8, zch);

		return pinyin;
	}

	hdb_unit = &(hdb[zch - HDB_OFFSET]);
	sprintf(pinyin, "%s", hdb_unit->pinyin);
	/*多音字只取第一个音*/
	cp = strchr(pinyin, ',');
	if (cp)
		*cp = '\0';

	return pinyin;
}

int convert_str(uint8 *chi)     
{     
	int i;
	uint16 value;
	char buf[20] = {0};
	char *word_py;

	debug_printf("convert string: %s\n", chi);
	if (strlen(chi) % 2)
	{
		debug_printf("odd bytes(%s)\n", chi);
		return -1;
	}
	for (i = 0; i < strlen(chi); i+=2)
	{
		value = *(chi+i)*256 + *(chi+i+1);
		word_py = convert_zchar(value);
		if (word_py)
		{
			strcat(buf, word_py);
		}
	}
	printf("%s\n", buf);
	return 0;
}     

#define WORDSIZE 0x20
int parse_file(char *filename)
{
	uint8 buf[WORDSIZE + 1] = {0};
	FILE *fp = fopen(filename, "r");

	debug_printf("parse_file: %s\n", filename);
	if (!fp)
	{
		debug_printf("read file %s fail\n", filename);
		return -1;
	}

	while(fgets(buf, WORDSIZE, fp))
	{
		if (buf[strlen(buf)-2] == '\r' || buf[strlen(buf)-2] == '\n')
			buf[strlen(buf)-2] = '\0';
		else if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';
		convert_str(buf);
	}
	fclose(fp);
	debug_printf("cold_num: %d\n", cold_num);
	return 0;
}

int han2pin_main(int argc, char** argv)
{
	int i;

	for (i = 1; i<argc; i++)
	{
		if (!stricmp(argv[i], "-debug"))
		{
			debug = 1;
			break;
		}
	}

	if (0 != read_hdb("han2pin_hdb.txt"))
	{
		printf("read hdb file fail\n");
		return -1;
	}

	for (i = 1; i<argc; i++)
	{
		if (!stricmp(argv[i], "-f") && argc > i+1)
		{
			return parse_file(argv[i+1]);
		}
	}

	if (argc > 1)
		return convert_str(argv[1]);

	return 0;
} 

