/*
 * @Filename: aclk.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 02:35:44 PM
 */
#ifndef __ACLK_H__
#define __ACLK_H__

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

typedef union cvmx_buf_ptr {
    void *ptr;
    uint64_t u64;
    struct {
#ifdef __OCTEON_SDK__
        uint64_t i:1;
        uint64_t back:4;
        uint64_t pool:3;
        uint64_t size:16;
        uint64_t addr:40;
#else
        void *addr;
#endif
    } s;
} cvmx_buf_ptr_t;

typedef struct cvmx_wqe {
    uint64_t word0;
    cvmx_wqe_word1_t word1;
    uint64_t word2;
    cvmx_buf_ptr_t packet_ptr;
    uint8_t packet_data[96];
} cvmx_wqe_t;

#ifndef CVMX_SHARED
#define CVMX_SHARED
#endif

extern CVMX_SHARED int g_log_level;

typedef enum {
    ACLK_DPI_LOG_LEVEL_NONE,
    ACLK_DPI_LOG_LEVEL_ERROR,
    ACLK_DPI_LOG_LEVEL_WARN,
    ACLK_DPI_LOG_LEVEL_INFO,
    ACLK_DPI_LOG_LEVEL_DEBUG,
    ACLK_DPI_LOG_LEVEL_ALL,
} aclk_dpi_log_level_t;

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

#define MAX_CORES   4

#include "aclk_dpi_module.h"
#include "aclk_dpi_decap.h"
#include "aclk_dpi_flow.h"

extern int g_local_core_id;

static inline int cvmx_wqe_get_len(cvmx_wqe_t *packet)
{
    return packet->word1.len;
}

static inline void *cvmx_phys_to_ptr(void *address)
{
    return (void *)address;
}

#endif
