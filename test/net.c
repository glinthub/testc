#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/sockios.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <string.h>
#include <getopt.h>
 
struct socks {
    int fd;
    struct sockaddr_ll addr;
} dp_socket;
 
    char dmac[6] = {0,1,2,3,4,5};
    char smac[6] = {8,8,8,8,8,8};
    char svlan = 0;
    uint8_t svlanPri = 0;
    uint16_t svlanId = 200;
    char cvlan = 1;
    uint8_t cvlanPri = 0;
    uint16_t cvlanId = 100;
    uint32_t sip = 0x01010101;
    uint32_t dip = 0x01010102;
    int proto = 0xFD;
    int packetLen = 100;
    int packetNum = 10;
    int interval = 1; /*ms*/

int getoptions(int argc, char **argv)
{
    int c;
    int digit_optind = 0;
 
    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"help", 0, 0, 0},
            {"mac_dest", 1, 0, 0},
            {"mac_src", 1, 0, 0},
            {"s_vlan", 1, 0, 0},
            {"s_vlan_user_prior", 1, 0, 0},
            {"s_vlan_id", 1, 0, 0},
            {"c_vlan", 1, 0, 0},
            {"c_vlan_user_prior", 1, 0, 0},
            {"c_vlan_id", 1, 0, 0},
            {"ip_dest", 1, 0, 0},
            {"ip_src", 1, 0, 0},
            {"protocol", 1, 0, 0},
            {"frame_len", 1, 0, 0},
            {"packet_count", 1, 0, 0},
            {"packet_delay", 1, 0, 0},
            {"lif", 1, 0, 0},
            {0, 0, 0, 0}
        };
 
        c = getopt_long(argc, argv, "012",
                 long_options, &option_index);
        if (c == -1)
            break;
 
        switch (c) {
        case 0:
            switch (option_index) {
            case 0:
                return 0;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                cvlanPri = atoi(optarg);
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
            case 11:
                break;
            case 12:
                break;
            case 13:
                packetNum = atoi(optarg);
                break;
            case 14:
                interval = atoi(optarg);
                break;
            case 15:
                break;
            }
            printf("option %s with arg %s\n", long_options[option_index].name, optarg?optarg:"(null)");
            break;
 
        case '0':
        case '1':
        case '2':
            if (digit_optind != 0 && digit_optind != this_option_optind)
              printf("digits occur in two different argv-elements.\n");
            digit_optind = this_option_optind;
            printf("option %c\n", c);
            break;
 
        case '?':
            break;
 
        default:
            printf("?? getopt returned character code 0x%x ??\n", c);
        }
    }

    if (optind < argc) {
        printf("non-option ARGV-elements ignored: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }

    return c;
}

int create_sockets(void)
{
    struct ifreq       ifr;
    struct sockaddr_ll addr;
    int                retval;
    int                s;
    __u16              type;
    int                dev, num_devs=2;
     
    type = htons(0x8d8d);
     
    dp_socket.fd = -1;
    s = socket (PF_PACKET, SOCK_RAW, type);
 
    if (s < 0) {
        return (s);
    }
 
    memset ((void*)&ifr, 0, sizeof (ifr));
 
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "eth1");
 
    retval = ioctl (s, SIOCGIFINDEX, &ifr);
    if (retval < 0) {
        close (s);
        return (retval);
    }
 
    memset (&addr, 0, sizeof (addr));
    addr.sll_family   = AF_PACKET;
    addr.sll_ifindex  = ifr.ifr_ifindex;
    addr.sll_protocol = type;
 
    retval = bind (s, (struct sockaddr *) &addr, sizeof (addr));
    if (retval < 0) {
        close (s);
        return (retval);
    }
 
    dp_socket.fd = s;
    memset(&dp_socket.addr, 0, sizeof(dp_socket.addr));
 
    dp_socket.addr.sll_family  = AF_PACKET;
    dp_socket.addr.sll_ifindex = ifr.ifr_ifindex;
    dp_socket.addr.sll_protocol = type;

    return 0;
}
 
struct vlanField {
    uint16_t vid;
    uint16_t proto;
};
    

int net_main(int argc, char *argv[])
{
    fd_set wfd_set;
    int i;
    int ret;
    char wbuf[512];

    int vlanNum = 0;
    uint8_t offset;
 
    struct ethhdr *eh = (struct ethhdr *)wbuf;
    struct vlanField *vh;
    struct iphdr *ih;

    ret = getoptions(argc, argv);
    if (ret != -1)
    {
        printf("usage: %s --mac_dest 001122334455 --mac_src 0A0A0A0A0A0A --s_vlan false --s_vlan_user_prior 0 --s_vlan_id 200 --c_vlan true --c_vlan_user_prior 0 --c_vlan_id 100 --ip_src 1.1.1.1 --ip_dest 1.1.1.2 --protocol 253 --frame_len 100 --packet_count 10 --packet_delay 1 --lif eth1\n", argv[0]);
        exit(1);
    }
    FD_ZERO(&wfd_set);
    create_sockets();
     
 
    memset(wbuf, 0, 512);
//printf("eh=%p\n", eh);
    memcpy(eh->h_dest, dmac, 6);
    memcpy(eh->h_source, smac, 6);
    eh->h_proto = htons(0x0800);

    vh = (struct vlanField *)((uint8_t *)eh + sizeof(struct ethhdr));
    if (svlan == 1)
    {
        eh->h_proto = htons(0x8100);
        vh->vid = htons(svlanPri << 13 | svlanId & 0xfff);
        vh->proto = (cvlan == 1) ? htons(0x8100) : htons(0x0800);
        vh++;
        vlanNum++;
    }
    if (cvlan == 1)
    {
        eh->h_proto = htons(0x8100);
        vh->vid = htons(cvlanPri << 13 | cvlanId & 0xfff);
        vh->proto = htons(0x0800);
        vh++;
        vlanNum++;
    }

    ih = (struct iphdr*)vh;
//printf("ih=%p\n", ih);
    ih->ihl = 5;
    ih->version = 4;
    ih->tot_len = htons(packetLen - sizeof(struct ethhdr) - sizeof(struct vlanField)*vlanNum);
    ih->ttl=16;
    ih->protocol = 0xFD; /*experimental*/
    ih->saddr = 0x01010101;
    ih->daddr = 0x02010101;

    offset=(uint8_t *)ih+sizeof(struct iphdr)-(uint8_t *)eh;
    while (offset < packetLen)
    {
        memcpy(wbuf + offset, "0123456789", (offset+10<packetLen) ? 10 : packetLen-offset);
        offset+=10;
    }
 
    i = 0;
    while (i < packetNum) {
        ret = write(dp_socket.fd, wbuf, packetLen);
        if (ret <=0) {
            printf("error write\n");
        }
        usleep(interval * 1000);
        i++;
    }
 
    return 0;
}
