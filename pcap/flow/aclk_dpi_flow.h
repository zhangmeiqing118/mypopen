/*
 * @Filename: aclk_dpi_flow.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 01/03/2017 02:40:44 PM
 */
#ifndef __ACLK_DPI_FLOW_H__
#define __ACLK_DPI_FLOW_H__

#ifdef __cplusplus
extern "C" {
#endif

int aclk_dpi_flow_init(void);

int aclk_dpi_flow_process_packet(cvmx_wqe_t *packet);

void aclk_dpi_flow_fini(void);

#ifdef __cplusplus
}
#endif
#endif
