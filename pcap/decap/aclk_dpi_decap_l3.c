/**
 * @Filename: aclk_dpi_decap_l3.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:41:29 PM
 */
#include "aclk_sdk.h"
#include "aclk.h"

#include "aclk_dpi_decap.h"
#include "aclk_dpi_decap_l3.h"
#include "aclk_dpi_decap_l4.h"

static int aclk_dpi_decap_arp(aclk_dpi_pkt_info_t *pkt, __attribute__((unused))uint8_t *data, __attribute__((unused))uint16_t len, __attribute__((unused))uint16_t *offset, uint16_t *proto)
{
    ///if need decap arp ,decap here
    pkt->appidx = ACLK_PIDE_PROTO_ARP;
    g_decap_stat.decap_pkt_arp++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    return 0;
}

static int aclk_dpi_decap_vlan(__attribute__((unused))aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    vlan_hdr_t *vlan;
    uint16_t next_proto;

    g_decap_stat.decap_pkt_vlan++;

    do {
        if (len < (sizeof(vlan_hdr_t) + *offset)) {
            return -1;
        }

        vlan = (vlan_hdr_t *)(data + *offset);
#if __BYTE_ORDER == __LITTLE_ENDIAN
        next_proto = ntohs(vlan->next_proto);
#else
        next_proto = vlan->next_proto;
#endif

        *offset += sizeof(vlan_hdr_t);
        ///*proto = vlan->next_proto;
    } while ((next_proto == 0x8100) || (next_proto == 0x88A8) 
            || (next_proto == 0x9100) || (next_proto == 0x9200) || (next_proto == 0x9300) );

    switch (next_proto) {  //define in linux/if_ether.h
    case 0x0800:
        *proto = ACLK_DPI_PROTO_IPV4;
        break;
    case 0x0806://arp
        *proto = ACLK_DPI_PROTO_ARP;
        break;
    case 0x86dd:
        *proto = ACLK_DPI_PROTO_IPV6;
        break;
    case 0x8847:
    case 0x8848:
        *proto = ACLK_DPI_PROTO_MPLS;
        break;
    case 0x8864:
        *proto = ACLK_DPI_PROTO_PPPOE;
        break;
    case 0xc021:    ///ppp lcp
    case 0xc023:    ///ppp pap
    case 0x8021:    ///ppp ipcp
    case 0x8057:    ///ppp ipv6cp
        *proto = ACLK_DPI_PROTO_PPP;
        break;
    default:
        return -1;
    }

    return 0;
}

static int aclk_dpi_decap_mpls(__attribute__((unused))aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    mpls_hdr_t *mpls;
    ipv4_hdr_t *ipv4;

    g_decap_stat.decap_pkt_mpls++;
    do {
        if (len < (sizeof(mpls_hdr_t) + *offset)) {
            return -1;
        }

        mpls = (mpls_hdr_t *)(data + *offset);
        *offset += sizeof(mpls_hdr_t);
    } while (!(mpls->s));
    
    if (len < (sizeof(ipv4_hdr_t)) + *offset) {
        return -1;
    }
    ipv4 = (ipv4_hdr_t *)(data + *offset);
    if (4 == ipv4->version) {
        *proto = ACLK_DPI_PROTO_IPV4;
    } else if (6 == ipv4->version) {
        *proto = ACLK_DPI_PROTO_IPV6;
    } else {
        return -1;
    }

    return 0;
}

