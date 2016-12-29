/**
 * @Filename: aclk_dpi_decap.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:15:00 PM
 */
#include "aclk_sdk.h"
#include "aclk.h"

CVMX_SHARED aclk_dpi_decap_stat_t g_decap_stat;

int aclk_dpi_decap_init(void)
{
    memset(&g_decap_stat, 0x00, sizeof(aclk_dpi_decap_stat_t));

    return 0;
}

int aclk_dpi_decap_process_packet(cvmx_wqe_t *packet)
{
    uint8_t *data = NULL;
    uint16_t offset, protocol;
    aclk_dpi_pkt_info_t *pkt;
    
    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return -1;
    }
    g_decap_stat.pkt_num++;
    
    
    offset = 0;
    protocol = ACLK_DPI_PROTO_LEVEL_2;
    pkt_len = aclk_dpi_wqe_get_len(packet);
    pkt = (aclk_dpi_pkt_info_t *)((uint8_t *)(packet->packet_data));
    data = (uint8_t *)(packet->packet_ptr);
    while (ACLK_DPI_PROTO_LEVEL_5 != protocol) {
        switch (protocol) {
            case ACLK_DPI_PROTO_LEVEL_2:
                if (aclk_dpi_decap_level_2(pkt, data, len, &offset, &protocol)) {
                    g_decap_stat.decap_err++;
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_LEVEL_3:
                if (aclk_dpi_decap_level_3(pkt, data, len, &offset, &protocol)) {
                    g_decap_stat.decap_err++;
                    return -1;
                }
                break;
            case ACLK_DPI_PROTO_LEVEL_2:
                if (aclk_dpi_decap_level_3(pkt, data, len, &offset, &protocol)) {
                    g_decap_stat.decap_err++;
                    return -1;
                }
                break;
            default:
                g_decap_stat.decap_err++;
                break;
        }
    }

    return 0;
}

void aclk_dpi_decap_fini(void)
{
    return;
}
