/*
 * @Filename: aclk.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 02:35:44 PM
 */
#ifndef __ACLK_H__
#define __ACLK_H__

#include "aclk_dpi_module.h"
#include "aclk_dpi_decap.h"
///#include "aclk_dpi_flow.h"

#ifdef  __cpulsplus
extern "C" {
#endif

typedef struct cvmx_wqe_word1 {
    uint64_t len:16;
    uint64_t zero_0:1;
    uint64_t qos:3;
    uint64_t zero_1:1;
    uint64_t grp:6;
    uint64_t zero_2:1;
    uint64_t tag_type:2;
    uint64_t tag:32;
} cvmx_wqe_word1_t;

typedef struct cvmx_wqe {
    uint64_t word0;
    cvmx_wqe_word1_t word1;
    uint64_t word2;
    void *packet_ptr;
    uint8_t packet_data[96];
} cvmx_wqe_t;

#define aclk_uart_printf printf
#define aclk_printf(level, fmt, arg ... )           \
    do {                                            \
        char *str;                                  \
                                                    \
        if (g_log_level >= level) {                 \
            switch (level) {                        \
                case ACLK_DPI_LOG_LEVEL_ERROR:      \
                    str = "ERROR";                  \
                    break;                          \
                case ACLK_DPI_LOG_LEVEL_WARN:       \
                    str = "WARN";                   \
                    break;                          \
                case ACLK_DPI_LOG_LEVEL_DEBUG:      \
                    str = "DEBUG";                  \
                    break;                          \
                case ACLK_DPI_LOG_LEVEL_INFO:       \
                default:                            \
                    str = "INFO";                   \
                    break;                          \
            }                                       \
            printf("[%s]"fmt, str, arg);            \
        }                                           \
    } while (0);

#ifdef  __cpulsplus
}
#endif

#endif
