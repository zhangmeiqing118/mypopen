/**
 * @File Name: aclk_dpi_decap.h
 * @brief: 
 * @Author: zhangmeiqing
 * @Version: 1.0.0
 * @Date: Tue 03 May 2016 02:26:21 PM CST
 */
#ifndef __ACLK_DPI_DECAP_H__
#define __ACLK_DPI_DECAP_H__

#ifdef  __cpulsplus
extern "C" {
#endif

#ifndef MAX_CORES
#define MAX_CORES   1
#endif

#ifndef CVMX_SHARED
#define CVMX_SHARED 
#endif

#ifndef ACLK_COMMAND_BUF_SIZE
#define ACLK_COMMAND_BUF_SIZE   (4 * 1024 * 1024)
#endif

extern CVMX_SHARED int g_log_level;

typedef enum {
    ACLK_DPI_LOG_LEVEL_NONE,
    ACLK_DPI_LOG_LEVEL_ERROR,
    ACLK_DPI_LOG_LEVEL_WARN,
    ACLK_DPI_LOG_LEVEL_INFO,
    ACLK_DPI_LOG_LEVEL_DEBUG,
    ACLK_DPI_LOG_LEVEL_ALL,
} aclk_dpi_log_level_t;

#define aclk_uart_printf printf

#define aclk_printf(level, fmt, arg ... )           \
    do {                                            \
        char *str;                                  \
                                                    \
        if (g_log_level >= level) {                 \
            switch (level) {                        \
                case ACLK_DPI_LOG_LEVEL_ERROR:      \
                    str = "ERROR";                  \
                    break;                          \
                case ACLK_DPI_LOG_LEVEL_WARN:       \
                    str = "WARN";                   \
                    break;                          \
                case ACLK_DPI_LOG_LEVEL_DEBUG:      \
                    str = "DEBUG";                  \
                    break;                          \
                case ACLK_DPI_LOG_LEVEL_INFO:       \
                default:                            \
                    str = "INFO";                   \
                    break;                          \
            }                                       \
            printf("[%s]"fmt, str, arg);            \
        }                                           \
    } while (0);

#pragma pack(push)
#pragma  pack(1)

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

////define protocol type
typedef enum aclk_dpi_proto {
    ACLK_DPI_PROTO_ETHER,
    ACLK_DPI_PROTO_ARP,
    ACLK_DPI_PROTO_VLAN,
    ACLK_DPI_PROTO_MPLS,
    ACLK_DPI_PROTO_PPPOE,
    ACLK_DPI_PROTO_IPV4,
    ACLK_DPI_PROTO_IPV6,
    ACLK_DPI_PROTO_ICMP,
    ACLK_DPI_PROTO_IGMP,
    ACLK_DPI_PROTO_DHCP,
    ACLK_DPI_PROTO_TCP,
    ACLK_DPI_PROTO_UDP,
    ACLK_DPI_PROTO_GRE,
    ACLK_DPI_PROTO_OSPF,
    ACLK_DPI_PROTO_IPSEC_ESP,
    ACLK_DPI_PROTO_IPSEC_AH,
    ACLK_DPI_PROTO_RIP,
    ACLK_DPI_PROTO_GTP,
    ACLK_DPI_PROTO_L2TP,
    ACLK_DPI_PROTO_PPTP,
    ACLK_DPI_PROTO_OPENVPN,
    ACLK_DPI_PROTO_FRAG,
    ACLK_DPI_PROTO_SCTP,
    ACLK_DPI_PROTO_S1AP,
    ACLK_DPI_PROTO_S6A,
    ACLK_DPI_PROTO_PPP,
    ACLK_DPI_PROTO_PAYLOAD,
} aclk_dpi_proto_t;

typedef enum aclk_dpi_app_no_flow {
    ACLK_PIDE_PROTO_UNKOWN = 0,
    ACLK_PIDE_PROTO_ARP = 1,
    ACLK_PIDE_PROTO_ICMP,
    ACLK_PIDE_PROTO_IGMP,
    ACLK_PIDE_PROTO_DHCP,
    ACLK_PIDE_PROTO_RIP,
    ACLK_PIDE_PROTO_GRE,
    ACLK_PIDE_PROTO_OSPF,
    ACLK_PIDE_PROTO_PPPOE,
    ACLK_PIDE_PROTO_PPP,
    ACLK_PIDE_PROTO_L2TP,
    ACLK_PIDE_PROTO_PPTP,
    ACLK_PIDE_PROTO_OPENVPN,
    ACLK_PIDE_PROTO_SCTP,
    ACLK_PIDE_PROTO_S1AP,
    ACLK_PIDE_PROTO_FRAG,
    ACLK_PIDE_PROTO_IPSEC_ESP,
    ACLK_PIDE_PROTO_IPSEC_AH,
    ACLK_PIDE_PROTO_INIT,
} aclk_dpi_app_no_flow_t;

typedef union {
    uint32_t ipv4;
    uint16_t ipv6[8];
} ip_info_t;

typedef void (*pide_callback)(void *);

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
    pide_callback callback;
} aclk_dpi_pkt_info_t;


