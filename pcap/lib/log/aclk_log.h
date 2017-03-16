/*
 * @Filename: aclk_log.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 06/29/2016 02:54:46 PM
 */
#ifndef __ACLK_LOG_H__
#define __ACLK_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ACLK_DPI_LOG_LEVEL_NONE,
    ACLK_DPI_LOG_LEVEL_ERROR,
    ACLK_DPI_LOG_LEVEL_WARN,
    ACLK_DPI_LOG_LEVEL_INFO,
    ACLK_DPI_LOG_LEVEL_DEBUG,
    ACLK_DPI_LOG_LEVEL_ALL,
} aclk_dpi_log_level_t;
    
int aclk_dpi_log_level_get(void);
void aclk_dpi_log_level_set(int);

int aclk_dpi_log(int level, char *format, ...);
#define aclk_printf aclk_dpi_log

#ifdef __cplusplus
}
#endif
#endif
