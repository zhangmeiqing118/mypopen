/**
 * @File Name: aclk_dpi_decap.c
 * @brief: 
 * @Author: zhangmeiqing
 * @Version: 1.0.0
 * @Date: Tue 03 May 2016 02:28:14 PM CST
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#include "aclk_dpi_decap.h"


CVMX_SHARED int g_log_level = ACLK_DPI_LOG_LEVEL_ERROR;
CVMX_SHARED int g_shell_cmd_flag = 0;
CVMX_SHARED int g_command_response_len = 0;
CVMX_SHARED char *g_command_response_buffer;

extern int g_write_flag;

static int g_local_core_id = 0;
CVMX_SHARED rx_pkt_count_t g_rx_pkt_count[MAX_CORES];

int cvmx_wqe_get_len(cvmx_wqe_t *wqe)
{
    return wqe->word1.len;
}

void aclk_dpi_process_packet_over(void *packet)
{
    //cvmx_wqe_t *wqe;
    //aclk_dpi_pkt_info_t *pkt;
    //     
    //if (NULL == packet) {
    //    aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
    //    return ;
    //}   

    /////set pide module callback
    /////get packet info
    //wqe = (cvmx_wqe_t *)packet;
    //pkt = (aclk_dpi_pkt_info_t *)(wqe->packet_data);
    /////update pide app stat
    /////aclk_pide_flow_timeout_stat(pkt->appidx, 1, cvmx_wqe_get_len(wqe));
    //if (pkt->callback) {
    //    pkt->callback(packet);
    //} else {
    //    ///aclk_hal_packet_free(packet, 1); 
    //}   

    return;
}

//int  aclk_printf(int level, char *fmt,  ... )
//{
//    char buffer[1024];
//    va_list args;
//    int recode;
//    char *ptr, *str;
//
//    recode = 0;
//    if (g_log_level >= level) {
//        switch (level) {
//            case ACLK_DPI_LOG_LEVEL_ERROR:
//                str = "ERROR";
//                break;
//            case ACLK_DPI_LOG_LEVEL_WARN:
//                str = "WARN";
//                break;
//            case ACLK_DPI_LOG_LEVEL_DEBUG:
//                str = "DEBUG";
//                break;
//            case ACLK_DPI_LOG_LEVEL_INFO:
//            default:
//                str = "INFO";
//                break;
//        }
//        va_start(args, format);
//        recode = vsnprintf(buffer, sizeof(buffer), "%s"format, str, args);
//        va_end(args);
//        printf("%s", buffer);
//    }
//
//    return recode;
//}


int aclk_rx_pkt_count_init(void)
{
    memset(g_rx_pkt_count, 0x00, sizeof(g_rx_pkt_count));

    return 0;
}

 int64_t aclk_get_rx_pkt_num()
{
    int64_t count_commit = 0 ;      
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_err_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_err_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_frag_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_frag_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_ipv6_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_ipv6_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_ip_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_ipv4_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_arp_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_arp_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_pppoe_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_pppoe_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_ppp_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_ppp_num;
    }

    return count_commit;
}


int64_t aclk_get_rx_pkt_tcp_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_tcp_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_udp_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_udp_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_gre_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_gre_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_gtp_ctrl_num()
{
    int64_t count_commit = 0;
    int32_t i = 0;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_gtp_ctrl_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_gtp_data_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_gtp_data_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_l2tp_ctrl_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_l2tp_ctrl_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_l2tp_data_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_l2tp_data_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_sctp_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_sctp_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_s1ap_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_s1ap_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_icmp_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;
    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_icmp_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_igmp_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;
    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_igmp_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_dhcp_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;
    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_dhcp_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_pptp_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_pptp_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_openvpn_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_openvpn_num;
    }

    return count_commit;
}

int64_t aclk_get_rx_pkt_ipsec_esp_num()
{
    int64_t count_commit = 0 ;
    int32_t i = 0 ;

    for (i = 0; i < MAX_CORES; i++) {
        count_commit += g_rx_pkt_count[i].rx_pkt_ipsec_esp_num;
    }

    return count_commit;
}

void aclk_decap_cmd_rxstate(int argc, char *argv[])
{
    if (argc == 1) {
        printf("Usage:\n");
        printf("    decap show\n");
        return;
    } 
    if (0 == strcmp(argv[1], "show")) {       
        if (!g_shell_cmd_flag) {
            aclk_uart_printf("decap rx pkt count:\n");
            aclk_uart_printf("rx pkt num count\t\t: %ld\n", aclk_get_rx_pkt_num());
            aclk_uart_printf("rx pkt err num count\t\t: %ld\n", aclk_get_rx_pkt_err_num());        
            aclk_uart_printf("rx pkt frag num count\t\t: %ld\n", aclk_get_rx_pkt_frag_num());        
            aclk_uart_printf("rx pkt arp num, count\t\t: %ld\n", aclk_get_rx_pkt_arp_num());
            aclk_uart_printf("rx pkt pppoe num, count\t\t: %ld\n", aclk_get_rx_pkt_pppoe_num()); 
            aclk_uart_printf("rx pkt ppp num, count\t\t: %ld\n", aclk_get_rx_pkt_ppp_num()); 
            aclk_uart_printf("rx pkt ipv4 num, count\t\t: %ld\n", aclk_get_rx_pkt_ip_num());
            aclk_uart_printf("rx pkt ipv6 num count\t\t: %ld\n", aclk_get_rx_pkt_ipv6_num());
            aclk_uart_printf("rx pkt udp num, count\t\t: %ld\n", aclk_get_rx_pkt_udp_num());   
            aclk_uart_printf("rx pkt tcp num, count\t\t: %ld\n", aclk_get_rx_pkt_tcp_num());    
            aclk_uart_printf("rx pkt icmp num, count\t\t: %ld\n", aclk_get_rx_pkt_icmp_num());        
            aclk_uart_printf("rx pkt igmp num, count\t\t: %ld\n", aclk_get_rx_pkt_igmp_num());        
            aclk_uart_printf("rx pkt dhcp num, count\t\t: %ld\n", aclk_get_rx_pkt_dhcp_num());        
            aclk_uart_printf("rx pkt gre num, count\t\t: %ld\n", aclk_get_rx_pkt_gre_num());
            aclk_uart_printf("rx pkt sctp num, count\t\t: %ld\n", aclk_get_rx_pkt_sctp_num());
            aclk_uart_printf("rx pkt s1ap num, count\t\t: %ld\n", aclk_get_rx_pkt_s1ap_num());
            aclk_uart_printf("rx pkt gtp data num count\t: %ld\n", aclk_get_rx_pkt_gtp_data_num());
            aclk_uart_printf("rx pkt gtp ctrl num count\t: %ld\n", aclk_get_rx_pkt_gtp_ctrl_num());
            aclk_uart_printf("rx pkt l2tp data num count\t: %ld\n", aclk_get_rx_pkt_l2tp_data_num());
            aclk_uart_printf("rx pkt l2tp ctrl num count\t: %ld\n", aclk_get_rx_pkt_l2tp_ctrl_num());       
            aclk_uart_printf("rx pkt pptp num, count\t\t: %ld\n", aclk_get_rx_pkt_pptp_num()); 
            aclk_uart_printf("rx pkt openvpn num, count\t: %ld\n", aclk_get_rx_pkt_openvpn_num()); 
            aclk_uart_printf("rx pkt ipsec esp num, count\t: %ld\n", aclk_get_rx_pkt_ipsec_esp_num()); 
        }else {        
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "decap rx pkt count:\r\n");   
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt num count\t\t: %ld\r\n", aclk_get_rx_pkt_num());            
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt err num count\t\t: %ld\r\n", aclk_get_rx_pkt_err_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt frag num count\t\t: %ld\r\n", aclk_get_rx_pkt_frag_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt arp num, count\t\t: %ld\r\n", aclk_get_rx_pkt_arp_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt pppoe num, count\t\t: %ld\r\n", aclk_get_rx_pkt_pppoe_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt ppp num, count\t\t: %ld\r\n", aclk_get_rx_pkt_ppp_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt ipv4 num count\t\t: %ld\r\n", aclk_get_rx_pkt_ip_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt ipv6 num count\t\t: %ld\r\n", aclk_get_rx_pkt_ipv6_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt udp num count\t\t: %ld\r\n", aclk_get_rx_pkt_udp_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt tcp num count\t\t: %ld\r\n", aclk_get_rx_pkt_tcp_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt icmp num count\t\t: %ld\r\n", aclk_get_rx_pkt_icmp_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt igmp num count\t\t: %ld\r\n", aclk_get_rx_pkt_igmp_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt dhcp num count\t\t: %ld\r\n", aclk_get_rx_pkt_dhcp_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt gre num count\t\t: %ld\r\n", aclk_get_rx_pkt_gre_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt sctp num count\t\t: %ld\r\n", aclk_get_rx_pkt_sctp_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt s1ap num count\t\t: %ld\r\n", aclk_get_rx_pkt_s1ap_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt gtp data num count\t: %ld\r\n", aclk_get_rx_pkt_gtp_data_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt gtp ctrl num count\t: %ld\r\n", aclk_get_rx_pkt_gtp_ctrl_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt l2tp data num count\t: %ld\r\n", aclk_get_rx_pkt_l2tp_data_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt l2tp ctrl num count\t: %ld\r\n", aclk_get_rx_pkt_l2tp_ctrl_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt pptp num count\t\t: %ld\r\n", aclk_get_rx_pkt_pptp_num());
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt openvpn num, count\t\t: %ld\n", aclk_get_rx_pkt_openvpn_num()); 
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "rx pkt ipsec esp num, count\t\t: %ld\n", aclk_get_rx_pkt_ipsec_esp_num()); 
        }
    }else if (0 == strcmp(argv[1], "clear")) {
        memset(g_rx_pkt_count, 0x00, sizeof(g_rx_pkt_count)); 
        if (!g_shell_cmd_flag) {
            aclk_uart_printf("clear decap stat ok!\n");
        }
    } else {               
        if (!g_shell_cmd_flag) {
            aclk_uart_printf("%s[%d]:Invalid command\n", __func__, __LINE__);
        } else {
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "Invalid command\r\n");
        }
    }

    return ;
}

//int aclk_decap_init_local(void)
//{
//    g_local_core_id = cvmx_get_core_num();
//
//    return 0;
//}

int aclk_decap_init_global(void)
{
    ///init count
    if (aclk_rx_pkt_count_init()) {
        aclk_uart_printf("%s[%d]: rx pkt count init error\n", __func__, __LINE__);
        return -1;
    }
    g_log_level = ACLK_DPI_LOG_LEVEL_ERROR;
    ///g_command_response_buffer = (char *)cvmx_bootmem_alloc_named(ACLK_COMMAND_BUF_SIZE, CVMX_CACHE_LINE_SIZE, "command_buf");
    g_command_response_buffer = (char *)malloc(ACLK_COMMAND_BUF_SIZE);
    if (NULL == g_command_response_buffer) {
        printf("%s[%d]:malloc for command buf error\n", __func__, __LINE__);
        return -1;
    }
    memset(g_command_response_buffer, 0x00, ACLK_COMMAND_BUF_SIZE);

    //register rx count state
    ///aclk_shell_cmd_register("decap", "show decap count command", aclk_decap_cmd_rxstate);

    return 0;
}

static int aclk_dpi_decap_ether(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    eth_header_t *ether;
    uint16_t protocol;
    ///if need decap arp ,decap here

    if (len < (sizeof(eth_header_t) + *offset)) {
        return -1;
    }

    ether = (eth_header_t *)(data + *offset);
#if __BYTE_ORDER == __LITTLE_ENDIAN
    protocol = ntohs(ether->protocol);
#else 
    protocol = ether->protocol;
#endif
    ///printf("ether proto:%02x, proto:%02x\n", ether->protocol, protocol);
    *offset += sizeof(eth_header_t);
    switch (protocol) {  //define in linux/if_ether.h
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

static int aclk_dpi_decap_arp(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    ///if need decap arp ,decap here

    g_rx_pkt_count[g_local_core_id].rx_pkt_arp_num++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    return 0;
}

static int aclk_dpi_decap_vlan(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    vlan_header_t *vlan;
    uint16_t protocol;

    do {
        if (len < (sizeof(vlan_header_t) + *offset)) {
            return -1;
        }

        vlan = (vlan_header_t *)(data + *offset);
#if __BYTE_ORDER == __LITTLE_ENDIAN
        protocol = ntohs(vlan->protocol);
#else
        protocol = vlan->protocol;
#endif

        *offset += sizeof(vlan_header_t);
        ///*proto = vlan->protocol;
    } while ((protocol == 0x8100) || (protocol == 0x88A8) || (protocol == 0x9100) || (protocol == 0x9200) || (protocol == 0x9300) );

    switch (protocol) {  //define in linux/if_ether.h
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

static int aclk_dpi_decap_mpls(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    mpls_header_t *mpls;
    ipv4_header_t *ipv4;

    do {
        if (len < (sizeof(mpls_header_t) + *offset)) {
            return -1;
        }

        mpls = (mpls_header_t *)(data + *offset);
        *offset += sizeof(mpls_header_t);
    } while (!(mpls->s));
    
    if (len < (sizeof(ipv4_header_t)) + *offset) {
        return -1;
    }
    ipv4 = (ipv4_header_t *)(data + *offset);
    if (4 == ipv4->version) {
        *proto = ACLK_DPI_PROTO_IPV4;
    } else if (6 == ipv4->version) {
        *proto = ACLK_DPI_PROTO_IPV6;
    } else {
        return -1;
    }

    return 0;
}

static int aclk_dpi_decap_pppoe(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    pppoe_header_t *pppoe;
    uint16_t protocol;

    if (len < (sizeof(pppoe_header_t) + *offset)) {
        return -1;
    }
    pppoe = (pppoe_header_t *)(data + *offset);
    *offset += sizeof(pppoe_header_t);
#if __BYTE_ORDER ==  __LITTLE_ENDIAN
    protocol = ntohs(*((uint16_t *)(data + *offset)));
#else
    protocol = *((uint16_t *)(data + *offset));
#endif

    ///printf("proto:%02x\n", protocol);
    if ((0xc021 == protocol) || (0x8021 == protocol)
            || (0x8057 == protocol) || (0xc023 == protocol)) {
        *proto = ACLK_DPI_PROTO_PPP;
        return 0;
    } else{
        g_rx_pkt_count[g_local_core_id].rx_pkt_pppoe_num++;
        *proto = ACLK_DPI_PROTO_PAYLOAD;
        return -1;
    }
    
}

static int aclk_dpi_decap_icmp(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data,  uint16_t *offset, uint16_t *proto)
{
    ///if need decap icmp ,decap here

    g_rx_pkt_count[g_local_core_id].rx_pkt_icmp_num++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    return 0;
}

static int aclk_dpi_decap_igmp(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data,  uint16_t *offset, uint16_t *proto)
{
    ///if need decap icmp ,decap here

    g_rx_pkt_count[g_local_core_id].rx_pkt_igmp_num++;
    *proto = ACLK_DPI_PROTO_PAYLOAD;
    
    return 0;
}

static int aclk_dpi_decap_ipv4(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    ipv4_header_t *ipv4;
    uint16_t frag_offset;
    uint8_t more_flag, donot_flag;

    if (len < (sizeof(ipv4_header_t) + *offset)) {
        return -1;
    }
    ipv4 = (ipv4_header_t *)(data + *offset);
    if (4 != ipv4->version) {
        return -1;
    }
    
    //ipv4 packet
    if (0 == pkt->ip_ver) {
        g_rx_pkt_count[g_local_core_id].rx_pkt_ipv4_num++;
    } else if (6 == pkt->ip_ver) {
        g_rx_pkt_count[g_local_core_id].rx_pkt_ipv6_num--;
        g_rx_pkt_count[g_local_core_id].rx_pkt_ipv4_num++;
    }
    
#if __BYTE_ORDER == __LITTLE_ENDIAN
    frag_offset = ntohs(ipv4->frag_offset);
#else
    frag_offset = ipv4->frag_offset;
#endif
    more_flag = (frag_offset >> 13) & 0x01;
    frag_offset = frag_offset & 0x1ff;
    ///printf("ver:%d, hdr len:%d, ipv4->frag_offset:%d\n", ipv4->version, ipv4->header_length, frag_offset);
    ///printf("more:%d\n", more_flag);
    if (more_flag || frag_offset) {//ip fragment
        *proto = ACLK_DPI_PROTO_FRAG;
        return 0;
    }
    
    pkt->ip_ver = 4;
    pkt->l4_proto = ipv4->protocol;
    pkt->sip.ipv4= ipv4->src_ip;
    pkt->dip.ipv4 = ipv4->dst_ip;
    *offset = *offset + ipv4->header_length * 4;
    ////printf("decp ipv4 protocol:%d, len:%d\n", ipv4->protocol, ipv4->header_length);
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

static int aclk_dpi_decap_ipv6(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    ipv6_header_t *ipv6;
    uint32_t version;

    if (len < (sizeof(ipv6_header_t) + *offset)) {
        return -1;
    }

    ipv6 = (ipv6_header_t *)(data + *offset);
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
        g_rx_pkt_count[g_local_core_id].rx_pkt_ipv6_num++;
    } else if (4 == pkt->ip_ver) {
        g_rx_pkt_count[g_local_core_id].rx_pkt_ipv4_num--;
        g_rx_pkt_count[g_local_core_id].rx_pkt_ipv6_num++;
    }

    pkt->ip_ver = 6;
    pkt->l4_proto= ipv6->ip6_ctlun.ip6_un1.ip6_un1_nxt;           
    memcpy(pkt->sip.ipv6, ipv6->ip6_src.__u6_addr.__u6_addr16, 16);
    memcpy(pkt->dip.ipv6, ipv6->ip6_dst.__u6_addr.__u6_addr16, 16);
    *offset += sizeof(ipv6_header_t);
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

static int aclk_dpi_decap_ipsec_ah(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    uint8_t *l4_proto;

    l4_proto = (uint8_t *)(data + *offset);
    
    if (len < *offset + 16) {
        return -1;
    }

    *offset = *offset + 16; ///ah header length:16
    //printf("decp ipv4 protocol:%d, len:%d\n", ipv4->protocol, ipv4->header_length);
    switch (*l4_proto) {
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
    //printf("decp ah proto:%d\n", *proto);
    
    return 0;
}

static int aclk_dpi_decap_ipv6_frag(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    ipv6_frag_t *frag;

    if (len < (sizeof(ipv6_frag_t) + *offset)) {
        return -1;
    }

    frag = (ipv6_frag_t *)(data + *offset);
    *offset += sizeof(ipv6_frag_t);
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

    return 0;
}

static int aclk_dpi_decap_tcp(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    uint8_t tcp_flag;
    tcp_header_t *tcp;
    
    if (len < (sizeof(tcp_header_t) + *offset)) {
        return -1;
    }
    
    if (0 == pkt->sport) {
        g_rx_pkt_count[g_local_core_id].rx_pkt_tcp_num++;
    } else if ((0x11 == pkt->l4_proto) && (pkt->sport)) {
        g_rx_pkt_count[g_local_core_id].rx_pkt_udp_num--;
        g_rx_pkt_count[g_local_core_id].rx_pkt_tcp_num++;
    }

    tcp = (tcp_header_t *)(data + *offset);
    pkt->l4_proto = 0x06;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    pkt->dport = ntohs(tcp->dst_port);
    pkt->sport = ntohs(tcp->src_port);
#else
    pkt->dport = tcp->dst_port;
    pkt->sport = tcp->src_port;
#endif

    ///set tcp_flag;
    tcp_flag = *((uint8_t *)(data + *offset + 16));
    aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: tcp_flag:%d\n", __func__, __LINE__, tcp_flag);
    switch (tcp_flag){
        case 0x01: //fin
            pkt->tcp_flag = TCP_FLAG_FIN;
            break;
        case 0x02: //syn
            pkt->tcp_flag = TCP_FLAG_SYN;
            break;
        case 0x04: //reset
            pkt->tcp_flag = TCP_FLAG_RST;
            break;
        case 0x10: //ack
            pkt->tcp_flag = TCP_FLAG_ACK;
            break;
        case 0x11: //fin ack
            pkt->tcp_flag = TCP_FLAG_FIN_ACK;
            break;
        case 0x12: //syn ack
            pkt->tcp_flag = TCP_FLAG_SYN_ACK;
            break;
        case 0x14: //rst ack
            pkt->tcp_flag = TCP_FLAG_RST_ACK;
            break;
        case 0x18: //psh ack
            pkt->tcp_flag = TCP_FLAG_PSH_ACK;
            break;
    }

    *offset += tcp->doff * 4;
    if (ACLK_DPI_PPTP_PORT == pkt->dport || ACLK_DPI_PPTP_PORT == pkt->sport) {       
        *proto = ACLK_DPI_PROTO_PPTP;        
    } else {   
        *proto = ACLK_DPI_PROTO_PAYLOAD;
    }

    return 0;
}

static int aclk_dpi_decap_udp(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    int flag;
    udp_header_t *udp;
    
    if (len < (sizeof(udp_header_t) + *offset)) {
        return -1;
    }
   
    if (0 == pkt->sport) {
        g_rx_pkt_count[g_local_core_id].rx_pkt_udp_num++;
    } else if ((0x06 == pkt->l4_proto) && (pkt->sport)) {
        g_rx_pkt_count[g_local_core_id].rx_pkt_tcp_num--;
        g_rx_pkt_count[g_local_core_id].rx_pkt_udp_num++;
    }

    udp = (udp_header_t *)(data + *offset);
    pkt->l4_proto = 0x11;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    pkt->dport = ntohs(udp->dst_port);
    pkt->sport = ntohs(udp->src_port);
#else
    pkt->dport = udp->dst_port;
    pkt->sport = udp->src_port;
#endif
    *offset += sizeof(udp_header_t);

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

static int aclk_dpi_decap_gre(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    gre_header_t *gre;

    if (len < (sizeof(gre_header_t) + *offset)) {
        return -1;
    }
    g_rx_pkt_count[g_local_core_id].rx_pkt_gre_num++;
    gre = (gre_header_t *)(data + *offset); 
#if __BYTE_ORDER == __LITTLE_ENDIAN
    pkt->l4_proto = ntohs(gre->protocol);
#else
    pkt->l4_proto = gre->protocol;
#endif
    *offset += sizeof(gre_header_t);

    return 0;
}

static int aclk_dpi_decap_sctp(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    int flag;
    sctp_data_t *sctp_data;
    uint32_t next_proto;

    if (len < (sizeof(sctp_header_t) + *offset)) {
        return -1;
    }
    *offset += sizeof(sctp_header_t);
    if (len < sizeof(sctp_data_t) + *offset) {
        return -1;
    }
    g_rx_pkt_count[g_local_core_id].rx_pkt_sctp_num++;

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

static int aclk_dpi_decap_s1ap(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{    
    g_rx_pkt_count[g_local_core_id].rx_pkt_s1ap_num++;        
    return -1;
}

static int aclk_dpi_decap_gtp(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    ipv4_header_t *ipv4;
    gtp_r99_header_t *gtp;
    
    if (len < (sizeof(gtp_r99_header_t) + *offset)) {
        return -1;
    }

    gtp = (gtp_r99_header_t *)(data + *offset);
    if (0xff == gtp->message_type) {///gtp data
        g_rx_pkt_count[g_local_core_id].rx_pkt_gtp_data_num++;                      
        if ((0 == gtp->exhdr_flag) && (0 == gtp->seq_flag) && (0 == gtp->npdu_flag)) {
            *offset += sizeof(struct gtp_r99_header_s);
        } else {           
            *offset += sizeof(struct gtp_r99_exheader_s);              
        }

        if (len < (sizeof(ipv4_header_t)) + *offset) {
            return -1;
        }
        ipv4 = (ipv4_header_t *)(data + *offset);
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
        g_rx_pkt_count[g_local_core_id].rx_pkt_gtp_ctrl_num++;                      
        
        return -1;
    }

    return 0;
}

static int aclk_dpi_decap_l2tp(aclk_dpi_pkt_info_t *pkt, uint16_t len, uint8_t *data, uint16_t *offset, uint16_t *proto)
{
    l2tp_pkt_type_t *l2tp;
    
    if (len < (sizeof(l2tp_pkt_type_t) + *offset)) {
        return -1;
    }

    l2tp = (l2tp_pkt_type_t *)(data + *offset);
    if (0 == l2tp->msg_type) {
        g_rx_pkt_count[g_local_core_id].rx_pkt_l2tp_data_num++;
    } else {
        g_rx_pkt_count[g_local_core_id].rx_pkt_l2tp_ctrl_num++;
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

    //    g_rx_pkt_count[g_local_core_id].rx_pkt_l2tp_data_num++;
    //} else {
    //    //l2tp ctrl
    //    g_rx_pkt_count[g_local_core_id].rx_pkt_l2tp_ctrl_num++;
    //    aclk_dpi_process_packet_over(packet);
    //    return -1;
    //}

    return -1;
}

int aclk_decap_process_packet(cvmx_wqe_t *packet)
{
    aclk_dpi_pkt_info_t *pkt;
    uint8_t *data = NULL;
    uint16_t protocol, pkt_len;
    uint16_t offset = 0;  

    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return -1;
    }
    g_rx_pkt_count[g_local_core_id].rx_pkt_num++;

    offset = 0;
    pkt_len = cvmx_wqe_get_len(packet);
    pkt = (aclk_dpi_pkt_info_t *)((uint8_t *)(packet->packet_data));

    ///data = (uint8_t *)cvmx_phys_to_ptr(packet->packet_ptr.s.addr);
    data = (uint8_t *)(packet->packet_ptr);
    protocol = ACLK_DPI_PROTO_ETHER;
    while (protocol != ACLK_DPI_PROTO_PAYLOAD) {
        switch (protocol) {
            case ACLK_DPI_PROTO_ETHER:
                if (aclk_dpi_decap_ether(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_ARP:
                pkt->appidx = ACLK_PIDE_PROTO_ARP;
                if (aclk_dpi_decap_arp(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                ///for arp packet, process over
                aclk_dpi_process_packet_over(packet);
                return -1;
            case ACLK_DPI_PROTO_VLAN:
                if (aclk_dpi_decap_vlan(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_MPLS:
                if (aclk_dpi_decap_mpls(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_PPPOE:
                if (aclk_dpi_decap_pppoe(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_PPP:
                pkt->appidx = ACLK_PIDE_PROTO_PPP;
                g_rx_pkt_count[g_local_core_id].rx_pkt_ppp_num++;
                ///for dhcp packet, process over
                aclk_dpi_process_packet_over(packet);
               return 0;
            case ACLK_DPI_PROTO_IPV4:
                if (aclk_dpi_decap_ipv4(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_IPV6:
                if (aclk_dpi_decap_ipv6(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_ICMP:
                pkt->appidx = ACLK_PIDE_PROTO_ICMP;
                if (aclk_dpi_decap_icmp(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                ///for icmp packet, process over
                aclk_dpi_process_packet_over(packet);
                return -1;
            case ACLK_DPI_PROTO_IGMP:
                pkt->appidx = ACLK_PIDE_PROTO_IGMP;
                if (aclk_dpi_decap_igmp(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                ///for igmp packet, process over
                aclk_dpi_process_packet_over(packet);
                return -1;
            case ACLK_DPI_PROTO_TCP:
                if (aclk_dpi_decap_tcp(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_UDP:
                if (aclk_dpi_decap_udp(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_GRE:
                pkt->appidx = ACLK_PIDE_PROTO_GRE;
                if (aclk_dpi_decap_gre(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                ///for gre packet, process over
                aclk_dpi_process_packet_over(packet);
                return -1;
            case ACLK_DPI_PROTO_DHCP:
                pkt->appidx = ACLK_PIDE_PROTO_DHCP;
                g_rx_pkt_count[g_local_core_id].rx_pkt_dhcp_num++;
                ///for dhcp packet, process over
                ///aclk_dpi_process_packet_over(packet);
               return 0;
            case ACLK_DPI_PROTO_RIP:
                pkt->appidx = ACLK_PIDE_PROTO_RIP;
                ///for dhcp packet, process over
                aclk_dpi_process_packet_over(packet);
                return -1;
            case ACLK_DPI_PROTO_FRAG:
                g_rx_pkt_count[g_local_core_id].rx_pkt_frag_num++; 
                if (4 == pkt->version) {
                    pkt->appidx = ACLK_PIDE_PROTO_FRAG;
                    ///for gre packet, process over
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                } else {
                    if (aclk_dpi_decap_ipv6_frag(pkt, pkt_len, data, &offset, &protocol)) {
                        g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                        aclk_dpi_process_packet_over(packet);
                        return -1;
                    }
                }
            case ACLK_DPI_PROTO_OSPF:
                pkt->appidx = ACLK_PIDE_PROTO_OSPF;
                ///for gre packet, process over
                aclk_dpi_process_packet_over(packet);
                return -1;
            case ACLK_DPI_PROTO_IPSEC_ESP:
                pkt->appidx = ACLK_PIDE_PROTO_IPSEC_ESP;
                g_rx_pkt_count[g_local_core_id].rx_pkt_ipsec_esp_num++; 
                ///for gre packet, process over
                aclk_dpi_process_packet_over(packet);
                return -1;
            case ACLK_DPI_PROTO_IPSEC_AH:
                pkt->appidx = ACLK_PIDE_PROTO_IPSEC_AH;
                if (aclk_dpi_decap_ipsec_ah(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
                ///*protocol
                ///for gre packet, process over
                ///aclk_dpi_process_packet_over(packet);
                ///return -1;
            case ACLK_DPI_PROTO_GTP:
                if (aclk_dpi_decap_gtp(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_L2TP:
                pkt->appidx = ACLK_PIDE_PROTO_L2TP;
                if (aclk_dpi_decap_l2tp(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_PPTP:
                pkt->appidx = ACLK_PIDE_PROTO_PPTP;
                g_rx_pkt_count[g_local_core_id].rx_pkt_pptp_num++;
                ///for pptp packet, process over
                aclk_dpi_process_packet_over(packet);
                return -1;
            case ACLK_DPI_PROTO_OPENVPN:
                pkt->appidx = ACLK_PIDE_PROTO_OPENVPN;
                g_rx_pkt_count[g_local_core_id].rx_pkt_openvpn_num++;
                ///for pptp packet, process over
                aclk_dpi_process_packet_over(packet);
                return -1;
            case ACLK_DPI_PROTO_SCTP:
                pkt->appidx = ACLK_PIDE_PROTO_SCTP;
                if (aclk_dpi_decap_sctp(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_S1AP:
                pkt->appidx = ACLK_PIDE_PROTO_S1AP;
                if (aclk_dpi_decap_s1ap(pkt, pkt_len, data, &offset, &protocol)) {
                    g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;
                    aclk_dpi_process_packet_over(packet);
                    return -1;
                }
                break;    
            default:
                g_rx_pkt_count[g_local_core_id].rx_pkt_err_num++;                      
                aclk_dpi_process_packet_over(packet);
                return -1;
        }
    }
    pkt->payload_offset = offset;
    if (pkt->ip_ver == 4) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_INFO, "ipv4, sip:%x, dip:%x, sport:%u, dport:%u,proto:%u, offset:%d\n", pkt->sip.ipv4, pkt->dip.ipv4, pkt->sport, pkt->dport, pkt->l4_proto, offset);
    } else {
        aclk_printf(ACLK_DPI_LOG_LEVEL_INFO, "ipv6, sip:%lx, dip:%lx, sport:%u, dport:%u,proto:%u, offset:%d\n", (uint64_t)pkt->sip.ipv6[0], (uint64_t)pkt->dip.ipv6[0], pkt->sport, pkt->dport, pkt->l4_proto, offset);
    }

    return 0;
}

//int aclk_decap_process_command(void *packet, void *data)
//{
//    return 0;
//}
//
//void aclk_decap_fini_local(void)
//{
//    return;
//}
//
//void aclk_decap_fini_global(void)
//{
//    return;
//}
//
