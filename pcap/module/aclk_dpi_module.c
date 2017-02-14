/**
 * @File Name: aclk_dpi_module.c
 * @brief: 
 * @Author: zhangmeiqing
 * @Version: 1.0.0
 * @Date: Tue 03 May 2016 04:09:08 PM CST
 */
#include "aclk_sdk.h"
#include "aclk.h"

char *argv[4];

void aclk_dpi_result_print(void)
{
    int argc;

    argc = 0;
    ///memset(argv[argc], 0x00, sizeof(argv[argc]));
    ///strcpy(argv[argc], "decap");
    argv[argc] = "decap";
    argc++;
    ///memset(argv[argc], 0x00, sizeof(argv[argc]));
    ///strcpy(argv[argc], "show");
    argv[argc] = "show";
    argc++;

    printf("argv[0]:%s, argv[1]:%s\n", argv[0], argv[1]);
    aclk_dpi_decap_stat(argc, argv);
    ///aclk_dpi_decap_stat(argc, (char **)argv);

    return;
}

#if 0
static int g_local_core_id = 0;

void aclk_dpi_match_callback(void *packet)
{
    cvmx_wqe_t *wqe;
    aclk_dpi_pkt_info_t *pkt;

    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_ERROR, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return;
    }

    ///get packet info
    wqe = (cvmx_wqe_t *)packet;
    pkt = (aclk_dpi_pkt_info_t *)(wqe->packet_data);
    //printf("appidx:%d, name:%s\n", pkt->appidx, aclk_pide_get_rule_name_by_appidx(pkt->appidx));
    if (pkt->appidx) {
        ///set session appidx
        if (pkt->flow) {
            if (pkt->ip_ver == 4) {
                ((aclk_dpi_ipv4_flow_info_t *)(pkt->flow))->appidx = pkt->appidx;
            } else {
                ((aclk_dpi_ipv6_flow_info_t *)(pkt->flow))->appidx = pkt->appidx;
            }
        }
    //    aclk_hal_packet_transmit(wqe, cvmx_wqe_get_port(wqe));
    //} else {
    //    aclk_hal_packet_free(wqe, 1);
    }

    aclk_dpi_action_process_packet(packet);

    return;
}
#endif

int aclk_dpi_init(void)
{
    if (sizeof(aclk_dpi_pkt_info_t) > sizeof(((cvmx_wqe_t *)(86))-> packet_data)) { ///96:the sizeof((cvmx_wqe_t *(0))-> packet_data)
        printf("%s[%d]: sizeof(aclk_dpi_pkt_info) is too big\n", __func__, __LINE__);
        return -1;
    }
    /// init decap
    if (aclk_dpi_decap_init_global()) {
        printf("%s[%d]: decap module init error\n", __func__, __LINE__);
        return -1;
    }
    /// init connect
    if (aclk_dpi_flow_init_global()) {
        printf("%s[%d]: flow module init error\n", __func__, __LINE__);
        return -1;
    }
    
    return 0;
}

int aclk_dpi_process_packet(void *packet)
{
    cvmx_wqe_t *wqe;
    aclk_dpi_pkt_info_t *pkt;
    
    ///set pide module callback
    ///get packet info
    wqe = (cvmx_wqe_t *)packet;
    pkt = (aclk_dpi_pkt_info_t *)(wqe->packet_data);
    memset(pkt, 0x00, sizeof(aclk_dpi_pkt_info_t));
   
    /// decap packet
    if (aclk_dpi_decap_process_packet(packet)) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: decap process packet error\n", __func__, __LINE__);
        return -1;
    }
    
    /// add packet info to session table
    if (aclk_dpi_flow_process_packet(packet)) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: connect process packet error\n", __func__, __LINE__);
        return -1;
    }
    
    return 0;
}

void aclk_dpi_process_packet_over(void *packet)
{
#if 0
    cvmx_wqe_t *wqe;
    aclk_dpi_pkt_info_t *pkt;
    
    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return ;
    }

    ///set pide module callback
    ///get packet info
    wqe = (cvmx_wqe_t *)packet;
    pkt = (aclk_dpi_pkt_info_t *)(wqe->packet_data);
    
    //add stat
#endif

    return;
}

void aclk_dpi_fini(void)
{
    aclk_dpi_flow_fini_global();
    aclk_dpi_decap_fini_global();

    return;
}

