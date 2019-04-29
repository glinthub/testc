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


void rand_fill(uint8* cp, int len)
{
	int i;
	
	for(i = 0; i < len; i++)
	{
		*(cp++) = rand()%256;
	}
}

/*
#define TOLOWER(c)	(c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c)
uint32 strnicmp(const char *str1, const char *str2, unsigned long n)
{
	int i;

	for(i = 0; i < n; i++)
	{
		if (TOLOWER(*(str1+i)) != TOLOWER(*(str2+i)))
			return -1;

		if (*(str1+i) == '\0')
			return 0;
	}
	return 0;
	
}
*/

uint32 msn_debug = 0;

uint32 get_tag(char **head, char *buf, char *tag)
{
	char *key, *tail;

	if (!buf || !head)
	{
		return 0;
	}

	while(1)
	{
		key = strchr(buf, '<');
		if (!key)
			break;

		tail = strchr(key, '>');
		if (!tail)
			break;
		if (strnicmp(key+1, tag, strlen(tag)))
		{
			buf = tail + 1;
			continue;
		}
		else
		{
			*head = key;
			return (tail + 1 - key);
		}
	}

	return 0;
}

uint32 get_block(char **head, char *buf, char *tag_begin, char *tag_end)
{
	char *tag1, *tag2;
	int len;

	len = get_tag(&tag1, buf, tag_begin);
	if (!len)
		return 0;
	
	*head = tag1;
	if (!tag_end)
	{
		return len;
	}

	len = get_tag(&tag2, tag1, tag_end);
	if (!len)
		return 0;

	return (tag2 + len - tag1);	
}

int get_parameter(char *tag_begin, char *paraname, char *value)
{
	uint32 len, i;
	char *cp1, *cp2;

	cp1 = strchr(tag_begin, '>'); /*tag_end*/
	if (!cp1)
		return -1;
	len = cp1 - tag_begin;
	for (i = 0; i < len; i++)
	{
		if (*(tag_begin + i) != ' ')
			continue;

		if (strnicmp(tag_begin + i + 1, paraname, strlen(paraname)))
			continue;

		if (*(tag_begin + i + 1 + strlen(paraname)) != '=')
			continue;

		cp1 = tag_begin + i + 1/*white space*/ + strlen(paraname) + 1/*'='*/;
		if (*cp1 == '"' || *cp1 == '\'')
		{
			cp2 = strchr(cp1 + 1, *cp1);
			cp1++;
		}
		else
		{
			cp2 = strchr(cp1, ' ');
			if (!cp2)
				cp2 = strchr(cp1, '>');
		}

		if (cp2)
		{
			strncpy(value, cp1, cp2-cp1);
			return 0;
		}
		else
			return -1;
	}
	return -2;
	
}

enum {
	MSG_TYPE_BEGIN,
	MSG_TYPE_MSG = MSG_TYPE_BEGIN,
	MSG_TYPE_JOIN,
	MSG_TYPE_LEAVE,
	MSG_TYPE_INVITATION,
	MSG_TYPE_COUNT
};

struct _msn_msg {
	uint32 sid;
	uint32 type;
	char date[16];
	char time[16];
	char datetime[32];
	char *body;
	uint32 len;
	struct _msn_msg *next;
};

struct _msn_session {
	uint32 id;
	char datetime[32];
	struct _msn_msg *msg;
	struct _msn_session *next;
};

struct _msn_msg_type {
	char *tag_begin;
	char *tag_end;
	char *body_begin;
	char *body_end;
} msg_tags[MSG_TYPE_COUNT] = 
{
	{"Message", "/Message", "From", "/text"},
	{"Join", "/Join", "User", "/text"},
	{"Leave", "/Leave", "User", "/text"},
	{"Invitation", "/Invitation", "From", "/text"},
};
int fput_session(struct _msn_session *sess, uint32 new_sid, FILE *fp)
{
	char buf[128] = {0};

	struct _msn_msg *msg = sess->msg;

	while(msg)
	{
		sprintf(buf, "<%s Date=\"%s\" Time=\"%s\" DateTime=\"%s\" SessionID=\"%d\">\n",
			msg_tags[msg->type].tag_begin, msg->date, msg->time, msg->datetime, new_sid);
		fwrite(buf, strlen(buf), 1, fp);
		fwrite(msg->body, msg->len, 1, fp);
		sprintf(buf, "<%s>\n", msg_tags[msg->type].tag_end);
		fwrite(buf, strlen(buf), 1, fp);
		msg = msg->next;
	}
	return 0;
}

