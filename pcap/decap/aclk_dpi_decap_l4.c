/**
 * @Filename: aclk_dpi_decap_l4.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:42:03 PM
 */
#include "aclk_sdk.h"
#include "aclk.h"

#include "aclk_dpi_decap_l3.h"
#include "aclk_dpi_decap_l4.h"

////identity by l4 protocol
static int aclk_dpi_decap_icmp(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    icmp_hdr_t *icmp;
    
    ///if need decap icmp ,decap here

    ///icmp don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_ICMP;
    g_decap_stat[g_local_core_id].decap_pkt_icmp++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;

    if (len < (sizeof(icmp_hdr_t) + *offset)) {
        return -1;
    }
    icmp = (icmp_hdr_t *)(data + *offset);

#ifdef __DEBUG__
    aclk_uart_printf("icmp type:%d, code :%d\n", icmp->type, icmp->code);
#endif
    
    return 0;
}

static int aclk_dpi_decap_igmp(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    igmp_hdr_t *igmp;

    ///if need decap icmp ,decap here

    ///igmp don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_IGMP;
    g_decap_stat[g_local_core_id].decap_pkt_igmp++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    if (len < (sizeof(igmp_hdr_t) + *offset)) {
        return -1;
    }
    igmp = (igmp_hdr_t *)(data + *offset);
#ifdef __DEBUG__
    aclk_uart_printf("igmp version:%d, type:%d\n", igmp->version, igmp->type);
#endif

    return 0;
}

static int aclk_dpi_decap_tcp(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    uint8_t tcp_flag;
    tcp_hdr_t *tcp;
    
    if (0 == pkt->sport) {
        g_decap_stat[g_local_core_id].decap_pkt_tcp++;
    } else if ((0x11 == pkt->l4_proto) && (pkt->sport)) {
        g_decap_stat[g_local_core_id].decap_pkt_udp--;
        g_decap_stat[g_local_core_id].decap_pkt_tcp++;
    }

    if (len < (sizeof(tcp_hdr_t) + *offset)) {
        return -1;
    }
    
    tcp = (tcp_hdr_t *)(data + *offset);
    pkt->l4_proto = 0x06;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    pkt->dport = ntohs(tcp->dst_port);
    pkt->sport = ntohs(tcp->src_port);
#else
    pkt->dport = tcp->dst_port;
    pkt->sport = tcp->src_port;
#endif

    tcp_flag = tcp->flag.tcp_flag;
    printf("tcp_flag:%x\n", tcp_flag);
    ///set tcp_flag;
    ///tcp_flag = *((uint8_t *)(data + *offset + 16));
    ///aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: tcp_flag:%d\n", __func__, __LINE__, tcp_flag);
    ///switch (tcp_flag){
    ///    case 0x01: //fin
    ///        pkt->tcp_flag = TCP_FLAG_FIN;
    ///        break;
    ///    case 0x02: //syn
    ///        pkt->tcp_flag = TCP_FLAG_SYN;
    ///        break;
    ///    case 0x04: //reset
    ///        pkt->tcp_flag = TCP_FLAG_RST;
    ///        break;
    ///    case 0x10: //ack
    ///        pkt->tcp_flag = TCP_FLAG_ACK;
    ///        break;
    ///    case 0x11: //fin ack
    ///        pkt->tcp_flag = TCP_FLAG_FIN_ACK;
    ///        break;
    ///    case 0x12: //syn ack
    ///        pkt->tcp_flag = TCP_FLAG_SYN_ACK;
    ///        break;
    ///    case 0x14: //rst ack
    ///        pkt->tcp_flag = TCP_FLAG_RST_ACK;
    ///        break;
    ///    case 0x18: //psh ack
    ///        pkt->tcp_flag = TCP_FLAG_PSH_ACK;
    ///        break;
    ///}

    *offset += (tcp->hdr_len << 2);
    if (ACLK_DPI_PPTP_PORT == pkt->dport || ACLK_DPI_PPTP_PORT == pkt->sport) {       
        *proto = ACLK_DPI_PROTO_PPTP;        
    } else {   
        *proto = ACLK_DPI_PROTO_PAYLOAD;
    }

    return 0;
}

