/**
 * @Filename: aclk_dpi_ftp.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:15:00 PM
 */
#include "aclk_sdk.h"
#include "aclk.h"

#include "aclk_dpi_ftp.h"

///CVMX_SHARED int g_rule_ftp_appidx = 0;
CVMX_SHARED uint64_t g_rule_ftp_ip_port_timeout = 0;
CVMX_SHARED uint16_t g_ftp_rule_ring_index;
CVMX_SHARED aclk_pide_ftp_rule_t *g_ftp_rule;

int aclk_dpi_ftp_init_local(void)
{
    return 0;
}

int aclk_dpi_ftp_init_global(void)
{
    g_rule_ftp_ip_port_timeout = 3600 * ((uint64_t)(cvmx_sysinfo_get()->cpu_clock_hz));
    g_ftp_rule = (aclk_pide_ftp_rule_t *)cvmx_malloc(g_pide_mem_arenas, sizeof(aclk_pide_ftp_rule_t) * ACLK_PIDE_MAX_FTP_RULE_NUM);
    if (NULL == g_ftp_rule) {
        aclk_uart_printf("%s[%d]: pide ftp rule array alloc error\n", __func__, __LINE__);
        return -1;
    }
    memset(g_ftp_rule, 0x00, sizeof(aclk_pide_ftp_rule_t) * ACLK_PIDE_MAX_FTP_RULE_NUM);

    return 0;
}

int aclk_dpi_ftp_parse_port_pasv(uint8_t data, int len, ip_info_t *ip, uint16_t *port)
{
    uint8_t *p;
    uint8_t ip_address[4], port[2];
    int num;

    p = data;

    while (('\0' != *p) && !isdigit(*p)) {
        p++;
    }
    if ('\0' == *p) {
        return -1;
    }
    num = sscanf(p, "%d,%d,%d,%d,%d,%d", &(ip_address[0]), &(ip_address[1]), &(ip_address[2]), &(ip_address[3]), &(port[0]), &(port[1]));
    if (6 != num) {
        return -1;
    }
    ip->ipv4 = htonl((ip_address[0] << 24) | (ip_address[1] << 16) | (ip_address[2] << 8) | (ip_address[3]));
    *port = (port[0] << 8) | port[1];

    return 0;
}

///EPRT<space><d><net-prt><d><net-addr><d><tcp-port><d>
///net-prt, 1: ipv4, 2:ipv6
int aclk_dpi_ftp_parse_port_eprt(uint8_t data, int len, uint8_t *ver, ip_info_t *ip, uint16_t *port)
{
    uint8_t *p, delimiter;
    uint8_t ip_address[4], port[2];
    int num;
    char ipv6_str[64];

    p = data;
    delimiter = *(p + 1);
    p += 2; ///space and delimiter;
    sscanf(p, "%d", *ver);
    p += 2; ///net-prt and delimiter
    if ((1 != *ver) && (2 != *ver)) {
        return -1;
    }
    if (1 == *ver) {
        ///ipv4
        num = sscanf(p, "%d.%d.%d.%d", &(ip_address[0]), &(ip_address[1]), &(ip_address[2]), &(ip_address[3]));
        if (4 != num) {
            return -1;
        }
        ip->ipv4 = htonl((ip_address[0] << 24) | (ip_address[1] << 16) | (ip_address[2] << 8) | (ip_address[3]));
        ip_len = (ip_address[0] < 10 ? 1 : (ip_address[0] < 100 ? 2 : 3)) + 1
            + (ip_address[1] < 10 ? 1 : (ip_address[1] < 100 ? 2 : 3)) + 1
            + (ip_address[2] < 10 ? 1 : (ip_address[2] < 100 ? 2 : 3)) + 1
            + (ip_address[3] < 10 ? 1 : (ip_address[3] < 100 ? 2 : 3));
        p += ip_len;
        p += 1;//delimiter
        num = sscanf(p, "%d", &port);
        if (num != 1) {
            return -1;
        }

        *ver = 4;
    } else {
        ///ipv6
        num = 0;
        e = p
        while (('\0' != *e) && (delimiter != *e)) {
            e++;
        }
        memcpy(ipv6_str, p, e - p);
        num = inet_pton(AF_INET6, p, (void *)(ip->ipv6));
        if (-1 == num) {
            return -1;
        }
        p = e + 1;//delimiter
        num = sscanf(p, "%d", &port);
        if (num != 1) {
            return -1;
        }
        *ver = 6;
    }

    return 0;
}


/**
 * @brief aclk_dpi_ftp_port_match 
 *
 * @param appidx
 * @param port
 *
 * @return 1:match success, 0:not match
 */
