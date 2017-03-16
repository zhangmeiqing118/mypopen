/**
 * @File Name: aclk_sdk_uart.c
 * @brief: 
 * @Author: zhangmeiqing
 * @Version: 1.0.0
 * @Date: Thu 05 May 2016 07:53:40 PM PDT
 */

#include "aclk_sdk.h"
#include "aclk.h"

static unsigned int g_uart_index = 0;
static CVMX_SHARED cvmx_spinlock_t g_uart_print_lock = CVMX_SPINLOCK_UNLOCKED_INITIALIZER;
#if !(OCTEON_UART_REAL)
static CVMX_SHARED uint64_t g_uart_addr = 0;
#endif

int aclk_uart_putchar(uint8_t ch)
{
#if OCTEON_UART_REAL
    cvmx_uart_lsr_t lsrval;

    /* Spin until there is room */
    do {
        lsrval.u64 = cvmx_read_csr(CVMX_MIO_UARTX_LSR(g_uart_index));
        if (lsrval.s.thre == 0) {
            cvmx_wait(10000);
        }
    } while (lsrval.s.thre == 0);

    /* Write the byte */
    cvmx_write_csr(CVMX_MIO_UARTX_THR(g_uart_index), ch);

    return 0;
#else
    if (g_uart_addr) {
        char ch0 = '\r';

        if (ch == '\n') {
            octeon_pci_console_write(g_uart_addr, g_uart_index,  &ch0, 1, OCT_PCI_CON_FLAG_NONBLOCK);
        }
        octeon_pci_console_write(g_uart_addr, g_uart_index,  &ch, 1, OCT_PCI_CON_FLAG_NONBLOCK);

        return 0;
    } else {
        printf("%s[%d]: octeon uart init error\n", __func__, __LINE__);
        return -1;
    }
#endif
}

int aclk_uart_getchar(uint8_t *ch)
{
#if OCTEON_UART_REAL
    cvmx_uart_lsr_t lsrval;

    /* Spin until data is available */
    do {
        lsrval.u64 = cvmx_read_csr(CVMX_MIO_UARTX_LSR(g_uart_index));
        if (lsrval.s.dr) {
            cvmx_wait(10000);
        }
    } while (!lsrval.s.dr);

    /* Read the data */
    *ch = cvmx_read_csr(CVMX_MIO_UARTX_RBR(g_uart_index));

    return 0;
#else
    if (g_uart_addr) {
        int rv;

        rv = octeon_pci_console_read(g_uart_addr, g_uart_index,  ch, 1, !(OCT_PCI_CON_FLAG_NONBLOCK));
        if (rv < 0) {
            return rv;
        }
        return 0;
    } else {
        printf("%s[%d]: octeon uart init error\n", __func__, __LINE__);
        return -1;
    }
#endif
}

int aclk_uart_getchar_nowait(uint8_t *ch)
{
#if OCTEON_UART_REAL
    cvmx_uart_lsr_t lsrval;

    lsrval.u64 = cvmx_read_csr(CVMX_MIO_UARTX_LSR(g_uart_index));
    if (!(lsrval.s.dr)) {
        return -1;
    }
    *ch =  cvmx_read_csr(CVMX_MIO_UARTX_RBR(g_uart_index));

    return 0;
#else
    if (g_uart_addr) {
        int rv;

        rv = octeon_pci_console_read(g_uart_addr, g_uart_index,  &ch, 1, !(OCT_PCI_CON_FLAG_NONBLOCK));
        if (rv < 0) {
            return rv;
        }
        return 0;
    } else {
        printf("%s[%d]: octeon uart init error\n", __func__, __LINE__);
        return -1;
    }
#endif
}

int aclk_uart_printf(const char *format, ... )
{
    char buffer[1024];
    va_list args;
    int i, result;
    char *ptr;

    va_start(args, format);
    result = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    i = result;
    ptr = buffer;
    cvmx_spinlock_lock(&g_uart_print_lock);
    while (i > 0) {
        if (*ptr == '\n') {
            aclk_uart_putchar('\r');
        }
        aclk_uart_putchar(*ptr);
        ptr++;
        i--;
    }
    cvmx_spinlock_unlock(&g_uart_print_lock);

    return result;
}

//==================================================
int aclk_uart_init(int index)
{
#if OCTEON_UART_REAL
#if 1
    cvmx_uart_ier_t ierval;

    g_uart_index = index;
    ierval.u64 = 0;
    cvmx_write_csr(CVMX_MIO_UARTX_IER(index), ierval.u64);
#endif
#if 0
    cvmx_uart_mcr_t mcr;

    mcr.u64 = cvmx_read_csr(CVMX_MIO_UARTX_MCR(0));
    mcr.s.afce = 1;
    cvmx_write_csr(CVMX_MIO_UARTX_MCR(0), mcr.u64);
#endif

    return 0;
#else
    cvmx_bootmem_named_block_desc_t *pci_console_ptr;
    
    pci_console_ptr = cvmx_bootmem_phy_named_block_find( "__pci_console", 0 );
    if (pci_console_ptr == NULL ){
        printf("uart pci console addr is NULL\n");
        g_uart_addr = 0;
    } else {
        g_uart_addr = pci_console_ptr->base_addr;
        printf("uart pci console addr in 0x%lx\n", g_uart_addr);
    }
#endif

    return 0;
}
