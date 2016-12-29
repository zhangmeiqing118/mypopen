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

typedef struct aclk_dpi_decap_stat {
    uint64_t decap_pkt;
    uint64_t decap_pkt_err;
    uint64_t decap_pkt_frag;
    uint64_t decap_pkt_arp;
    uint64_t decap_pkt_pppoe;
    uint64_t decap_pkt_ppp;
    uint64_t decap_pkt_ipv4;  
    uint64_t decap_pkt_ipv6;
    uint64_t decap_pkt_tcp;
    uint64_t decap_pkt_udp;
    uint64_t decap_pkt_icmp;
    uint64_t decap_pkt_igmp;
    uint64_t decap_pkt_dhcp;
    uint64_t decap_pkt_pptp;
    uint64_t decap_pkt_openvpn;
    uint64_t decap_pkt_gre;
    uint64_t decap_pkt_sctp;
    uint64_t decap_pkt_s1ap;
    uint64_t decap_pkt_gtp_ctrl;
    uint64_t decap_pkt_gtp_data;
    uint64_t decap_pkt_l2tp_ctrl;
    uint64_t decap_pkt_l2tp_data;
    uint64_t decap_pkt_ipsec_esp;
} aclk_dpi_decap_stat_t;

///function define

int aclk_dpi_decap_init(void);

int aclk_dpi_decap_process_packet(cvmx_wqe_t *packet);

void aclk_dpi_decap_show(int argc, char *argv[]);

#ifdef  __cpulsplus
}
#endif
#endif