static int aclk_dpi_decap_ipv4(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    ipv4_hdr_t *ipv4;
    uint16_t frag_offset;
    uint8_t more_flag;//, donot_flag;

    if (len < (sizeof(ipv4_hdr_t) + *offset)) {
        return -1;
    }
    ipv4 = (ipv4_hdr_t *)(data + *offset);
    if (4 != ipv4->version) {
        return -1;
    }
    
    //ipv4 packet
    if (0 == pkt->ip_ver) {
        g_decap_stat.decap_pkt_ipv4++;
    } else if (6 == pkt->ip_ver) {
        g_decap_stat.decap_pkt_ipv6--;
        g_decap_stat.decap_pkt_ipv4++;
    }
    
#if __BYTE_ORDER == __LITTLE_ENDIAN
    frag_offset = ntohs(ipv4->frag_offset);
#else
    frag_offset = ipv4->frag_offset;
#endif
    more_flag = (frag_offset >> 13) & 0x01;
    frag_offset = frag_offset & 0x1ff;
    
    ///printf("ver:%d, hdr len:%d, ipv4->frag_offset:%d\n", ipv4->version, ipv4->hdr_len, frag_offset);
    ///printf("more:%d\n", more_flag);
    if (more_flag || frag_offset) {//ip fragment
        *proto = ACLK_DPI_PROTO_FRAG;
        return 0;
    }
    
    pkt->ip_ver = 4;
    pkt->l4_proto = ipv4->protocol;
    pkt->sip.ipv4= ipv4->src_ip;
    pkt->dip.ipv4 = ipv4->dst_ip;
    *offset = *offset + (ipv4->hdr_len << 2);
    ////printf("decp ipv4 protocol:%d, len:%d\n", ipv4->protocol, ipv4->hdr_len);
    switch (pkt->l4_proto) {
        case 0x01:///next is icmp
            *proto = ACLK_DPI_PROTO_ICMP;
            break;
        case 0x02:///next is igmp;
            *proto = ACLK_DPI_PROTO_IGMP;
            break;
        case 0x06:///next is tcp
            *proto = ACLK_DPI_PROTO_TCP;
            break;
        case 0x11:///next is udp
            *proto = ACLK_DPI_PROTO_UDP;
            break;
        case 0x29:///next is ipv6
            *proto = ACLK_DPI_PROTO_IPV6;
            break;
        case 0x2f:///next is gre
            *proto = ACLK_DPI_PROTO_GRE;
            break;
        case 0x32:///next is esp
            *proto = ACLK_DPI_PROTO_IPSEC_ESP;
            break;
        case 0x33:///next is ah
            *proto = ACLK_DPI_PROTO_IPSEC_AH;
            break;
        case 0x59:///next is ospf
            *proto = ACLK_DPI_PROTO_OSPF;
            break;
        case 0x84:///next is sctp
            *proto = ACLK_DPI_PROTO_SCTP;
            break;
        default:
            return -1;
    }
    ///printf("decp ipv4 proto:%d\n", *proto);
    
    return 0;
}