void date_adjust(char *date)
{
	struct tm tm;

	if (strchr(date, '-'))
		sscanf(date, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
	else if (strchr(date, '/'))
		sscanf(date, "%d/%d/%d", &tm.tm_mon, &tm.tm_mday, &tm.tm_year);
	else
	{
		printf("date adjust error!\n");
		return;
	}
	sprintf(date, "%04d-%02d-%02d", tm.tm_year, tm.tm_mon, tm.tm_mday);
}

void time_adjust(char *time)
{
	struct tm tm;
	char *ap;

	sscanf(time, "%d:%d:%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
	ap = strchr(time, ' ');
	if (ap)
	{
		tm.tm_hour %= 12;
		if (!strnicmp(ap+1, "PM", 2))
			tm.tm_hour += 12;
	}
	sprintf(time, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

struct _msn_msg *get_msg(char **offset)
{
	uint32 len;
	char *limit = *offset + strlen(*offset);
	char *msg_tag = NULL;
	uint32 msg_type, i;
	char value[128] = {0};
	struct _msn_msg *msg = NULL, *msg_head = NULL, *msg_tail = NULL;
	uint32 session_id = 0;
	uint32 lens[MSG_TYPE_COUNT];
	char *offs[MSG_TYPE_COUNT];
	uint32 msg_cnt = 0;
	
	while(1)
	{
		if (*offset > limit)
		{
			*offset = limit;
			break;
		}
		
		msg_type = -1;
		msg_tag = NULL;
		for (i = MSG_TYPE_BEGIN; i < MSG_TYPE_COUNT; i++)
		{
			lens[i] = get_block(&offs[i], *offset, msg_tags[i].tag_begin, msg_tags[i].tag_end);
			if (!lens[i])
				continue;
			if (!msg_tag || msg_tag > offs[i])
			{
				msg_type = i;
				msg_tag = offs[i];
				len = lens[i];
			}
		}
		if (msg_type == -1)
			break;

		if (*offset + len > limit)
		{
			printf("msg exceed buf limit!\n");
			break;
		}

		if (get_parameter(msg_tag, "SessionID", value))
			break;

		if (session_id && session_id != atoi(value))
			break;
		session_id = atoi(value);

/*
if (msn_debug)
	printf("msg %d: *ofst = %p, tag=%p, type %d, len %d\n", msg_cnt, *offset, msg_tag, msg_type, len);
*/
		*offset = msg_tag + len;
		
		msg = malloc(sizeof(struct _msn_msg));
		if (!msg)
		{
			printf("no space to alloc msg!\n");
			continue;
		}
		msg_cnt ++;
		memset(msg, 0, sizeof(struct _msn_msg));
		msg->sid = atoi(value);
		msg->type = msg_type;
		get_parameter(msg_tag, "Date", msg->date);
		get_parameter(msg_tag, "Time", msg->time);
		date_adjust(msg->date);
		time_adjust(msg->time);
		get_parameter(msg_tag, "DateTime", msg->datetime);
		msg->len = get_block(&msg->body, msg_tag,
			msg_tags[msg->type].body_begin, msg_tags[msg->type].body_end);
		if (!msg->len)
		{
			printf("msg body not found\n");
			free(msg);
			continue;
		}

		if (msg_head)
		{
			msg_tail->next = msg;
			msg_tail = msg;
		}
		else
		{
			msg_head = msg_tail = msg;
		}
	}
	
	return msg_head;
}

uint32 get_msg_num(struct _msn_msg *msg)
{
	int num = 0;
	while (msg)
	{
		num++;
		msg = msg->next;
	}
	return num;
}

#define MSN_FILESIZE 0x00400000
struct _msn_session * get_session(char *buf)
{
	struct _msn_session *sess_head, *sess_tail, *sess;
	struct _msn_msg *msg;
	char *offset, *key;
	uint32 session_id;
	uint32 len;
	char value[128] = {0};

#define RET_ERR_SESS(text) \
	do { \
		printf("line %d: %s\n", __LINE__, text); \
		return NULL; \
		\
	} while(0)

	len = get_block(&key, buf, "Log", NULL);
	if (!len)
		RET_ERR_SESS("block <log> not found");	
	if (get_parameter(key, "FirstSessionID", value))
		RET_ERR_SESS("param fid search fail");
	printf("FirstSessionID %s\n", value);
	if (get_parameter(key, "LastSessionID", value))
		RET_ERR_SESS("param lid search fail");
	printf("LastSessionID %s\n", value);

	session_id = 0;
	sess = sess_head = sess_tail = NULL;
	offset = key + len;
	while(1)
	{
		if (offset > buf + strlen(buf))
			break;

		msg = get_msg(&offset);
		if (!msg)
			break;

		session_id = msg->sid;

if (session_id == 14)
	msn_debug = 1;
else
	msn_debug = 0;

		sess = malloc(sizeof(struct _msn_session));
		if (!sess)
		{
			printf("session alloc fail\n");
			continue;
		}

		memset(sess, 0, sizeof(struct _msn_session));
		sess->id = session_id;
		sprintf(sess->datetime, "%s %s", msg->date, msg->time);
		sess->msg = msg;

		if (sess_head)
		{
			sess_tail->next = sess;
			sess_tail = sess;
		}
		else
		{
			sess_head = sess_tail = sess;
		}
		printf("SessionID=%d, time=%s, msgcnt=%d\n", sess->id, sess->datetime, get_msg_num(msg));
	}

	return sess_head;
}

struct _msn_session * compose(struct _msn_session *sess1, struct _msn_session *sess2)
{
	struct _msn_session *sess_new, *sess_tmp;

	if (!sess1)
		return sess2;

	if (!sess2)
		return sess1;

	if (strcmp(sess1->datetime, sess2->datetime) < 0)
	{
		sess_new = sess1;
		sess1 = sess1->next;
	}
	else
	{
		sess_new = sess2;
		sess2 = sess2->next;
	}
	sess_tmp = sess_new;

	while (1)
	{
		if (!sess1)
		{
			sess_tmp->next = sess2;
			break;
		}
		if (!sess2)
		{
			sess_tmp->next = sess1;
			break;
		}
		
		if (strcmp(sess1->datetime, sess2->datetime) < 0)
		{
			sess_tmp->next = sess1;
			sess1 = sess1->next;
		}
		else
		{
			sess_tmp->next = sess2;
			sess2 = sess2->next;
		}
		sess_tmp = sess_tmp->next;
	}
	return sess_new;
}

uint32 get_sess_num(struct _msn_session *sess)
{
	int num = 0;
	while (sess)
	{
		num++;
		sess = sess->next;
	}
	return num;
}

void msn_free_session(struct _msn_session *sess)
{
	struct _msn_msg *msg;
	while (sess->msg)
	{
		msg = sess->msg;
		sess->msg = sess->msg->next;
		free(msg);
	}
	free(sess);
}

#define SESSION_SIZE 0x100000 /*1M*/

int get_wfile_len(WCHAR *wfilename)
{
	WCHAR *wbuf = (WCHAR *)malloc(MSN_FILESIZE*sizeof(WCHAR));
	WCHAR *wcp = wbuf, *wcp2;
	FILE *fp;

	if (!wbuf)
	{
		printf("wbuf alloc fail\n");
		return 0;
	}
	fp = _wfopen(wfilename, L"r");
	if (!fp)
	{
		printf("wfile open error\n");
		free(wbuf);
		return 0;
	}
	while (1)
	{
		wcp2 = fgetws(wcp, MSN_FILESIZE, fp);
		if (wcp2 != wcp)
			break;
		wcp += wcslen(wcp2);
	}
	free(wbuf);
	fclose(fp);
	return wcp - wbuf;
}

int msn_main(int argc, char** argv)
{
	char *filename1 = NULL, *filename2 = NULL, filename3[128] = {0};
	WCHAR wfilename[128];
	FILE *fp1 = NULL, *fp2 = NULL, *fp3 = NULL;
	char *buf1 = NULL, *buf2 = NULL;
	struct _msn_session *sess_list1, *sess_list2, *sess_new, *sess_tmp;
	char text[128] = {0};
	int i;

#define RET_ERR(text) \
	do { \
		printf("line %d: %s\n", __LINE__, text); \
		if (fp1) fclose(fp1); \
		if (fp2) fclose(fp2); \
		if (fp3) fclose(fp3); \
		if (buf1) free(buf1); \
		if (buf2) free(buf2); \
		return -1; \
		\
	} while(0)

	if (argc < 2)
		return -1;

	filename1 = argv[1]/*"msn1\\msn1.xml"*/;
	filename2 = argv[2]/*"msn2\\msn2.xml"*/;
	sprintf(filename3, "%s.new.xml", filename1);

	fp1 = fopen(filename1, "r");
	if (!fp1)
	{
		RET_ERR("f1 open err");
	}

	fp2 = fopen(filename2, "r");
	if (!fp2)
	{
		RET_ERR("f2 open error");
	}

	buf1 = malloc(MSN_FILESIZE);
	buf2 = malloc(MSN_FILESIZE);
	if (!buf1 || !buf2)
	{
		RET_ERR("file buffer alloc fail");
	}
	memset(buf1, 0, MSN_FILESIZE);
	memset(buf2, 0, MSN_FILESIZE);
	fread(buf1, MSN_FILESIZE, 1, fp1);
	fread(buf2, MSN_FILESIZE, 1, fp2);
	if (errno)
		RET_ERR("fread fail");
	
	MultiByteToWideChar (CP_ACP, 0, filename1, -1, wfilename, 128);
	buf1[get_wfile_len(wfilename)] = '\0';
	printf("file1 size: %d\n", strlen(buf1));
	//mem_dump(buf1, 64);
	MultiByteToWideChar (CP_ACP, 0, filename2, -1, wfilename, 128);
	buf2[get_wfile_len(wfilename)] = '\0';
	printf("file2 size: %d\n", strlen(buf2));
	//mem_dump(buf2, strlen(buf2) + 16);

#if 1
	sess_list1 = get_session(buf1);
	sess_list2 = get_session(buf2);
	if (!sess_list1 || !sess_list2)
	{
		if (sess_list1)
			execl("copy", filename1, filename3);
		if (sess_list2)
			execl("copy", filename2, filename3);
		RET_ERR("at least 1 file empty");
	}

	fp3 = fopen(filename3, "wr");
	if (!fp3)
	{
		RET_ERR("f3 open error");
	}

	sess_new = compose(sess_list1, sess_list2);
	sess_tmp = sess_new;
	sprintf(text, "<?xml version=\"1.0\"?>\n");
	fwrite(text, strlen(text), 1, fp3);
	sprintf(text, "<?xml-stylesheet type='text/xsl' href='MessageLog.xsl'?>\n");
	fwrite(text, strlen(text), 1, fp3);
	sprintf(text, "<Log FirstSessionID=\"1\" LastSessionID=\"%d\">\n", get_sess_num(sess_new));
	fwrite(text, strlen(text), 1, fp3);
	i = 0;
	while (sess_new)
	{
		i++;
		sess_tmp = sess_new->next;
		fput_session(sess_new, i, fp3);
		msn_free_session(sess_new);
		sess_new = sess_tmp;
	}
	sprintf(text, "</Log>\n");
	fwrite(text, strlen(text), 1, fp3);
#endif

	free(buf1);
	free(buf2);

	if (fp1)
		fclose(fp1);
	if (fp2)
		fclose(fp2);
	if (fp3)
		fclose(fp3);

	printf("compose complete.\n");
	getch();
	return 0;
} 

