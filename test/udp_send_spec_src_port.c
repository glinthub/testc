#if 1/*_WIN32*/
#pragma comment(lib,"ws2_32")
#pragma pack(1)
#define WIN32_LEAN_AND_MEAN 
#include <winsock2.h>
#include <ws2tcpip.h> /* IP_HDRINCL */
#include <stdio.h>
#include <stdlib.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/timeb.h>
#include <string.h>
#endif

#include "test.h"

/* Define the IP header */
typedef struct ip_hdr {
    unsigned char  ip_verlen;        /* IP version & length */
    unsigned char  ip_tos;           /* IP type of service */
    unsigned short ip_totallength;   /* Total length */
    unsigned short ip_id;            /* Unique identifier */
    unsigned short ip_offset;        /* Fragment offset field */
    unsigned char  ip_ttl;           /* Time to live */
    unsigned char  ip_protocol;      /* Protocol */
    unsigned short ip_checksum;      /* IP checksum */
    unsigned int   ip_srcaddr;       /* Source address */
    unsigned int   ip_destaddr;      /* Destination address */
} IP_HDR, *PIP_HDR, FAR* LPIP_HDR;

/* Define the UDP header */
typedef struct udp_hdr {
    unsigned short src_portno;       /* Source port number */
    unsigned short dst_portno;       /* Destination port number */
    unsigned short udp_length;       /* UDP packet length */
    unsigned short udp_checksum;     /* UDP checksum (optional) */
} UDP_HDR, *PUDP_HDR;

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned int
#define uint64 unsigned long long

#define int8 char
#define int16 short
#define int32 int
#define int64 long long

typedef unsigned short __u16;
typedef unsigned int   __u32;

#define bzero(addr, count) memset(addr, 0, count)

#define PADDR(val)	printf(#val" addrs:\t0x%08x\n", (uint32)&(val));
#define PINT(val)	printf(#val" value:\t0x%08x\n", (uint32)(val))
#define PVAL(val)	printf(#val" value:\t%d\n", val)

