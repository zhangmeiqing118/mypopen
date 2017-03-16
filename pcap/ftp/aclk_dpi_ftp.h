/*
 * @Filename: aclk_dpi_ftp.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:31:57 PM
 */

#ifndef __ACLK_DPI_FTP_H__
#define __ACLK_DPI_FTP_H__

#ifdef  __cpulsplus
extern "C" {
#endif

extern CVMX_SHARED int g_rule_ftp_appidx;

///function define
void aclk_dpi_ftp_stat(int argc, char *argv[]);

int aclk_dpi_ftp_init_local(void);
int aclk_dpi_ftp_init_global(void);

int aclk_dpi_ftp_process_packet(cvmx_wqe_t *packet);
int aclk_dpi_ftp_process_command(void *packet, void *data);

void aclk_dpi_ftp_fini_local(void);
void aclk_dpi_ftp_fini_global(void);


#ifdef  __cpulsplus
}
#endif
#endif