static int aclk_dpi_decap_udp(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    int flag;
    udp_hdr_t *udp;
   
    if (0 == pkt->sport) {
        g_decap_stat[g_local_core_id].decap_pkt_udp++;
    } else if ((0x06 == pkt->l4_proto) && (pkt->sport)) {
        g_decap_stat[g_local_core_id].decap_pkt_tcp--;
        g_decap_stat[g_local_core_id].decap_pkt_udp++;
    }

    if (len < (sizeof(udp_hdr_t) + *offset)) {
        return -1;
    }
   
    udp = (udp_hdr_t *)(data + *offset);
    pkt->l4_proto = 0x11;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    pkt->dport = ntohs(udp->dst_port);
    pkt->sport = ntohs(udp->src_port);
#else
    pkt->dport = udp->dst_port;
    pkt->sport = udp->src_port;
#endif
    *offset += sizeof(udp_hdr_t);

    ///printf("udp sport:%d, dport:%d\n", pkt->sport, pkt->dport);
    flag = 0;
    switch (pkt->sport) {
        case ACLK_DPI_DHCP_PORT0:
            if ((4 == pkt->ip_ver) && (ACLK_DPI_DHCP_PORT1 == pkt->dport)) {
                *proto = ACLK_DPI_PROTO_DHCP;
            }
            break;
        case ACLK_DPI_DHCP_PORT1:
            if ((4 == pkt->ip_ver) && (ACLK_DPI_DHCP_PORT0 == pkt->dport)) {
                *proto = ACLK_DPI_PROTO_DHCP;
            }
            break;
        case ACLK_DPI_DHCPV6_PORT0:
            if ((6 == pkt->ip_ver) && (ACLK_DPI_DHCPV6_PORT1 == pkt->dport)) {
                *proto = ACLK_DPI_PROTO_DHCP;
            }
            break;
        case ACLK_DPI_DHCPV6_PORT1:
            if ((6 == pkt->ip_ver) && (ACLK_DPI_DHCPV6_PORT0 == pkt->dport)) {
                *proto = ACLK_DPI_PROTO_DHCP;
            }
            break;
        case ACLK_DPI_RIP_PORT0:
        case ACLK_DPI_RIP_PORT1:
            *proto = ACLK_DPI_PROTO_RIP;
            break;
        case ACLK_DPI_GTP_PORT0:
        case ACLK_DPI_GTP_PORT1:
            *proto = ACLK_DPI_PROTO_GTP;
            break;
        case ACLK_DPI_L2TP_PORT:
            *proto = ACLK_DPI_PROTO_L2TP;
            break;
        case ACLK_DPI_PPTP_PORT:
            *proto = ACLK_DPI_PROTO_PPTP;
            break;
        case ACLK_DPI_OPENVPN_PORT:
            *proto = ACLK_DPI_PROTO_OPENVPN;
            break;
        case ACLK_DPI_TEREDO_PORT:
            *proto = ACLK_DPI_PROTO_TEREDO;
            break;
        default:
            flag = 1;
            break;
    }
    
    if (flag) {
        flag = 0;
        switch (pkt->dport) {
            case ACLK_DPI_RIP_PORT0:
            case ACLK_DPI_RIP_PORT1:
                *proto = ACLK_DPI_PROTO_RIP;
                    break;
            case ACLK_DPI_GTP_PORT0:
            case ACLK_DPI_GTP_PORT1:
                *proto = ACLK_DPI_PROTO_GTP;
                    break;
            case ACLK_DPI_L2TP_PORT:
                *proto = ACLK_DPI_PROTO_L2TP;
                break;
            case ACLK_DPI_PPTP_PORT:
                *proto = ACLK_DPI_PROTO_PPTP;
                break;
            case ACLK_DPI_OPENVPN_PORT:
                *proto = ACLK_DPI_PROTO_OPENVPN;
                break;
            case ACLK_DPI_TEREDO_PORT:
                *proto = ACLK_DPI_PROTO_TEREDO;
                break;
            default:
                flag = 1;
                break;
        }
    }
    if (flag) {
        *proto = ACLK_DPI_PROTO_PAYLOAD;
    }
    ///printf("next proto:%d\n", *proto);

    return 0;
}

static int aclk_dpi_decap_gre(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    gre_hdr_t *gre;
    gre_route_t *route;

    g_decap_stat[g_local_core_id].decap_pkt_gre++;
    ///if need decap gre ,decap here

    ///gre don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_GRE;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    if (len < (sizeof(gre_hdr_t) + *offset)) {
        return -1; 
    }        

    gre = (gre_hdr_t *)(data + *offset); 
#ifdef __DEBUG__
    aclk_uart_printf("gre version:%d\n", gre->version);