int aclk_dpi_ftp_port_match(uint32_t appidx, uint16_t port)
{
    uint64_t cycles;
    uint32_t idx;

    cycles = cvmx_get_cycle();
    if (appidx < ACLK_PIDE_MAX_RULE_NUM) {
        return 0;
    }

    idx = appidx - ACLK_PIDE_MAX_RULE_NUM;
    if ((g_ftp_rule[idx].cycles < cycles) && (g_ftp_rule[idx].cycles == port)) {
        return 1;
    }

    return 0;
}

int aclk_dpi_ftp_packet_analysis(cvmx_wqe_t *packet)
{
    uint8_t is_port_request, is_eprt_request, is_pasv_response;
    uint8_t *data, *sptr, *eptr, ver;
    ip_info_t ftp_ip;
    uint16_t ftp_port;
    aclk_dpi_pkt_info_t *pkt;
    
    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return -1;
    }

    pkt = (aclk_dpi_pkt_info_t *)((uint8_t *)(packet->packet_data));
    data = (uint8_t *)cvmx_phys_to_ptr(packet->packet_ptr.s.addr + pkt->payload_offset);
    eptr = data + cvmx_wqe_get_len(packet);

    s_ptr = data;
    is_port_request = 0;
    is_eprt_request = 0;
    if (pkt->dport == ACLK_DPI_FTP_PORT) {
        if (0 == strncmp(s_ptr, "PORT", 4)) {
            is_port_request = 1;
            sptr = data + 4;
        } 
        if (0 == strncmp(s_ptr, "EPRT", 4)) {
            is_eprt_request = 1;
            sptr = data + 4;
        }
    } else {
        if (0 == strncmp(s_ptr, "227", 3)) {
            is_pasv_response = 1;
            sptr = data + 3;
        }
    }

    if (is_port_request || is_pasv_response) {
        recode = aclk_dpi_ftp_parse_port_pasv(sptr, eptr - sptr, &ftp_ip, &ftp_port);
        if (!recode) {
            ///add ip port to ip rule(ftp_ip[0], ftp_port)
            g_ftp_rule[g_ftp_rule_ring_index].cycles = cvmx_get_cycle();
            g_ftp_rule[g_ftp_rule_ring_index].port = ftp_port;
            if (aclk_pide_ip_config_add(4, ftp_ip, 0, ACLK_PIDE_MAX_RULE_NUM + g_ftp_rule_ring_index)) {
                g_ftp_rule[g_ftp_rule_ring_index].cycles = 0;
                g_ftp_rule[g_ftp_rule_ring_index].port = 0;
            }
            if (g_ftp_rule_ring_index == (ACLK_PIDE_MAX_FTP_RULE_NUM - 1)) {
                g_ftp_rule_ring_index = 0;
            }
        }
    }
    if (is_eprt_request) {
        recode = aclk_dpi_ftp_parse_port_eprt(sptr, eptr - sptr, &ver, &ftp_ip, &ftp_port);
        if (!recode) {
            ///add ip port to ip rule(ver = 4:ftp_ip[0], ftp_port, ver = 6: ftp_ip, ftp_port)
            if (4 == ver) {
                ///ipv4
                g_ftp_rule[g_ftp_rule_ring_index].cycles = cvmx_get_cycle();
                g_ftp_rule[g_ftp_rule_ring_index].port = ftp_port;
                if (aclk_pide_ip_config_add(4, ftp_ip, 0, ACLK_PIDE_MAX_RULE_NUM + g_ftp_rule_ring_index)) {
                    g_ftp_rule[g_ftp_rule_ring_index].cycles = 0;
                    g_ftp_rule[g_ftp_rule_ring_index].port = 0;
                }
                if (g_ftp_rule_ring_index == (ACLK_PIDE_MAX_FTP_RULE_NUM - 1)) {
                    g_ftp_rule_ring_index = 0;
                }
            } else {
                ///ipv6
                g_ftp_rule[g_ftp_rule_ring_index].cycles = cvmx_get_cycle();
                g_ftp_rule[g_ftp_rule_ring_index].port = ftp_port;
                if (aclk_pide_ip_config_add(6, ftp_ip, 0, ACLK_PIDE_MAX_RULE_NUM + g_ftp_rule_ring_index)) {
                    g_ftp_rule[g_ftp_rule_ring_index].cycles = 0;
                    g_ftp_rule[g_ftp_rule_ring_index].port = 0;
                }
                if (g_ftp_rule_ring_index == (ACLK_PIDE_MAX_FTP_RULE_NUM - 1)) {
                    g_ftp_rule_ring_index = 0;
                }
            }
        }
    }

    return 0;
}

int aclk_dpi_ftp_process_packet(cvmx_wqe_t *packet)
{
    return ;
}

int aclk_dpi_ftp_process_command(void *packet, void *data)
{
    return 0;
}

void aclk_dpi_ftp_fini_local(void)
{
    return;
}

void aclk_dpi_ftp_fini_global(void)
{
    if (g_ftp_rule) {
        cvmx_free(g_ftp_rule);
        g_ftp_rule = NULL;
    }
    return;
}
