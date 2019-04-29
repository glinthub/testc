#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

typedef unsigned char bool;
#define true 1
#define false 0

typedef struct socket_info {
    int sockfd;
    int proto;
    bool raw;
    bool initiated;
} socket_info_t;

int local_family = AF_INET;
bool listenOnly = false;

int init_socket_info(socket_info_t *sock) {
    if (sock->initiated)
        return 0;

    int type = SOCK_RAW;
    if (sock->proto == IPPROTO_UDP)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;
    if (sock->raw == true)
        type = SOCK_RAW;

    sock->sockfd = socket(local_family, type, sock->proto);
    if (sock->sockfd < 0) {
        fprintf(stderr, "Socket Error:%s\n\a", strerror(errno));
        exit(1);
    }

    if (sock->raw == true) {
        int on = 1;
        setsockopt(sock->sockfd,
                (local_family == AF_INET6) ? IPPROTO_IPV6 : IPPROTO_IP,
                IP_HDRINCL, &on, sizeof(on));
    }

    sock->initiated = true;
    return 0;
}

//unsigned short check_sum(unsigned short *addr, int len);

void a_to_in6(char *str_addr, struct in6_addr *in_addr) {
    char *p = 0;
    char *token = str_addr;
    int i = 0, j = -1;
    while (token && i < 8) {
        p = strchr(token, ':');
        if (p) {
            *p = 0;
        }
        in_addr->s6_addr16[i++] = htons(strtoul(token, NULL, 16));

        if (!p)
            break;

        token = p + 1;
        if (i == 8 && token) {
            printf("Error: too many fields found in ipv6 address!\n");
            exit(1);
        }

        if (*token == ':') {
            if (j != -1) {
                printf("Error: more than 1 \'::\' detected!\n");
                exit(1);
            }
            j = i;
            token++;
        }
    }

    if (j == -1 && i < 8) {
        printf("Error: insufficient fields found in ipv6 address!\n");
        exit(1);
    }

    if (j != -1) {
        int gap = 8 - i;
        int len = i - j;
        memcpy(&in_addr->s6_addr16[j + gap], &in_addr->s6_addr16[j], len * 2);
        memset(&in_addr->s6_addr16[j], 0, gap * 2);
    }
}

char *in6_to_a(struct in6_addr *in_addr) {
    static char str[41] = { 0 };
    char *p = str;
    int i = 0;
    int zero_filling = 0;
    int zero_filled = 0;
    for (i = 0; i < 8; i++) {
        if (in_addr->s6_addr16[i] == 0) {
            if (!zero_filled) {
                if (!zero_filling) {
                    *p++ = ':';
                    zero_filling = 1;
                    continue;
                } else
                    //already in filling
                    continue;
            }
        } else {
            if (zero_filling) {
                zero_filling = 0;
                zero_filled = 1;
            }
        }
        p += sprintf(p, "%x:", ntohs(in_addr->s6_addr16[i]));
    }
    if (p > str)
        *(p - 1) = 0;
    return str;
}

typedef union test_sockaddr_in {
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;
} test_sockaddr_in_t;

int printf_usage() {
    printf(
            "<executable> [-4 | -6] -da <destAddr> [-dp <destPort>] [-sa <srcAddr> [-sp <srcPort>] [-l]]\n");
    return 0;
}

void send_packet(socket_info_t *sock, void *addr, unsigned char* buf,
        unsigned int len) {
    int rc = -1;
    if (local_family == AF_INET6)
        rc = sendto(sock->sockfd, buf, len, 0, addr,
                sizeof(struct sockaddr_in6));
    else
        rc = sendto(sock->sockfd, buf, len, 0, addr,
                sizeof(struct sockaddr_in));
    printf("sendto returns %d\n", rc);
}

