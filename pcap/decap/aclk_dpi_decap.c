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

CVMX_SHARED aclk_dpi_decap_stat_t g_decap_stat[MAX_CORES];

void aclk_dpi_decap_stat(int argc, char *argv[])
{
    int i;
    aclk_dpi_decap_stat_t stat;

    if (argc == 1) {
        printf("Usage:\n");
        printf("    decap show\n");
        return;
    }

    if (0 == strcmp(argv[1], "show")) {       
        memset(&stat, 0x00, sizeof(stat));
        for (i = 0; i < MAX_CORES; i++) {
            stat.decap_pkt_num += g_decap_stat[i].decap_pkt_num;
            stat.decap_pkt_err += g_decap_stat[i].decap_pkt_err;

            stat.decap_pkt_vlan += g_decap_stat[i].decap_pkt_vlan;
            stat.decap_pkt_mpls += g_decap_stat[i].decap_pkt_mpls;
            stat.decap_pkt_arp += g_decap_stat[i].decap_pkt_arp; 
            stat.decap_pkt_ipv4 += g_decap_stat[i].decap_pkt_ipv4;
            stat.decap_pkt_ipv6 += g_decap_stat[i].decap_pkt_ipv6;
            stat.decap_pkt_pppoe += g_decap_stat[i].decap_pkt_pppoe; 
            stat.decap_pkt_ppp += g_decap_stat[i].decap_pkt_ppp;

            stat.decap_pkt_icmp += g_decap_stat[i].decap_pkt_icmp;
            stat.decap_pkt_igmp += g_decap_stat[i].decap_pkt_igmp;
            stat.decap_pkt_tcp += g_decap_stat[i].decap_pkt_tcp;
            stat.decap_pkt_udp += g_decap_stat[i].decap_pkt_udp;
            stat.decap_pkt_gre += g_decap_stat[i].decap_pkt_gre;
            stat.decap_pkt_ipsec_esp += g_decap_stat[i].decap_pkt_ipsec_esp;
            stat.decap_pkt_ipsec_ah += g_decap_stat[i].decap_pkt_ipsec_ah;
            stat.decap_pkt_ospf += g_decap_stat[i].decap_pkt_ospf;
            stat.decap_pkt_sctp += g_decap_stat[i].decap_pkt_sctp;
            stat.decap_pkt_frag += g_decap_stat[i].decap_pkt_frag;

            stat.decap_pkt_pptp += g_decap_stat[i].decap_pkt_pptp;

            stat.decap_pkt_dhcp += g_decap_stat[i].decap_pkt_dhcp;
            stat.decap_pkt_rip += g_decap_stat[i].decap_pkt_rip;
            stat.decap_pkt_gtp_ctrl += g_decap_stat[i].decap_pkt_gtp_ctrl;
            stat.decap_pkt_gtp_data += g_decap_stat[i].decap_pkt_gtp_data;
            stat.decap_pkt_l2tp_ctrl += g_decap_stat[i].decap_pkt_l2tp_ctrl;
            stat.decap_pkt_l2tp_data += g_decap_stat[i].decap_pkt_l2tp_data;
            stat.decap_pkt_openvpn += g_decap_stat[i].decap_pkt_openvpn;
            stat.decap_pkt_teredo += g_decap_stat[i].decap_pkt_teredo;
            stat.decap_pkt_s1ap += g_decap_stat[i].decap_pkt_s1ap;
            stat.decap_pkt_s6a += g_decap_stat[i].decap_pkt_s6a;
        }

#ifdef __OCTEON_SDK__
        if (!g_shell_cmd_flag) {
#endif
            aclk_uart_printf("============= decap rx pkt count=============\n");
            aclk_uart_printf("rx pkt count              : %"PRIu64"\n", stat.decap_pkt_num);
            aclk_uart_printf("rx pkt err count          : %"PRIu64"\n", stat.decap_pkt_err);

            aclk_uart_printf("rx pkt vlan count         : %"PRIu64"\n", stat.decap_pkt_vlan);
            aclk_uart_printf("rx pkt mpls count         : %"PRIu64"\n", stat.decap_pkt_mpls);
            aclk_uart_printf("rx pkt arp count          : %"PRIu64"\n", stat.decap_pkt_arp); 
            aclk_uart_printf("rx pkt ipv4  count        : %"PRIu64"\n", stat.decap_pkt_ipv4);
            aclk_uart_printf("rx pkt ipv6 count         : %"PRIu64"\n", stat.decap_pkt_ipv6);
            aclk_uart_printf("rx pkt pppoe count        : %"PRIu64"\n", stat.decap_pkt_pppoe); 
            aclk_uart_printf("rx pkt ppp  count         : %"PRIu64"\n", stat.decap_pkt_ppp);

            aclk_uart_printf("rx pkt icmp  count        : %"PRIu64"\n", stat.decap_pkt_icmp);
            aclk_uart_printf("rx pkt igmp  count        : %"PRIu64"\n", stat.decap_pkt_igmp);
            aclk_uart_printf("rx pkt tcp count          : %"PRIu64"\n", stat.decap_pkt_tcp);
            aclk_uart_printf("rx pkt udp count          : %"PRIu64"\n", stat.decap_pkt_udp);
            aclk_uart_printf("rx pkt gre  count         : %"PRIu64"\n", stat.decap_pkt_gre);
            aclk_uart_printf("rx pkt ipsec esp count    : %"PRIu64"\n", stat.decap_pkt_ipsec_esp);
            aclk_uart_printf("rx pkt ipsec ah count     : %"PRIu64"\n", stat.decap_pkt_ipsec_ah);
            aclk_uart_printf("rx pkt ospf count         : %"PRIu64"\n", stat.decap_pkt_ospf);
            aclk_uart_printf("rx pkt sctp  count        : %"PRIu64"\n", stat.decap_pkt_sctp);
            aclk_uart_printf("rx pkt frag count         : %"PRIu64"\n", stat.decap_pkt_frag);

            aclk_uart_printf("rx pkt pptp count         : %"PRIu64"\n", stat.decap_pkt_pptp);

            aclk_uart_printf("rx pkt dhcp  count        : %"PRIu64"\n", stat.decap_pkt_dhcp);
            aclk_uart_printf("rx pkt rip  count         : %"PRIu64"\n", stat.decap_pkt_rip);
            aclk_uart_printf("rx pkt gtp ctrl count     : %"PRIu64"\n", stat.decap_pkt_gtp_ctrl);
            aclk_uart_printf("rx pkt gtp data count     : %"PRIu64"\n", stat.decap_pkt_gtp_data);
            aclk_uart_printf("rx pkt l2tp ctrl count    : %"PRIu64"\n", stat.decap_pkt_l2tp_ctrl);
            aclk_uart_printf("rx pkt l2tp data  count   : %"PRIu64"\n", stat.decap_pkt_l2tp_data);
            aclk_uart_printf("rx pkt openvpn count      : %"PRIu64"\n", stat.decap_pkt_openvpn);
            aclk_uart_printf("rx pkt teredo count       : %"PRIu64"\n", stat.decap_pkt_teredo);
            aclk_uart_printf("rx pkt s1ap  count        : %"PRIu64"\n", stat.decap_pkt_s1ap);
            aclk_uart_printf("rx pkt s6a  count         : %"PRIu64"\n", stat.decap_pkt_s6a);
#ifdef __OCTEON_SDK__
        }else {        
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "============= decap rx pkt count=============\r\n");
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt count              : %"PRIu64"\r\n", stat.decap_pkt_num);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt err count          : %"PRIu64"\r\n", stat.decap_pkt_err);

            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt vlan count         : %"PRIu64"\r\n", stat.decap_pkt_vlan);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt mpls count         : %"PRIu64"\r\n", stat.decap_pkt_mpls);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt arp count          : %"PRIu64"\r\n", stat.decap_pkt_arp); 
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt ipv4  count        : %"PRIu64"\r\n", stat.decap_pkt_ipv4);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt ipv6 count         : %"PRIu64"\r\n", stat.decap_pkt_ipv6);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt pppoe count        : %"PRIu64"\r\n", stat.decap_pkt_pppoe); 
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt ppp  count         : %"PRIu64"\r\n", stat.decap_pkt_ppp);

            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt icmp  count        : %"PRIu64"\r\n", stat.decap_pkt_icmp);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt igmp  count        : %"PRIu64"\r\n", stat.decap_pkt_igmp);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt tcp count          : %"PRIu64"\r\n", stat.decap_pkt_tcp);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt udp count          : %"PRIu64"\r\n", stat.decap_pkt_udp);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt gre  count         : %"PRIu64"\r\n", stat.decap_pkt_gre);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt ipsec esp count    : %"PRIu64"\r\n", stat.decap_pkt_ipsec_esp);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt ipsec ah count     : %"PRIu64"\r\n", stat.decap_pkt_ipsec_ah);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt ospf count         : %"PRIu64"\r\n", stat.decap_pkt_ospf);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt sctp  count        : %"PRIu64"\r\n", stat.decap_pkt_sctp);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt frag count         : %"PRIu64"\r\n", stat.decap_pkt_frag);

            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt pptp count         : %"PRIu64"\r\n", stat.decap_pkt_pptp);

            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt dhcp  count        : %"PRIu64"\r\n", stat.decap_pkt_dhcp);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt rip  count         : %"PRIu64"\r\n", stat.decap_pkt_rip);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt gtp ctrl count     : %"PRIu64"\r\n", stat.decap_pkt_gtp_ctrl);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt gtp data count     : %"PRIu64"\r\n", stat.decap_pkt_gtp_data);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt l2tp ctrl count    : %"PRIu64"\r\n", stat.decap_pkt_l2tp_ctrl);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt l2tp data  count   : %"PRIu64"\r\n", stat.decap_pkt_l2tp_data);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt openvpn count      : %"PRIu64"\r\n", stat.decap_pkt_openvpn);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt teredo count       : %"PRIu64"\r\n", stat.decap_pkt_teredo);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt s1ap  count        : %"PRIu64"\r\n", stat.decap_pkt_s1ap);
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt s6a  count         : %"PRIu64"\r\n", stat.decap_pkt_s6a);
        }
