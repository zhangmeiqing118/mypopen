/*
 * @Filename: aclk_dpi_decap_l4.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:36:00 PM
 */
#ifndef __ACLK_DPI_DECAP_L4_H__
#define __ACLK_DPI_DECAP_L4_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum aclk_dpi_proto_l4 {
    ACLK_DPI_PROTO_L4_START = 0x4000,
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
} aclk_dpi_proto_l4_t;

typedef struct icmp_hdr {
    unsigned char	type;
    unsigned char	code;
    unsigned short	checksum;
    unsigned short	id;
    unsigned short	seqno;
} icmp_hdr_t;

typedef struct igmp_hdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char type:4;
    unsigned char version:4;
#else
    unsigned char version:4;
    unsigned char type:4;
#endif
    unsigned char rtime;
    unsigned short chksum;
    unsigned int group;
} igmp_hdr_t;

typedef struct tcp_hdr {
    unsigned short	src_port;
    unsigned short	dst_port;
    unsigned int	seqno;						
    unsigned int	ackno;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char  res1:4;
    unsigned char  hdr_len:4;
    union {
        unsigned char  tcp_flag;
        struct {
            unsigned char  fin:1;
            unsigned char  syn:1;
            unsigned char  rst:1;
            unsigned char  psh:1;
            unsigned char  ack:1;
            unsigned char  urg:1;
            unsigned char  echo:1;
            unsigned char  cwr:1;
        } s;
    } flag;
#else
    unsigned char  hdr_len:4;
    unsigned char  res1:4;
    union {
        uint16_t tcp_flag;
        struct {
            unsigned char  cwr:1;
            unsigned char  echo:1;
            unsigned char  urg:1;
            unsigned char  ack:1;
            unsigned char  psh:1;
            unsigned char  rst:1;
            unsigned char  syn:1;
            unsigned char  fin:1;
        }s;
    } flag;
#endif
    unsigned short 	window;
    unsigned short 	checksum;
    unsigned short 	urgent;
} tcp_hdr_t;

typedef struct udp_hdr {
    unsigned short src_port;
    unsigned short dst_port;
    unsigned short length;
    unsigned short checksum;
} udp_hdr_t;

typedef struct ipv6_ext_hdr_esp {
    unsigned int spi;
    unsigned int seqno;
} ipv6_ext_hdr_esp_t;

typedef struct ipv6_ext_hdr_ah {
    uint8_t next_proto;
    uint8_t ah_len;
    uint16_t res;
    uint32_t secure;
    uint32_t seqno;
} ipv6_ext_hdr_ah_t;

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

typedef struct gre_hdr {
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
    unsigned short next_proto;
} gre_hdr_t;

typedef struct ospf_hdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char type:4;
    unsigned char version:4;
#else
    unsigned char version:4;
    unsigned char type:4;
#endif
    unsigned short len;
    unsigned int route;
    unsigned int area;
    unsigned short chksum;
    unsigned short ah_type;
    uint64_t value;
} ospf_hdr_t;

typedef struct l2tp_hdr {
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
} l2tp_hdr_t;
    
typedef struct sctp_hdr {
	unsigned short src_port;
	unsigned short dst_port;
	unsigned int ivertag;
	unsigned int ichecksum;
} sctp_hdr_t;

typedef struct sctp_data {
    uint8_t chunk_type;
    uint8_t chunk_flag;
    uint32_t tsn;
    uint16_t stream_id;
    uint16_t stream_seq;
    uint32_t next_proto;
} sctp_data_t;

typedef struct gtp_r99_hdr {
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
} gtp_r99_hdr_t;

typedef struct gtp_r99_ext_hdr {
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
} gtp_r99_ext_hdr_t;


int aclk_dpi_decap_level_4(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint32_t len, uint16_t *offset, uint16_t *protocol);

#ifdef __cplusplus
}
#endif
#endif