static int aclk_dpi_decap_ipv6(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    ipv6_hdr_t *ipv6;
    uint32_t version;

    if (len < (sizeof(ipv6_hdr_t) + *offset)) {
        return -1;
    }

    ipv6 = (ipv6_hdr_t *)(data + *offset);
#if __BYTE_ORDER == __LITTLE_ENDIAN
    version = ntohl(ipv6->ip6_ctlun.ip6_un1.ip6_un1_flow);
#else
    version = ipv6->ip6_ctlun.ip6_un1.ip6_un1_flow;
#endif
    
    version = (version >> 28) & 0x0f;
    if (6 != version) {
        return -1;
    }
    
    ///printf("version:%d\n", version);
    //ipv6 packet
    if (0 == pkt->ip_ver) {
        g_decap_stat.decap_pkt_ipv6++;
    } else if (4 == pkt->ip_ver) {
        g_decap_stat.decap_pkt_ipv4--;
        g_decap_stat.decap_pkt_ipv6++;
    }

    pkt->ip_ver = 6;
    pkt->l4_proto= ipv6->ip6_ctlun.ip6_un1.ip6_un1_nxt;           
    memcpy(pkt->sip.ipv6, ipv6->ip6_src.__u6_addr.__u6_addr16, 16);
    memcpy(pkt->dip.ipv6, ipv6->ip6_dst.__u6_addr.__u6_addr16, 16);
    *offset += sizeof(ipv6_hdr_t);
    switch (ipv6->ip6_ctlun.ip6_un1.ip6_un1_nxt) {
        case 0x3a:  ///next is icmpv6
            *proto = ACLK_DPI_PROTO_ICMP;
            break;
        case 0x02:  ///next is igmp
            *proto = ACLK_DPI_PROTO_IGMP;
            break;
        case 0x04:  ///next is ipv4
            *proto = ACLK_DPI_PROTO_IPV4;
            break;
        case 0x06:  ///next is tcp
            *proto = ACLK_DPI_PROTO_TCP;
            break;
        case 0x11:  ///next is udp
            *proto = ACLK_DPI_PROTO_UDP;
            break;
        case 0x2c:  ///next is gre
            *proto = ACLK_DPI_PROTO_FRAG;
            break;
        case 0x2f:  ///next is gre
            *proto = ACLK_DPI_PROTO_GRE;
            break;
        case 0x32:///next is esp
            *proto = ACLK_DPI_PROTO_IPSEC_ESP;
            break;
        case 0x33:///next is ah
            *proto = ACLK_DPI_PROTO_IPSEC_AH;
            break;
        case 0x00:  //do hop by hop header
        case 0x2b:  // do routing header
        case 0x3c:  //do destination options header
        default:
            return -1;
    }

    return 0;
}

static int aclk_dpi_decap_pppoe(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    pppoe_hdr_t *pppoe;
    uint16_t next_proto, payload_len;

    if (len < (sizeof(pppoe_hdr_t) + *offset)) {
        return -1;
    }

    pppoe = (pppoe_hdr_t *)(data + *offset);
    *offset += sizeof(pppoe_hdr_t);
#if __BYTE_ORDER ==  __LITTLE_ENDIAN
    payload_len = ntohs(pppoe->len);
#else
    payload_len = pppoe->len;
#endif
    if (payload_len) {
#if __BYTE_ORDER ==  __LITTLE_ENDIAN
        next_proto = ntohs(*((uint16_t *)(data + *offset)));
#else
        next_proto = *((uint16_t *)(data + *offset));
#endif

        ///printf("proto:%02x\n", next_proto);
        if ((0xc021 == next_proto) || (0x8021 == next_proto)
                || (0x8057 == next_proto) || (0xc023 == next_proto)) {
            *proto = ACLK_DPI_PROTO_PPP;
            return 0;
        }
    }
    pkt->appidx = ACLK_PIDE_PROTO_PPPOE;
    g_decap_stat.decap_pkt_pppoe++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;

    return 0;

}

static int aclk_dpi_decap_ppp(aclk_dpi_pkt_info_t *pkt, __attribute__((unused))uint8_t *data, __attribute__((unused))uint16_t len, __attribute__((unused))uint16_t *offset, uint16_t *proto)
{
    pkt->appidx = ACLK_PIDE_PROTO_PPP;
    g_decap_stat.decap_pkt_ppp++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;

    return 0;
}
int aclk_dpi_decap_level_3(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint32_t len, uint16_t *offset, uint16_t *protocol)
{
    int recode;

    recode = 0;
    switch (*protocol) {
        case ACLK_DPI_PROTO_ARP:
            recode = aclk_dpi_decap_arp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat.decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_VLAN:
            recode = aclk_dpi_decap_vlan(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat.decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_MPLS:
            recode = aclk_dpi_decap_mpls(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat.decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_IPV4:
            recode = aclk_dpi_decap_ipv4(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat.decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_IPV6:
            recode = aclk_dpi_decap_ipv6(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat.decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_PPPOE:
            recode = aclk_dpi_decap_pppoe(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat.decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_PPP:
            recode = aclk_dpi_decap_ppp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat.decap_pkt_err++;
            }
            break;
        default:
            recode = -1;
            break;
    }

    return recode;
}