#endif
        switch (gre->next_proto) {  //define in linux/if_ether.h
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
        default:
            return -1;
    }    
    
        ///pkt->l4_proto = gre->proto;
    *offset += sizeof(gre_hdr_t);
    if (gre->version) {
        ///not process gre 1;
        return -1;
    }
    if (gre->chk_flag || gre->rnt_flag) {
        *offset += 4;   ///checksum:2bytes, offset:2bytes
    }
    //if (gre->gre->rnt_flag) {
    //    *offset += 8;   ///route info; 8bytes;
    //}
    if (gre->key_flag) {
        *offset += 4;   ///key:4bytes
    }
    if (gre->seq_flag) {
        *offset += 4;   /// seq no: 4bytes
    }
    if (gre->rnt_flag) {
        do {
            route = (gre_route_t *)(data + *offset);
            *offset += sizeof(gre_route_t);
            *offset += route->len;
        } while (route->af);
    }
 

    return 0;
}

static int aclk_dpi_decap_ipsec_esp(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    ipv6_ext_hdr_esp_t *esp;
    ///if need decap ipsec esp ,decap here

    ///ipsec esp don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_IPSEC_ESP;

    g_decap_stat[g_local_core_id].decap_pkt_ipsec_esp++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;

    if (len < (sizeof(ipv6_ext_hdr_esp_t) + *offset)) {
        return -1;
    }
    esp = (ipv6_ext_hdr_esp_t *)(data + *offset);
    *offset += sizeof(ipv6_ext_hdr_esp_t);
#ifdef __DEBUG__
    aclk_uart_printf("esp spi:%d, seqno:%d\n", ntohl(esp->spi), ntohl(esp->seqno));
#endif

    return 0;
}

