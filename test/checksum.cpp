#include <stdio.h>

typedef unsigned short __u16;
typedef unsigned int   __u32;

unsigned char ip_hdr[21] = {
	0x45, 0x00, 0x00, 0x1c, 
	0x06, 0xaf, 0x00, 0x00, 
	0x01, 0x02, 0x00, 0x00,
	0xc0, 0xa8, 0x14, 0x76, 
	0xe0, 0x01, 0x02, 0x78, 0x19
	};

__u16 checksum(__u16 *dp, __u16 length)
{
    __u32 sum = 0;
    __u16 len = length >> 1;

    int i = 0;
    for(i = 0; i < 21; i++) {
		printf("[%d]: 0x%x\t", i, (int)&ip_hdr[i]);
		if(!((i+1)%4)) puts("");
    }
    puts("");
    
    while(len-- > 0) {
        sum += *dp++;
		printf("while#%d,0x%x: 0x%x\n", len, (int)dp, sum);
    }
    if(length & 1) {
        sum += (*dp & 0xFF00);
	    printf("if: 0x%x\n", sum);
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
	printf("1: 0x%x\n", sum);
    sum += (sum >> 16);
	printf("2: 0x%x\n", sum);       
    len = sum & 0xFFFF;
	printf("len: 0x%x\n", len);       
    return(~len);
}

int cksum_test(int argc, char *argv[])
{
    __u16 *ip_hdr16 = (__u16*)ip_hdr;
	printf("0x%x\n", checksum(ip_hdr16, 21));

	long l = 0x11ff;
	short* ip = (short*)&l + 1;
	printf("0x%x: 0x%x\n", ip, *ip);
	printf("0x%x: 0x%x\n", &l, l);
	
	/* 0xfb99 */
	return 0;
}
