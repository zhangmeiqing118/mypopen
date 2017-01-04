/*
 * @Filename: aclk_dpi_decap.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:31:57 PM
 */

#ifndef __ACLK_DPI_DECAP_H__
#define __ACLK_DPI_DECAP_H__

#ifdef  __cpulsplus
extern "C" {
#endif

///specal tcp port
#define ACLK_DPI_PPTP_PORT      0x06bb

///specal udp port
#define ACLK_DPI_DHCP_PORT0     0x0043
#define ACLK_DPI_DHCP_PORT1     0x0044
#define ACLK_DPI_RIP_PORT0      0x0208
#define ACLK_DPI_RIP_PORT1      0x0209
#define ACLK_DPI_DHCPV6_PORT0   0x0222
#define ACLK_DPI_DHCPV6_PORT1   0x0223
#define ACLK_DPI_GTP_PORT0      0x084b
#define ACLK_DPI_GTP_PORT1      0x0868
#define ACLK_DPI_L2TP_PORT      0x06a5
#define ACLK_DPI_OPENVPN_PORT   0x04aa

///osi level
typedef enum aclk_decap_level {
    ACLK_DPI_PROTO_LEVEL_2 = 1,
    ACLK_DPI_PROTO_LEVEL_3,
    ACLK_DPI_PROTO_LEVEL_4,
    ACLK_DPI_PROTO_LEVEL_5,     ///decap payload
} aclk_decap_level_t;
#define ACLK_DPI_PROTO_PAYLOAD  0xffff

typedef enum aclk_dpi_app_id {
    ACLK_PIDE_PROTO_UNKOWN = 0,
    ACLK_PIDE_PROTO_ARP = 1,
    ACLK_PIDE_PROTO_ICMP,
    ACLK_PIDE_PROTO_IGMP,
    ACLK_PIDE_PROTO_GRE,
    ACLK_PIDE_PROTO_IPSEC_ESP,
    ACLK_PIDE_PROTO_IPSEC_AH,
    ACLK_PIDE_PROTO_OSPF,
    ACLK_PIDE_PROTO_SCTP,
    ACLK_PIDE_PROTO_FRAG,
    
    ACLK_PIDE_PROTO_PPTP,
    
    ACLK_PIDE_PROTO_DHCP,
    ACLK_PIDE_PROTO_RIP,
    ACLK_PIDE_PROTO_PPPOE,
    ACLK_PIDE_PROTO_PPP,
    ACLK_PIDE_PROTO_L2TP,

    ACLK_PIDE_PROTO_OPENVPN,
    ACLK_PIDE_PROTO_S1AP,
    ACLK_PIDE_PROTO_S6A,

    ACLK_PIDE_PROTO_INIT,
} aclk_dpi_app_id_t;


typedef struct aclk_dpi_decap_stat {
    uint64_t decap_pkt_num;
    uint64_t decap_pkt_err;

    uint64_t decap_pkt_vlan;
    uint64_t decap_pkt_mpls;
    uint64_t decap_pkt_arp;
    uint64_t decap_pkt_ipv4;  
    uint64_t decap_pkt_ipv6;
    uint64_t decap_pkt_pppoe;
    uint64_t decap_pkt_ppp;

///identity by l3 protocol
    uint64_t decap_pkt_icmp;
    uint64_t decap_pkt_igmp;
    uint64_t decap_pkt_tcp;
    uint64_t decap_pkt_udp;
    uint64_t decap_pkt_gre;
    uint64_t decap_pkt_ipsec_esp;
    uint64_t decap_pkt_ipsec_ah;
    uint64_t decap_pkt_ospf;
    uint64_t decap_pkt_sctp;
    uint64_t decap_pkt_frag;

///identity by tcp port
    uint64_t decap_pkt_pptp;
///identity by udp port
    uint64_t decap_pkt_dhcp;
    uint64_t decap_pkt_rip;
    uint64_t decap_pkt_gtp_ctrl;
    uint64_t decap_pkt_gtp_data;
    uint64_t decap_pkt_l2tp_ctrl;
    uint64_t decap_pkt_l2tp_data;
    uint64_t decap_pkt_openvpn;
    uint64_t decap_pkt_s1ap;
    uint64_t decap_pkt_s6a;
} aclk_dpi_decap_stat_t;

extern CVMX_SHARED aclk_dpi_decap_stat_t g_decap_stat;
///function define

void aclk_dpi_decap_stat_print(void);

int aclk_dpi_decap_init(void);

int aclk_dpi_decap_process_packet(cvmx_wqe_t *packet);

void aclk_dpi_decap_show(int argc, char *argv[]);

#ifdef  __cpulsplus
}
#endif
#endif


