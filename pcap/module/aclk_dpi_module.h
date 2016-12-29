/**
 * @File Name: aclk_dpi_module.h
 * @brief: 
 * @Author: zhangmeiqing
 * @Version: 1.0.0
 * @Date: Tue 03 May 2016 04:07:13 PM CST
 */
#ifndef __ACLK_DPI_MODULE_H__
#define __ACLK_DPI_MODULE_H__

#ifdef  __cpulsplus
extern "C" {
#endif

///macro for hardwar character

typedef union {
    uint32_t ipv4;
    uint16_t ipv6[8];
} ip_info_t;

///typedef void (*pide_callback)(void *);

///sizeof(aclk_dpi_pkt_info_t) must small than 96 byte
typedef struct aclk_dpi_pkt_info {
    ///word 0~3
    ip_info_t   sip; 
    ip_info_t   dip; 

    ///word 4
    uint16_t    sport; 
    uint16_t    dport; 
    uint32_t    l4_proto:8; 
    uint32_t    reserve:8;
    uint32_t    seq_no:16;

    ///word 5
    uint64_t     flow_dir:2;
    uint64_t     tcp_flag:4;
    uint64_t     ip_ver:4;
    uint64_t     reserve1:54;

    ///word 6
    uint16_t    payload_offset; 
    uint16_t    payload_len;  
    uint32_t    appidx;
    ///word 7
    uint32_t    flow_hash;

    ///word 8
    void *flow;

    ///word 9
    ///pide_callback callback;
} aclk_dpi_pkt_info_t;

#if 0
typedef enum {
    ACLK_MODULE_DECAP = 0x100,
    ACLK_MODULE_CONNECT = 0x200,
    ACLK_MODULE_COMMAND = 0x400,
    ACLK_MODULE_PIDE    = 0x600,
    ACLK_MAJOR_MASK = 0xFFFFFF00,
    ACLK_MINOR_MASK = 0x000000FF,
    ACLK_RESPONSE_MASK = 0x80000000,
} aclk_module_code_t;


typedef enum aclk_dpi_app_no_flow {
    ACLK_PIDE_PROTO_UNKOWN = 0,
    ACLK_PIDE_PROTO_ARP = 1,
    ACLK_PIDE_PROTO_ICMP,
    ACLK_PIDE_PROTO_IGMP,
    ACLK_PIDE_PROTO_DHCP,
    ACLK_PIDE_PROTO_RIP,
    ACLK_PIDE_PROTO_GRE,
    ACLK_PIDE_PROTO_OSPF,
    ACLK_PIDE_PROTO_L2TP,
    ACLK_PIDE_PROTO_PPTP,
    ACLK_PIDE_PROTO_SCTP,
    ACLK_PIDE_PROTO_S1AP,
    ACLK_PIDE_PROTO_FRAG,
    ACLK_PIDE_PROTO_IPSEC_ESP,
    ACLK_PIDE_PROTO_IPSEC_AH,
    ACLK_PIDE_PROTO_INIT,
} aclk_dpi_app_no_flow_t;
#endif


int aclk_dpi_init(void);

int aclk_dpi_process_packet(void *packet);
void aclk_dpi_process_packet_over(void *packet);

void aclk_dpi_fini(void);

#ifdef  __cpulsplus
}
#endif

#endif

