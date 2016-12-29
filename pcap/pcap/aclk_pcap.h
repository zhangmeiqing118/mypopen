/*
 * @Filename: aclk_pcap.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 12:13:00 PM
 */
#ifndef __ACLK_PCAP_H__
#define __ACLK_PCAP_H__

#ifdef __cplusplus
extern "C" {
#endif

struct time_val {
    uint32_t tv_sec;
    uint32_t tv_usec;
};

typedef struct pcap_packet_header{
    struct time_val ts;
    uint32_t capture_len;
    uint32_t len;
} pcap_packet_header_t;

typedef struct pcap_file_header {
    uint32_t magic;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t thiszone;    
    uint32_t sigfigs;   
    uint32_t snaplen;   
    uint32_t linktype;  
} pcap_file_header_t;


FILE *aclk_pcap_open(const char *filename);

int aclk_pcap_read(FILE *fp, char *buf, int len);

#ifdef __cplusplus
}
#endif
#endif
