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

CVMX_SHARED int g_rule_ftp_appidx = 0;
CVMX_SHARED uint64_t g_rule_ftp_ip_port_timeout = 0;

int aclk_dpi_ftp_init_local(void)
{
    return 0;
}

int aclk_dpi_ftp_init_global(void)
{
    g_rule_ftp_ip_port_timeout = 3600 * ((uint64_t)(cvmx_sysinfo_get()->cpu_clock_hz));

    return 0;
}

int aclk_dpi_ftp_parse_port_pasv(uint8_t data, int len, uint32_t ip, uint16_t port)
{
    return 0;
}

int aclk_dpi_ftp_process_packet(cvmx_wqe_t *packet)
{
    uint8_t is_request, is_port_request, is_eprt_request;
    uint8_t is_pasv_response, is_epasv_response;
    uint8_t *data, *sptr, *eptr;
    uint32_t ftp_ip;
    uint16_t ftp_port;
    aclk_dpi_pkt_info_t *pkt;
    
    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return -1;
    }

    pkt = (aclk_dpi_pkt_info_t *)((uint8_t *)(packet->packet_data));
    data = (uint8_t *)cvmx_phys_to_ptr(packet->packet_ptr.s.addr + pkt->payload_offset);
    eptr = data + cvmx_wqe_get_len(packet);

    is_request = 0;
    s_ptr = data;
    is_port_request = 0;
    is_eprt_request = 0;
    if (pkt->dport == ACLK_DPI_FTP_PORT) {
        is_request = 1;
        if (0 == strncmp(s_ptr, "PORT", 4)) {
            is_port_request = 1;
        } 
        if (0 == strncmp(s_ptr, "EPRT", 4)) {
            is_eprt_request = 1;
        }
    } else {
        is_request = 0;
        if (0 == strncmp(s_ptr, "227", 3)) {
            is_pasv_response = 1;
        }
        if (0 == strncmp(s_ptr, "229", 3)) {
            is_epasv_response = 1;
        }
    }
    if (4 == pkt->ip_ver) {
        if (pkt->dport == 21) {
            /// request 
            /// port request
            if (0 == strncmp(data, "PORT", 4)) {
                sptr = data + 4;
                recode = aclk_dpi_ftp_parse_port_pasv(sptr, eptr - sptr, &ftp_ip, &ftp_port);
                if (!recode) {
                    ///add ip port to ip rule
                }
            }
        } else if (pkt->sport == 21) {
            ///for port 
            if (0 == strncmp(data, "227", 4)) {
                sptr = data + 3;
                recode = aclk_dpi_ftp_parse_port_pasv(sptr, eptr - sptr, &ftp_ip, &ftp_port);
                if (!recode) {
                    ///add ip port to ip rule
                }
            }
        }
    }

    return 0;
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
    return;
}