int socket_test_main(int argc, char **argv) {
    socket_info_t sock;
    test_sockaddr_in_t d_addr;
    test_sockaddr_in_t s_addr;

    char *dst_addr = 0;
    int dst_port = 8888;
    char *src_addr = 0;
    int src_port = 8888;

    int i;
    char *p = 0;
    for (i = 0; i < argc;) {
        if (!strcmp(argv[i], "-4")) {
            local_family = AF_INET;
        } else if (!strcmp(argv[i], "-6")) {
            local_family = AF_INET6;
            printf("Using ipv6\n");
        } else if (!strcmp(argv[i], "-l")) {
            listenOnly = true;
            printf("Listen only\n");
        } else if (!strcmp(argv[i], "-sa")) {
            if (++i == argc) {
                printf("Invalid source address\n");
                printf_usage();
                exit(1);
            }
            src_addr = argv[i];
        } else if (!strcmp(argv[i], "-sp")) {
            if (++i == argc) {
                printf("Invalid source port\n");
                printf_usage();
                exit(1);
            }
            src_port = argv[i];
        } else if (!strcmp(argv[i], "-da")) {
            if (++i == argc) {
                printf("Invalid destination address\n");
                printf_usage();
                exit(1);
            }
            dst_addr = argv[i];
        } else if (!strcmp(argv[i], "-dp")) {
            if (++i == argc) {
                printf("Invalid destination port\n");
                printf_usage();
                exit(1);
            }
            dst_port = argv[i];
        }
        i++;
    }

    printf("src addr %s\n", src_addr);
    printf("src port %d\n", src_port);

    if (dst_addr == 0 && src_addr == 0) {
        printf("Dst or Src address must be specified, or both.\n");
        printf_usage();
        exit(1);
    }

    sock.proto = IPPROTO_UDP;
    sock.raw = false;
    sock.initiated = false;
    init_socket_info(&sock);
    //setuid(getpid());

    bzero(&d_addr, sizeof(test_sockaddr_in_t));
    bzero(&s_addr, sizeof(test_sockaddr_in_t));
    if (local_family == AF_INET6) {
        if (dst_addr) {
            d_addr.addr6.sin6_family = AF_INET6;
            d_addr.addr6.sin6_port = htons(dst_port);
            //a_to_in6(dst_addr, &d_addr.addr6.sin6_addr);
            inet_pton(AF_INET6, dst_addr, &d_addr.addr6.sin6_addr);
            printf("dst: addr %s port %d\n", in6_to_a(&d_addr.addr6.sin6_addr),
                    ntohs(d_addr.addr6.sin6_port));
        }
        if (src_addr) {
            s_addr.addr6.sin6_family = AF_INET6;
            s_addr.addr6.sin6_port = htons(src_port);
            a_to_in6(src_addr, &s_addr.addr6.sin6_addr);
            if (bind(sock.sockfd, (struct sockaddr*) &s_addr,
                    sizeof(struct sockaddr_in6)) != 0) {
                close(sock.sockfd);
                p = strerror(errno);
                printf("Bind Error:%s\n\a", (p));
                exit(1);
            }
        }
    } else {
        if (dst_addr) {
            d_addr.addr4.sin_family = AF_INET;
            d_addr.addr4.sin_port = htons(dst_port);
            inet_aton(dst_addr, &d_addr.addr4.sin_addr);
            printf("dst addr %s port %d\n", inet_ntoa(d_addr.addr4.sin_addr),
                    dst_port);
        }
        if (src_addr) {
            s_addr.addr4.sin_family = AF_INET;
            s_addr.addr4.sin_port = htons(src_port);
            inet_aton(src_addr, &s_addr.addr4.sin_addr);
            if (bind(sock.sockfd, (struct sockaddr*) &s_addr,
                    sizeof(struct sockaddr_in)) != 0) {
                close(sock.sockfd);
                p = strerror(errno);
                printf("Bind Error:%s\n\a", (p));
                exit(1);
            }
        }
    }

    if (listenOnly) {
        while (1) {
            sleep(1);
            printf("Listening...\n");
        }
    } else if (dst_addr) {
        unsigned char buf[100] = "hello, socket!";
        while (1) {
            send_packet(&sock, &d_addr, buf, sizeof(buf));
            sleep(1);
        }
    }
    return 0;
}

#if 0
void send_udp(int sockfd, struct sockaddr_in *addr, int raw) {
    sendto(sockfd, buffer)
}

void send_tcp(int sockfd, struct sockaddr_in *addr, int raw) {
    char buffer[100];
    struct ip *ip;
    struct tcphdr *tcp;
    int head_len;
    head_len = sizeof(struct ip) + sizeof(struct tcphdr);
    bzero(buffer, 100);
    ip = (struct ip *) buffer;
    ip->ip_v = IPVERSION;
    ip->ip_hl = sizeof(struct ip) >> 2;
    ip->ip_tos = 0;
    ip->ip_len = htons(head_len);
    ip->ip_id = 0;
    ip->ip_off = 0;
    ip->ip_ttl = MAXTTL;
    ip->ip_p = IPPROTO_TCP;
    ip->ip_sum = 0;
    ip->ip_dst = addr->sin_addr;
    tcp = (struct tcphdr *) (buffer + sizeof(struct ip));
    tcp->source = htons(LOCALPORT);
    tcp->dest = addr->sin_port;
    tcp->seq = random();
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->syn = 1;
    tcp->check = 0;
//	while (1) {
    ip->ip_src.s_addr = random();
    tcp->check = check_sum((unsigned short *) tcp, sizeof(struct tcphdr));
    sendto(sockfd, buffer, head_len, 0, addr, sizeof(struct sockaddr_in));

//	}
}
#endif
