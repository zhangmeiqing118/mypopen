/**
 * @Filename: psend.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 06/16/2016 01:17:31 PM
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif

#include <endian.h>

#include "aclk_dpi_decap.h"

typedef unsigned int  bpf_u_int32;
typedef unsigned short  u_short;
typedef int bpf_int32;

struct timeval {
    uint32_t tv_sec;
    uint32_t tv_usec;
};

typedef struct pcap_header{
    struct timeval ts;
    bpf_u_int32 capture_len;
    bpf_u_int32 len;
} pcap_header_t;

typedef struct pcap_file_header {
    uint32_t magic;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t thiszone;    
    uint32_t sigfigs;   
    uint32_t snaplen;   
    uint32_t linktype;  
} pcap_file_header_t;

int g_write_flag = 0;

#define __WRITE_PCAP__  1

int main(int argc, char *argv[])
{
    FILE *fp, *fp_tmp;
    pcap_file_header_t file_head;
    pcap_header_t pcap_head;
    time_t tv_sec;
    struct  tm *tmp;
    char outstr[128];
    char sdata[BUFSIZ];
    int ssize;
    cvmx_wqe_t *wqe;
    aclk_dpi_pkt_info_t *pkt;

    if (argc != 2) {
        printf("Usage:\n \t%s [pcap file]\n", argv[0]);
        return -1;
    }

    printf("pcap file:%s\n", argv[1]);
    fp = fopen(argv[1], "r");
    if (NULL == fp) {
        printf("pcap file %s open error\n", argv[1]);
        return -1;
    }
#if __WRITE_PCAP__
    fp_tmp = fopen("tmp.pcap", "w+");
    if (NULL == fp_tmp) {
        printf("pcap file tmp.pcap open error\n");
        return -1;
    }
#endif

    if (1 != fread(&file_head, sizeof(pcap_file_header_t), 1, fp)) {
        printf("read pcap file header error\n", argv[1]);
        fclose(fp);

        return -1;
    }
#if __WRITE_PCAP__
    fwrite(&file_head, sizeof(pcap_file_header_t), 1, fp_tmp);
#endif

    file_head.magic = le32toh(file_head.magic);
    file_head.version_major = le16toh(file_head.version_major);
    file_head.version_minor = le16toh(file_head.version_minor);
    file_head.thiszone = le32toh(file_head.thiszone);
    file_head.sigfigs = le32toh(file_head.sigfigs);
    file_head.snaplen = le32toh(file_head.snaplen);
    file_head.linktype = le32toh(file_head.linktype);
    
    ///printf("magic:0x%x\n", file_head.magic);
    ///printf("major:0x%x\n", file_head.version_major);
    ///printf("minor:0x%x\n", file_head.version_minor);
    ///printf("thiszone:0x%x\n", file_head.thiszone);
    ///printf("sigfigs:%u\n", file_head.sigfigs);
    ///printf("snaplen:%u\n", file_head.snaplen);
    ///printf("linktype:%u\n", file_head.linktype);
    if (file_head.magic != 0xa1b2c3d4) {
        printf("Invalid pcap file\n");
        fclose(fp);
        return -1;
    }

    int g_sip_counter = 0;

    wqe = (cvmx_wqe_t *)sdata;
    wqe->packet_ptr = sdata + sizeof(cvmx_wqe_t);
    do {
        if (1 != fread(&pcap_head, sizeof(pcap_header_t), 1, fp)) {
            //printf("read pcap header error\n", argv[1]);
            break;
        }

        //printf("time:%d:%d\n", pcap_head.ts.tv_sec, pcap_head.ts.tv_usec);
        tv_sec = le32toh(pcap_head.ts.tv_sec);
        tmp = localtime(&tv_sec);
        strftime(outstr, 128, "%B %d %Y %r", tmp);
        ///printf("time:%s\n", outstr);
        ///printf("snaplen:%u, cap len:%u\n", le32toh(pcap_head.capture_len), le32toh(pcap_head.len));

        ssize = le32toh(pcap_head.len);
        if (1 != fread(wqe->packet_ptr, ssize, 1, fp)) {
            //printf("read pcap header error\n", argv[1]);
            break;
        }
        wqe->word1.len = ssize;
        memset(wqe->packet_data, 0x00, 96);
        g_write_flag = 0;
        if (-1 == aclk_decap_process_packet(wqe)) {
            ///g_write_flag = 1;
        }

        pkt = (aclk_dpi_pkt_info_t *)((uint8_t *)(wqe->packet_data));
#if __WRITE_PCAP__
        if (g_write_flag) {
            fwrite(&pcap_head, sizeof(pcap_header_t), 1, fp_tmp);
            fwrite(wqe->packet_ptr, ssize, 1, fp_tmp);
        }
#endif

        ///printf("sport:%d, dport:%d\n", pkt->sport, pkt->dport);
        if ((4376 == pkt->sport) || (4376 == pkt->dport)) {
            g_sip_counter++;
#if __WRITE_PCAP__
            ///fwrite(&pcap_head, sizeof(pcap_header_t), 1, fp_tmp);
            ///fwrite(wqe->packet_ptr, ssize, 1, fp_tmp);
#endif
        }

        ///send and recv data
    } while(1);

    fclose(fp);
#if __WRITE_PCAP__
    fclose(fp_tmp);
#endif

    ///
    argc = 2;
    argv[0] = "decap";
    argv[1] = "show";
    aclk_decap_cmd_rxstate(argc, argv);
    aclk_uart_printf("rx pkt sip num, count\t: %ld\n", g_sip_counter);

    return 0;
}
