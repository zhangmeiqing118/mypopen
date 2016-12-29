/*
 * @Filename: aclk_dpi_decap_l2.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:36:00 PM
 */
#ifndef __ACLK_DPI_DECAP_L2_H__
#define __ACLK_DPI_DECAP_L2_H__

#ifdef __cplusplus
extern "C" {
#endif

int aclk_dpi_decap_level_2(aclk_dpi_pkt_info_t *pkt, uint8_t *data, uint32_t len, uint16_t *offset, uint16_t *protocol);

#ifdef __cplusplus
}
#endif
#endif