static int aclk_dpi_decap_ipsec_ah(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    ipv6_ext_hdr_ah_t *ext_hdr;

    if (len < *offset + sizeof(ipv6_ext_hdr_ah_t)) {
        return -1;
    }

    ext_hdr = (ipv6_ext_hdr_ah_t *)(data + *offset);
    *offset += (ext_hdr->ah_len + 2) << 2;  ///ipv6 ah header length
    //printf("decp ipv4 protocol:%d, len:%d\n", ipv4->protocol, ipv4->header_length);
    switch (ext_hdr->next_proto) {
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
    pkt->l4_proto = ext_hdr->next_proto;
    //printf("decp ah proto:%d\n", *proto);
    
    return 0;
}

static int aclk_dpi_decap_ospf(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    ospf_hdr_t *ospf;

    ///ospf don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_OSPF;

    g_decap_stat[g_local_core_id].decap_pkt_ospf++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;

    if (len < (sizeof(ospf_hdr_t) + *offset)) {
        return -1;
    }

    ospf = (ospf_hdr_t *)(data + *offset);
#ifdef __DEBUG__
    printf("ospf version:%d, type:%d\n", ospf->version, ospf->type);
#endif
    
    return 0;
}

static int aclk_dpi_decap_sctp(__attribute__((unused))aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    int flag;
    sctp_data_t *sctp_data;
    uint32_t next_proto;

    g_decap_stat[g_local_core_id].decap_pkt_sctp++;
    if (len < (sizeof(sctp_hdr_t) + *offset)) {
        return -1;
    }
    *offset += sizeof(sctp_hdr_t);
    if (len < sizeof(sctp_data_t) + *offset) {
        return -1;
    }

    flag = 0;
    do {
        sctp_data = (sctp_data_t *)(data + *offset);
#if __BYTE_ORDER == __LITTLE_ENDIAN
        next_proto = ntohl(sctp_data->next_proto);
#else
        next_proto = sctp_data->next_proto;
#endif
        switch (next_proto) {
            case 0x12:
                flag = 1;
                *proto = ACLK_DPI_PROTO_S1AP;
                break;
            case 0x2e:
                *proto = ACLK_DPI_PROTO_S6A;
                flag = 1;
                break;
            default:
                break;
        }
        *offset += sizeof(sctp_data_t);
        if (!flag) {
            if (len < sizeof(sctp_data_t) + *offset) {
                return -1;
            }
        }
    } while (!flag);

    return 0;
}

static int aclk_dpi_decap_frag(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    ipv6_frag_t *frag;

    g_decap_stat[g_local_core_id].decap_pkt_frag++; 
    if (4 == pkt->ip_ver) {
        pkt->appidx = ACLK_PIDE_PROTO_FRAG;
        ///for gre packet, process over
        *proto = ACLK_DPI_PROTO_PAYLOAD;
        return 0;
    } else {
        if (len < (sizeof(ipv6_frag_t) + *offset)) {
            return -1;
        }

        frag = (ipv6_frag_t *)(data + *offset);
#if __BYTE_ORDER == __LITTLE_ENDIAN
        pkt->frag_offset = ntohs(frag->offset) >> 3;
        pkt->frag_more = ntohs(frag->offset) & 0x01;
#else
        pkt->frag_offset = frag->offset >> 3;
        pkt->frag_more = frag->offset & 0x01;
#endif
        pkt->id = frag->id;
        *offset += sizeof(ipv6_frag_t);
        printf("frag offset:%d, more:%d\n", pkt->frag_offset , pkt->frag_more);

        if (pkt->frag_offset) {
            pkt->appidx = ACLK_PIDE_PROTO_FRAG;
            *proto = ACLK_DPI_PROTO_PAYLOAD;
            return 0;
        }

        switch (frag->next_proto) {
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
    }

    return 0;
}


static int aclk_dpi_decap_pptp(aclk_dpi_pkt_info_t *pkt, __attribute__((unused))uint8_t *data, __attribute__((unused))uint16_t len, __attribute__((unused))uint16_t *offset, uint16_t *proto)
{
    ///if need decap pptp ,decap here

    ///pptp don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_PPTP;

    g_decap_stat[g_local_core_id].decap_pkt_pptp++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    return 0;
}


static int aclk_dpi_decap_dhcp(aclk_dpi_pkt_info_t *pkt, __attribute__((unused))uint8_t *data, __attribute__((unused))uint16_t len, __attribute__((unused))uint16_t *offset, uint16_t *proto)
{
    ///if need decap dhcp ,decap here

    ///dhcp don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_DHCP;

    g_decap_stat[g_local_core_id].decap_pkt_dhcp++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    return 0;
}

static int aclk_dpi_decap_rip(aclk_dpi_pkt_info_t *pkt, __attribute__((unused))uint8_t *data, __attribute__((unused))uint16_t len, __attribute__((unused))uint16_t *offset, uint16_t *proto)
{
    ///if need decap rip ,decap here

    ///rip don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_RIP;

    g_decap_stat[g_local_core_id].decap_pkt_rip++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    return 0;
}

static int aclk_dpi_decap_gtp(__attribute__((unused))aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    ipv4_hdr_t *ipv4;
    gtp_r99_hdr_t *gtp;
    
    if (len < (sizeof(gtp_r99_hdr_t) + *offset)) {
        return -1;
    }

    gtp = (gtp_r99_hdr_t *)(data + *offset);
    if (0xff == gtp->message_type) {///gtp data
        g_decap_stat[g_local_core_id].decap_pkt_gtp_data++;                      
        if ((0 == gtp->exhdr_flag) && (0 == gtp->seq_flag) && (0 == gtp->npdu_flag)) {
            *offset += sizeof(gtp_r99_hdr_t);
        } else {           
            *offset += sizeof(gtp_r99_ext_hdr_t);              
        }

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
        ///printf("invalid type:%d, len:%d\n", ipv4->version, ipv4->header_length);
    } else {
        //gtp_sig
        g_decap_stat[g_local_core_id].decap_pkt_gtp_ctrl++;                      
        
        return -1;
    }

    return 0;
}

static int aclk_dpi_decap_l2tp(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint16_t len, uint16_t *offset, uint16_t *proto)
{
    l2tp_hdr_t *l2tp;
   
    pkt->appidx = ACLK_PIDE_PROTO_L2TP;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    if (len < (sizeof(l2tp_hdr_t) + *offset)) {
        return -1;
    }

    l2tp = (l2tp_hdr_t *)(data + *offset);
    if (0 == l2tp->msg_type) {
        g_decap_stat[g_local_core_id].decap_pkt_l2tp_data++;
    } else {
        g_decap_stat[g_local_core_id].decap_pkt_l2tp_ctrl++;
    }

    //pkt->dport = M_L2TP_PROTOCOL;
    //pkt->sport = M_L2TP_PROTOCOL;
    //if (M_L2TP_DATA_MSG == l2tp_packet_type->msg_type) {
    //    if (M_L2TP_DATA_LEN == l2tp_packet_type->len_present) {
    //        offset += sizeof(struct l2tp_head_no_len_s);
    //        if (packet->word1.len > offset) {
    //            pkt->payload_offset = (UINT16)offset;
    //        }
    //        //pkt->l4_proto = M_L2TP_PROTOCOL;
    //    } else {
    //        offset += sizeof(struct l2tp_head_s);
    //        if (packet->word1.len > offset) {
    //            pkt->payload_offset = (UINT16)offset;
    //        }
    //        //pkt->l4_proto = M_L2TP_PROTOCOL;
    //    }

    //    g_decap_stat[g_local_core_id].decap_pkt_l2tp_data_num++;
    //} else {
    //    //l2tp ctrl
    //    g_decap_stat[g_local_core_id].decap_pkt_l2tp_ctrl_num++;
    //    aclk_dpi_process_packet_over(packet);
    //    return -1;
    //}

    return 0;
}

static int aclk_dpi_decap_openvpn(aclk_dpi_pkt_info_t *pkt, __attribute__((unused))uint8_t *data, __attribute__((unused))uint16_t len, __attribute__((unused))uint16_t *offset, uint16_t *proto)
{
    ///if need decap openvpn ,decap here

    ///openvpn don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_OPENVPN;

    g_decap_stat[g_local_core_id].decap_pkt_openvpn++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    return 0;
}

static int aclk_dpi_decap_teredo(aclk_dpi_pkt_info_t *pkt, __attribute__((unused))uint8_t *data, __attribute__((unused))uint16_t len, __attribute__((unused))uint16_t *offset, uint16_t *proto)
{
    uint8_t ip_ver;

    printf("decap teredo\n");
    ip_ver = *((uint8_t *)(data + *offset));
#if __BYTE_ORDER == __LITTLE_ENDIAN
    ip_ver = ip_ver >> 4;
#else
    ip_ver = ip_ver & 0xf;
#endif
    printf("ip ver:%d\n", ip_ver);
    if (ip_ver == 4) {
        *proto = ACLK_DPI_PROTO_IPV4;
    } else if (ip_ver == 6) {
        *proto = ACLK_DPI_PROTO_IPV6;
    } else {
        return -1;
    }

    g_decap_stat[g_local_core_id].decap_pkt_teredo++;

    ///teredo don't need match more
    //pkt->appidx = ACLK_PIDE_PROTO_TEREDO;

    //g_decap_stat[g_local_core_id].decap_pkt_teredo++;
    
    return 0;
}

static int aclk_dpi_decap_s1ap(aclk_dpi_pkt_info_t *pkt, __attribute__((unused))uint8_t *data, __attribute__((unused))uint16_t len, __attribute__((unused))uint16_t *offset, uint16_t *proto)
{
    ///if need decap s1ap ,decap here

    ///s1ap don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_S1AP;

    g_decap_stat[g_local_core_id].decap_pkt_s1ap++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    return 0;
}

static int aclk_dpi_decap_s6a(aclk_dpi_pkt_info_t *pkt, __attribute__((unused))uint8_t *data, __attribute__((unused))uint16_t len, __attribute__((unused))uint16_t *offset, uint16_t *proto)
{
    ///if need decap s6a ,decap here

    ///s6a don't need match more
    pkt->appidx = ACLK_PIDE_PROTO_S6A;

    g_decap_stat[g_local_core_id].decap_pkt_s6a++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    return 0;
}

int aclk_dpi_decap_level_4(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint32_t len, uint16_t *offset, uint16_t *protocol)
{
    int recode;

    recode = 0;
    switch (*protocol) {
        case ACLK_DPI_PROTO_ICMP:
            recode = aclk_dpi_decap_icmp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_IGMP:
            recode = aclk_dpi_decap_igmp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_TCP:
            recode = aclk_dpi_decap_tcp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_UDP:
            recode = aclk_dpi_decap_udp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_GRE:
            recode = aclk_dpi_decap_gre(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_IPSEC_ESP:
            recode = aclk_dpi_decap_ipsec_esp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_IPSEC_AH:
            recode = aclk_dpi_decap_ipsec_ah(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_OSPF:
            recode = aclk_dpi_decap_ospf(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_SCTP:
            recode = aclk_dpi_decap_sctp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_FRAG:
            recode = aclk_dpi_decap_frag(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;

        case ACLK_DPI_PROTO_PPTP:
            recode = aclk_dpi_decap_pptp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;

        case ACLK_DPI_PROTO_DHCP:
            recode = aclk_dpi_decap_dhcp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_RIP:
            recode = aclk_dpi_decap_rip(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_GTP:
            recode = aclk_dpi_decap_gtp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_L2TP:
            recode = aclk_dpi_decap_l2tp(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_OPENVPN:
            recode = aclk_dpi_decap_openvpn(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_TEREDO:
            recode = aclk_dpi_decap_teredo(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_S1AP:
            recode = aclk_dpi_decap_s1ap(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        case ACLK_DPI_PROTO_S6A:
            recode = aclk_dpi_decap_s6a(pkt, data, len, offset, protocol);
            if (recode) {
                g_decap_stat[g_local_core_id].decap_pkt_err++;
            }
            break;
        default:
            g_decap_stat[g_local_core_id].decap_pkt_err++;                      
            recode = -1;
            break;
    }
    if (!recode) {
        pkt->payload_offset = *offset;
    }

    return recode;
}