typedef struct cvmx_wqe_word1 {
    uint64_t len:16;
    uint64_t zero_0:1;
    uint64_t qos:3;
    uint64_t zero_1:1;
    uint64_t grp:6;
    uint64_t zero_2:1;
    uint64_t tag_type:2;
    uint64_t tag:32;
} cvmx_wqe_word1_t;

typedef struct cvmx_wqe {
    uint64_t word0;
    cvmx_wqe_word1_t word1;
    uint64_t word2;
    void *packet_ptr;
    uint8_t packet_data[96];
} cvmx_wqe_t;

typedef enum aclk_dpi_tcp_flag {
    TCP_FLAG_FIN,
    TCP_FLAG_SYN,
    TCP_FLAG_RST,
    TCP_FLAG_ACK,
    TCP_FLAG_FIN_ACK,
    TCP_FLAG_SYN_ACK,
    TCP_FLAG_RST_ACK,
    TCP_FLAG_PSH_ACK,
} aclk_dpi_tcp_flag_t;

typedef struct {    
    uint64_t rx_pkt_num;
    uint64_t rx_pkt_err_num;
    uint64_t rx_pkt_frag_num;
    uint64_t rx_pkt_arp_num;
    uint64_t rx_pkt_pppoe_num;
    uint64_t rx_pkt_ppp_num;
    uint64_t rx_pkt_ipv4_num;  
    uint64_t rx_pkt_ipv6_num;
    uint64_t rx_pkt_tcp_num;
    uint64_t rx_pkt_udp_num;
    uint64_t rx_pkt_icmp_num;
    uint64_t rx_pkt_igmp_num;
    uint64_t rx_pkt_dhcp_num;
    uint64_t rx_pkt_pptp_num;
    uint64_t rx_pkt_openvpn_num;
    uint64_t rx_pkt_gre_num;
    uint64_t rx_pkt_sctp_num;
    uint64_t rx_pkt_s1ap_num;
    uint64_t rx_pkt_gtp_ctrl_num;
    uint64_t rx_pkt_gtp_data_num;
    uint64_t rx_pkt_l2tp_ctrl_num;
    uint64_t rx_pkt_l2tp_data_num;
    uint64_t rx_pkt_ipsec_esp_num;
} rx_pkt_count_t;

typedef struct eth_header_s {
    unsigned char  dst_mac [6];
    unsigned char  src_mac [6];
    unsigned short protocol;
} eth_header_t;

typedef struct vlan_header_s {
  unsigned short vlanId;
  unsigned short protocol;
} vlan_header_t;

typedef struct mpls_header {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t ttl:8;
    uint32_t s:1;
    uint32_t exp:3;
    uint32_t label:20;
#else
    uint32_t label:20;
    uint32_t exp:3;
    uint32_t s:1;
    uint32_t ttl:8;
#endif
} mpls_header_t;

typedef struct pppoe_header_s {
    uint8_t version:4;
    uint8_t type:4;
    uint8_t code;
    uint16_t sesid;
    uint16_t pay_len;
    //uint16_t proto;
} pppoe_header_t;

typedef struct eth_arp_s {
    unsigned short	hw_type;
    unsigned short 	pt_type;
    unsigned char	hw_size;
    unsigned char	pt_size;
    unsigned short	opcode;
    unsigned char	sender_mac[6];
    unsigned int	sender_ip;
    unsigned char	target_mac[6];
    unsigned int	target_ip;
} eth_arp_t;

typedef struct ipv4_header_s {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char	header_length:4;
    unsigned char	version:4;
#else
    unsigned char	version:4;
    unsigned char	header_length:4;
#endif
    unsigned char	tos;
    unsigned short	length;
    unsigned short	id;
    unsigned short	frag_offset;
    unsigned char	ttl;
    unsigned char	protocol;
    unsigned short	checksum;
    unsigned int	src_ip;
    unsigned int	dst_ip;
} ipv4_header_t;

struct in6_addr_s {
  union {
    unsigned char   __u6_addr8[16];
    unsigned short  __u6_addr16[8];
    unsigned int  __u6_addr32[4];
  } __u6_addr;                    /* 128-bit IP6 address */
};

typedef struct ipv6_header_s {
  union {
    struct mydpi_ip6_hdrctl {
      unsigned int ip6_un1_flow;
      unsigned short ip6_un1_plen;
      unsigned char ip6_un1_nxt;
      unsigned char ip6_un1_hlim;
    } ip6_un1;
    unsigned char ip6_un2_vfc;
  } ip6_ctlun;
  struct in6_addr_s ip6_src;
  struct in6_addr_s ip6_dst;
} ipv6_header_t;

typedef struct ipv6_frag {
    uint8_t next_proto;
    uint8_t res1;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint16_t more:1;
    uint16_t res2:2;
    uint16_t offset:13;
#else
    uint16_t offset:13;
    uint16_t res2:2;
    uint16_t more:1;
#endif
} ipv6_frag_t;

