/*
 * @Filename: aclk_pide_ftp.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:31:57 PM
 */

#ifndef __ACLK_PIDE_FTP_H__
#define __ACLK_PIDE_FTP_H__

#ifdef  __cpulsplus
extern "C" {
#endif

#define ACLK_PIDE_FTP_MODUEL  1
#define ACLK_DPI_FTP_PORT               21
#define ACLK_PIDE_MAX_FTP_RULE_NUM      4096

typedef struct {
    uint64_t cycles;
    uint16_t port;
} aclk_pide_ftp_rule_t;

extern CVMX_SHARED uint32_t g_pide_rule_ftp_appidx;

int aclk_pide_ftp_init_local(void);
int aclk_pide_ftp_init_global(void);

int aclk_pide_ftp_port_match(uint32_t appidx, uint16_t port);
int aclk_pide_ftp_rule_valid(uint32_t appidx);
int aclk_pide_ftp_ip_port_exist(uint32_t appidx, uint16_t port, uint64_t cycles);


int aclk_pide_ftp_process_packet(cvmx_wqe_t *packet);
int aclk_pide_ftp_packet_analysis(cvmx_wqe_t *packet);
int aclk_pide_ftp_process_command(void *packet, void *data);

void aclk_pide_ftp_fini_local(void);
void aclk_pide_ftp_fini_global(void);


#ifdef  __cpulsplus
}
#endif
#endif


