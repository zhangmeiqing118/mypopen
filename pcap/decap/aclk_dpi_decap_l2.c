/**
 * @Filename: aclk_dpi_decap_l2.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:40:57 PM
 */
#include "aclk_sdk.h"
#include "aclk.h"

#include "aclk_dpi_decap.h"
#include "aclk_dpi_decap_l2.h"
#include "aclk_dpi_decap_l3.h"

static int aclk_dpi_decap_ether(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    ethhdr_t *ether;
    uint16_t next_proto;
    ///if need decap arp ,decap here

    if (len < (sizeof(ethhdr_t) + *offset)) {
        return -1;
    }

    ether = (ethhdr_t *)(data + *offset);
#if __BYTE_ORDER == __LITTLE_ENDIAN
    next_proto = ntohs(ether->proto);
#else 
    next_proto = ether->proto;
#endif

    ///printf("ether proto:%02x, proto:%02x\n", ether->protocol, protocol);
    *offset += sizeof(ethhdr_t);
    switch (next_proto) {  //define in linux/if_ether.h
        case 0x0800:
            *proto = ACLK_DPI_PROTO_IPV4;
            break;
        case 0x0806://arp
            *proto = ACLK_DPI_PROTO_ARP;
            break;
        case 0x8100:
        case 0x88A8:
        case 0x9100:
        case 0x9200:
        case 0x9300:
            *proto = ACLK_DPI_PROTO_VLAN;
            break;
        case 0x86dd:
            *proto = ACLK_DPI_PROTO_IPV6;
            break;
        case 0x8847:
        case 0x8848:
            *proto = ACLK_DPI_PROTO_MPLS;
            break;
        case 0x8864:
        case 0x8863:
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

int aclk_dpi_decap_level_2(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint32_t len, uint16_t *offset, uint16_t *protocol)
{
    int recode;

    recode =  aclk_dpi_decap_ether(pkt, data, len, offset, protocol);
    if (recode) {
        g_decap_stat[g_local_core_id].decap_pkt_err++;
    }

    return recode;
}