typedef struct tcp_header_s {
    unsigned short	src_port;
    unsigned short	dst_port;
    unsigned int	seqno;						
    unsigned int	ackno;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned short  res1:4;
    unsigned short  doff:4;
    
    unsigned short  fin:1;
    unsigned short  syn:1;
    unsigned short  rst:1;
    unsigned short  psh:1;
    unsigned short  ack:1;
    unsigned short  urg:1;
    unsigned short  res2:2;
#else
    unsigned short  doff:4;
    unsigned short  res1:4;
    unsigned short  res2:2;
    unsigned short  urg:1;
    unsigned short  ack:1;
    unsigned short  psh:1;
    unsigned short  rst:1;
    unsigned short  syn:1;
    unsigned short  fin:1;
#endif
    unsigned short 	window;
    unsigned short 	checksum;
    unsigned short 	urgent;
} tcp_header_t;

typedef struct udp_header_s {
    unsigned short src_port;
    unsigned short dst_port;
    unsigned short length;
    unsigned short checksum;
} udp_header_t;

typedef struct icmp_header_s {
    unsigned char	type;
    unsigned char	code;
    unsigned short	checksum;
    unsigned short	id;
    unsigned short	seqno;
} icmp_header_t;

typedef struct gtp_r99_header_s {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char npdu_flag:1;
    unsigned char seq_flag:1;
    unsigned char exhdr_flag:1;
    unsigned char rfu:1;
    unsigned char type:1;
    unsigned char version:3;
#else
    unsigned char version:3;
    unsigned char type:1;
    unsigned char rfu:1;
    unsigned char exhdr_flag:1;
    unsigned char seq_flag:1;
    unsigned char npdu_flag:1;
#endif
    unsigned char message_type;
    unsigned short length;
    unsigned int teid;
} gtp_r99_header_t;

typedef struct gtp_r99_exheader_s {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char npdu_flag:1;
    unsigned char seq_flag:1;
    unsigned char exhdr_flag:1;
    unsigned char rfu:1;
    unsigned char type:1;
    unsigned char version:3;
#else
    unsigned char version:3;
    unsigned char type:1;
    unsigned char rfu:1;
    unsigned char exhdr_flag:1;
    unsigned char seq_flag:1;
    unsigned char npdu_flag:1;
#endif
    unsigned char message_type;
    unsigned short length;
    unsigned int teid;
    unsigned short seq_num;
    unsigned char npdu_num;
    unsigned char exhdr_type;
} gtp_r99_exheader_t;

typedef struct gre_header_s {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int recur_ctrl:3;
    unsigned int ssr_flag:1;
    unsigned int seq_flag:1;
    unsigned int key_flag:1;
    unsigned int rnt_flag:1;
    unsigned int chk_flag:1;
    
    unsigned int version:3;
    unsigned int flags:4;
    unsigned int ack_flag:1;
#else
    unsigned int chk_flag:1;
    unsigned int rnt_flag:1;
    unsigned int key_flag:1;
    unsigned int seq_flag:1;
    unsigned int ssr_flag:1;
    unsigned int recur_ctrl:3;
   
    unsigned int ack_flag:1;
    unsigned int flags:4;
    unsigned int version:3;
#endif
    unsigned int protocol:16;
} gre_header_t;

typedef struct l2tp_pkt_type_s {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned short    pro:1;
    unsigned short    offset:2;
    unsigned short    seq:1;
    unsigned short	  pay1:2;
    unsigned short    len_present:1;
    unsigned short	  msg_type:1;
    
    unsigned short	  version:4;
    unsigned short    pay2:4;
#else
    unsigned short	  msg_type:1;
    unsigned short    len_present:1;
    unsigned short	  pay1:2;
    unsigned short    seq:1;
    unsigned short    offset:2;
    unsigned short    pro:1;

    unsigned short    pay2:4;
    unsigned short	  version:4;
#endif
} l2tp_pkt_type_t;
    
typedef struct l2tp_head_s {
    unsigned short	  pkt_type;
    unsigned short	  length;
    unsigned short    tunnel_id;
    unsigned short    session_id;
} l2tp_head_t;

typedef struct l2tp_head_no_len_s {
    unsigned short	  pkt_type;
    unsigned short    tunnel_id;
    unsigned short    session_id;
} l2tp_head_no_len_t;

typedef struct sctp_header_s {
	unsigned short srcport;
	unsigned short dstport;
	unsigned int ivertag;
	unsigned int ichecksum;
} sctp_header_t;

typedef struct sctp_data_s {
    uint8_t chunk_type;
    uint8_t chunk_flag;
    uint32_t tsn;
    uint16_t stream_id;
    uint16_t stream_seq;
    uint32_t next_proto;
} sctp_data_t;

///int aclk_decap_init_local(void);
int aclk_decap_init_global(void);

int aclk_decap_process_packet(cvmx_wqe_t *packet);
///int aclk_decap_process_command(void *packet, void *data);

void aclk_decap_cmd_rxstate(int argc, char *argv[]);

////void aclk_decap_fini_local(void);
////void aclk_decap_fini_global(void);

#pragma pack(pop)

#ifdef  __cpulsplus
}
#endif
#endif

