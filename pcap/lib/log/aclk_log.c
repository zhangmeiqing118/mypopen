/**
 * @Filename: aclk_log.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 06/29/2016 03:07:57 PM
 */
#include "aclk_sdk.h"
#include "aclk.h"

CVMX_SHARED static int g_dpi_log_level = ACLK_DPI_LOG_LEVEL_ERROR;

int aclk_dpi_log_level_get(void)
{
    return g_dpi_log_level;
}

void aclk_dpi_log_level_set(int level)
{
    g_dpi_log_level = level;

    return;
}

int aclk_dpi_log(int level, char *fmt, ...)
{
    int size;
    char buf[2048];
    va_list ap;

    size = 0;
    if (level <= g_dpi_log_level) {
        va_start(ap, fmt);
        size = vsprintf(buf, fmt, ap);
        va_end(ap);
        printf("%s", buf);
    }

    return size;
}
