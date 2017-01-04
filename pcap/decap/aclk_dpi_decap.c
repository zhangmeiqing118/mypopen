/**
 * @Filename: aclk_dpi_decap.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:15:00 PM
 */
#include "aclk_sdk.h"
#include "aclk.h"

#include "aclk_dpi_decap.h"
#include "aclk_dpi_decap_l2.h"
#include "aclk_dpi_decap_l3.h"
#include "aclk_dpi_decap_l4.h"

CVMX_SHARED aclk_dpi_decap_stat_t g_decap_stat;

void aclk_dpi_decap_stat_print(void)
{
    aclk_uart_printf("============= decap rx pkt count=============\n");
    aclk_uart_printf("rx pkt count              : %"PRIu64"\n", g_decap_stat.decap_pkt_num);
    aclk_uart_printf("rx pkt err count          : %"PRIu64"\n", g_decap_stat.decap_pkt_err);

    aclk_uart_printf("rx pkt vlan count         : %"PRIu64"\n", g_decap_stat.decap_pkt_vlan);
    aclk_uart_printf("rx pkt mpls count         : %"PRIu64"\n", g_decap_stat.decap_pkt_mpls);
    aclk_uart_printf("rx pkt arp count          : %"PRIu64"\n", g_decap_stat.decap_pkt_arp); 
    aclk_uart_printf("rx pkt ipv4  count        : %"PRIu64"\n", g_decap_stat.decap_pkt_ipv4);
    aclk_uart_printf("rx pkt ipv6 count         : %"PRIu64"\n", g_decap_stat.decap_pkt_ipv6);
    aclk_uart_printf("rx pkt pppoe count        : %"PRIu64"\n", g_decap_stat.decap_pkt_pppoe); 
    aclk_uart_printf("rx pkt ppp  count         : %"PRIu64"\n", g_decap_stat.decap_pkt_ppp);

    aclk_uart_printf("rx pkt icmp  count        : %"PRIu64"\n", g_decap_stat.decap_pkt_icmp);
    aclk_uart_printf("rx pkt igmp  count        : %"PRIu64"\n", g_decap_stat.decap_pkt_igmp);
    aclk_uart_printf("rx pkt tcp count          : %"PRIu64"\n", g_decap_stat.decap_pkt_tcp);
    aclk_uart_printf("rx pkt udp count          : %"PRIu64"\n", g_decap_stat.decap_pkt_udp);
    aclk_uart_printf("rx pkt gre  count         : %"PRIu64"\n", g_decap_stat.decap_pkt_gre);
    aclk_uart_printf("rx pkt ipsec esp count    : %"PRIu64"\n", g_decap_stat.decap_pkt_ipsec_esp);
    aclk_uart_printf("rx pkt ipsec ah count     : %"PRIu64"\n", g_decap_stat.decap_pkt_ipsec_ah);
    aclk_uart_printf("rx pkt ospf count         : %"PRIu64"\n", g_decap_stat.decap_pkt_ospf);
    aclk_uart_printf("rx pkt sctp  count        : %"PRIu64"\n", g_decap_stat.decap_pkt_sctp);
    aclk_uart_printf("rx pkt frag count         : %"PRIu64"\n", g_decap_stat.decap_pkt_frag);

    aclk_uart_printf("rx pkt pptp count         : %"PRIu64"\n", g_decap_stat.decap_pkt_pptp);

    aclk_uart_printf("rx pkt dhcp  count        : %"PRIu64"\n", g_decap_stat.decap_pkt_dhcp);
    aclk_uart_printf("rx pkt rip  count         : %"PRIu64"\n", g_decap_stat.decap_pkt_rip);
    aclk_uart_printf("rx pkt gtp ctrl count     : %"PRIu64"\n", g_decap_stat.decap_pkt_gtp_ctrl);
    aclk_uart_printf("rx pkt gtp data count     : %"PRIu64"\n", g_decap_stat.decap_pkt_gtp_data);
    aclk_uart_printf("rx pkt l2tp ctrl count    : %"PRIu64"\n", g_decap_stat.decap_pkt_l2tp_ctrl);
    aclk_uart_printf("rx pkt l2tp data  count   : %"PRIu64"\n", g_decap_stat.decap_pkt_l2tp_data);
    aclk_uart_printf("rx pkt openvpn count      : %"PRIu64"\n", g_decap_stat.decap_pkt_openvpn);
    aclk_uart_printf("rx pkt s1ap  count        : %"PRIu64"\n", g_decap_stat.decap_pkt_s1ap);
    aclk_uart_printf("rx pkt s6a  count         : %"PRIu64"\n", g_decap_stat.decap_pkt_s6a);

    return;
}

int aclk_dpi_decap_init(void)
{
    memset(&g_decap_stat, 0x00, sizeof(aclk_dpi_decap_stat_t));

    return 0;
}

int aclk_dpi_decap_level(uint16_t protocol)
{
    int level;

    if (protocol < ACLK_DPI_PROTO_L3_START) {
        level = ACLK_DPI_PROTO_LEVEL_2;
    } else if (protocol < ACLK_DPI_PROTO_L4_START) {
        level = ACLK_DPI_PROTO_LEVEL_3;
    } else if (protocol < ACLK_DPI_PROTO_PAYLOAD) {
        level = ACLK_DPI_PROTO_LEVEL_4;
    } else {
        level = ACLK_DPI_PROTO_LEVEL_5;
    }

    return level;
}

int aclk_dpi_decap_process_packet(cvmx_wqe_t *packet)
{
    uint8_t *data = NULL;
    uint16_t offset, protocol, level, pkt_len;
    aclk_dpi_pkt_info_t *pkt;
    
    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return -1;
    }
    g_decap_stat.decap_pkt_num++;

    
    offset = 0;
    protocol = ACLK_DPI_PROTO_ETHER;
    pkt_len = aclk_dpi_wqe_get_len(packet);
    pkt = (aclk_dpi_pkt_info_t *)((uint8_t *)(packet->packet_data));
    data = (uint8_t *)(packet->packet_ptr);
    while (ACLK_DPI_PROTO_PAYLOAD != protocol) {
        level = aclk_dpi_decap_level(protocol);
        switch (level) {
            case ACLK_DPI_PROTO_LEVEL_2:
                if (aclk_dpi_decap_level_2(pkt, data, pkt_len, &offset, &protocol)) {
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_LEVEL_3:
                if (aclk_dpi_decap_level_3(pkt, data, pkt_len, &offset, &protocol)) {
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_LEVEL_4:
                if (aclk_dpi_decap_level_4(pkt, data, pkt_len, &offset, &protocol)) {
                    return -1;
                }
                break;
            default:
                g_decap_stat.decap_pkt_err++;
                return -1;
        }
        ///printf("protocol:%d\n", protocol);
    }

    return 0;
}

void aclk_dpi_decap_fini(void)
{
    return;
}
