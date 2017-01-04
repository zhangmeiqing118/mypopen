/*
 * @Filename: aclk_dpi_decap_l3.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:36:00 PM
 */
#ifndef __ACLK_DPI_DECAP_L3_H__
#define __ACLK_DPI_DECAP_L3_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum aclk_dpi_proto_l3 {
    ACLK_DPI_PROTO_L3_START = 0x2000,
    ACLK_DPI_PROTO_ARP,
    ACLK_DPI_PROTO_VLAN,
    ACLK_DPI_PROTO_MPLS,
    ACLK_DPI_PROTO_IPV4,
    ACLK_DPI_PROTO_IPV6,
    ACLK_DPI_PROTO_PPPOE,
    ACLK_DPI_PROTO_PPP,
} aclk_dpi_proto_l3_t;

typedef struct vlan_hdr {
  unsigned short vlanId;
  unsigned short next_proto;
} vlan_hdr_t;

typedef struct mpls_hdr {
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
} mpls_hdr_t;

typedef struct ipv4_hdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char	hdr_len:4;
    unsigned char	version:4;
#else
    unsigned char	version:4;
    unsigned char	hdr_len:4;
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
} ipv4_hdr_t;

typedef struct in6_addr_s {
  union {
    unsigned char   __u6_addr8[16];
    unsigned short  __u6_addr16[8];
    unsigned int  __u6_addr32[4];
  } __u6_addr;                    /* 128-bit IP6 address */
} in6_addr_t;

typedef struct ipv6_hdr {
  union {
    struct mydpi_ip6_hdrctl {
      unsigned int ip6_un1_flow;
      unsigned short ip6_un1_plen;
      unsigned char ip6_un1_nxt;
      unsigned char ip6_un1_hlim;
    } ip6_un1;
    unsigned char ip6_un2_vfc;
  } ip6_ctlun;
  in6_addr_t ip6_src;
  in6_addr_t ip6_dst;
} ipv6_hdr_t;

typedef struct pppoe_hdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t type:4;
    uint8_t version:4;
#else
    uint8_t version:4;
    uint8_t type:4;
#endif
    uint8_t code;
    uint16_t sid;
    uint16_t len;
} pppoe_hdr_t;

int aclk_dpi_decap_level_3(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint32_t len, uint16_t *offset, uint16_t *protocol);

#ifdef __cplusplus
}
#endif
#endif