__u16 checksum(__u16 *dp, __u16 length)
{
    __u32 sum = 0;
    __u16 len = length >> 1;

    while(len-- > 0) {
		printf("while#%d,0x%08x + 0x%04x\n", len, sum, (*dp));
        sum += (*dp);
		dp++;
    }
    if(length & 1) {
        sum += (*dp & 0xFF00);
	    printf("if: 0x%x\n", sum);
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
	/*printf("1: 0x%08x\n", sum);*/
    sum += (sum >> 16);
	printf("2: 0x%08x\n", sum);       
    len = sum & 0xFFFF;
	/*printf("result: 0x%04x->0x%04x\n", len, ~len);*/
    return(~len);
}

int udp1_main(int argc, char *argv[])
{
struct sockaddr addrfrom;
struct sockaddr addrto;
int s;
u_char outpack[65536];
char buf[1500];
IP_HDR *ip_hdr;
UDP_HDR *udp_hdr;

    struct sockaddr_in *from;
    struct sockaddr_in *to;
    struct protoent *proto;
    int i;
    char *src,*dest;
    int srcp, destp;
    int packetsize,datasize;
    int on = 1;
    char* cp;

#ifdef _WIN32
	WSADATA WSAData;
	if( WSAStartup(0x0110, &WSAData) ) {
	//if( WSAStartup(WINSOCK_VERSION, &WSAData) ) {
		printf("WSA error");
		exit(-1);
	}
#endif

    fprintf(stderr,"PingPong 1.0 - 970621 by Willy Tarreau <tarreau@aemiaif.ibp.fr>\n");
    fprintf(stderr,"<<< PLEASE USE THIS FOR TESTS ONLY AND WITH ADMINISTRATORS' AUTHORIZATION >>>\n\n");
#if 1
    if (argc!=5) {
        fprintf(stderr,"wrong arg count.\nUsage: pingpong src_addr src_port dst_addr dst_port\n");
        fprintf(stderr,"src_addr and dst_addr must be given as IP addresses (xxx.xxx.xxx.xxx)\n");
        fprintf(stderr,"Note that it often works with 127.0.0.1 as src_addr !\n");
        exit(2);
    }
#endif
    src=argv[1]/*"192.168.213.60"*/;
    srcp=atoi(argv[2])/*33090*/;
    dest=argv[3]/*"58.34.232.138"*/;
    destp=atoi(argv[4])/*10021*/;

	/*
    if (!(proto = getprotobyname("raw"))) {
        perror("getprotobyname(raw)");
        exit(2);
    }
    */

    /* "raw" should be 255 */
    if ((s = socket(AF_INET, SOCK_RAW, 17/*proto->p_proto*/)) < 0) {
        perror("socket");
        exit(2);
    }

	setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char *)&on, sizeof(on));

    memset(&addrfrom, 0, sizeof(struct sockaddr));
    from = (struct sockaddr_in *)&addrfrom;
    from->sin_family = AF_INET;
    from->sin_port=htons(srcp);
    if (!inet_aton(src, &from->sin_addr)) {
        fprintf(stderr,"Incorrect address for 'from': %s\n",src);
        exit(2);
    }

    memset(&addrto, 0, sizeof(struct sockaddr));
    to = (struct sockaddr_in *)&addrto;
    to->sin_family = AF_INET;
    to->sin_port=htons(destp);
    if (!inet_aton(dest, &to->sin_addr)) {
        fprintf(stderr,"Incorrect address for 'to': %s\n",dest);
        exit(2);
    }

    packetsize=0;

    /* lets's build a complete UDP packet from scratch */

    ip_hdr=(struct ip *)outpack;
    ip_hdr->ip_verlen=0x45;      /* IPv4 */
    ip_hdr->ip_tos=0;
    ip_hdr->ip_id=0;
    ip_hdr->ip_ttl=0x40;
    /*
    if (!(proto = getprotobyname("udp"))) {
        perror("getprotobyname(udp)");
        exit(2);
    }
    */
    /* "udp" should be 17 */
    ip_hdr->ip_protocol=/*proto->p_proto*/17;   /* udp */
    ip_hdr->ip_checksum=0;    /* null checksum, will be automatically computed by the kernel */
    ip_hdr->ip_srcaddr=from->sin_addr.s_addr;
    ip_hdr->ip_destaddr=to->sin_addr.s_addr;
    /* end of ip header */
    packetsize+=20;
    /* udp header */
    udp_hdr=(UDP_HDR *)((int)(outpack + 20));
    udp_hdr->src_portno=htons(srcp);
    udp_hdr->dst_portno=htons(destp);
    udp_hdr->udp_checksum=0;   /* ignore checksum */
    packetsize+=sizeof(UDP_HDR);
    /* end of udp header */
    /* add udp data here if you like */
    for (datasize=0;datasize<2;datasize++) {
        outpack[packetsize+datasize]='A'+datasize;
    }
    packetsize+=datasize;
    udp_hdr->udp_length=htons(sizeof(UDP_HDR)+datasize);
    ip_hdr->ip_totallength=htons(packetsize);

	cp = buf;
    memset(buf, 0, 1500);
    memcpy(cp, &ip_hdr->ip_srcaddr, sizeof(ip_hdr->ip_srcaddr));
    cp += sizeof(ip_hdr->ip_srcaddr);
    memcpy(cp, &ip_hdr->ip_destaddr, sizeof(ip_hdr->ip_destaddr));
    cp += sizeof(ip_hdr->ip_destaddr);
	cp += 1;
    memcpy(cp, &ip_hdr->ip_protocol, sizeof(ip_hdr->ip_protocol));
    cp += sizeof(ip_hdr->ip_protocol);
    memcpy(cp, &udp_hdr->udp_length, sizeof(udp_hdr->udp_length));
    cp += sizeof(udp_hdr->udp_length);
    memcpy(cp, udp_hdr, sizeof(UDP_HDR));
    cp += sizeof(UDP_HDR);
    memcpy(cp, outpack+sizeof(IP_HDR)+sizeof(UDP_HDR), datasize);
    cp += datasize;
    udp_hdr->udp_checksum = checksum((unsigned short *)buf, cp-buf);
	/*
    */

    if (sendto(s, (char *)outpack, packetsize, 0, &addrto, sizeof(struct sockaddr))==-1) {
        perror("sendto");
        exit(2);
    }
    printf("packet sent !\n");
    close(s);
}

int udpsend_spec_main(int argc, char** argv)
{
#ifdef _WIN32
	WSADATA WSAData;
	if( WSAStartup(0x0110, &WSAData) ) {
	//if( WSAStartup(WINSOCK_VERSION, &WSAData) ) {
		printf("WSA error");
		exit(-1);
	}
#endif

	/*f();*/

} 