#endif
    }else if (0 == strcmp(argv[1], "clear")) {
        memset(g_decap_stat, 0x00, sizeof(g_decap_stat)); 
#ifdef __OCTEON_SDK__
        if (!g_shell_cmd_flag) {
#endif
            aclk_uart_printf("clear decap stat ok!\n");
#ifdef __OCTEON_SDK__
        }
#endif
    } else {               
#ifdef __OCTEON_SDK__
        if (!g_shell_cmd_flag) {
#endif
            aclk_uart_printf("%s[%d]:Invalid command\n", __func__, __LINE__);
#ifdef __OCTEON_SDK__
        } else {
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "Invalid command\r\n");
        }
#endif
    }

    return;
}

int aclk_dpi_decap_init_local(void)
{
    return 0;
}

int aclk_dpi_decap_init_global(void)
{
    ///init count
    memset(g_decap_stat, 0x00, sizeof(g_decap_stat));

#ifdef __OCTEON_SDK__
    //register rx count state
    aclk_shell_cmd_register("decap", "show decap count command", aclk_dpi_decap_stat);
#endif

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
    g_decap_stat[g_local_core_id].decap_pkt_num++;

    
    offset = 0;
    protocol = ACLK_DPI_PROTO_ETHER;
    pkt_len = cvmx_wqe_get_len(packet);
    pkt = (aclk_dpi_pkt_info_t *)((uint8_t *)(packet->packet_data));
    data = (uint8_t *)cvmx_phys_to_ptr(packet->packet_ptr.s.addr);
    while (ACLK_DPI_PROTO_PAYLOAD != protocol) {
        level = aclk_dpi_decap_level(protocol);
        printf("level:%d, proto:0x%0x\n", level, protocol);
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
                g_decap_stat[g_local_core_id].decap_pkt_err++;
                return -1;
        }
        ///printf("protocol:%d\n", protocol);
    }

    return 0;
}

int aclk_dpi_decap_process_command(void *packet, void *data)
{
    return 0;
}

void aclk_dpi_decap_fini_local(void)
{
    return;
}

void aclk_dpi_decap_fini_global(void)
{
    return;
}
