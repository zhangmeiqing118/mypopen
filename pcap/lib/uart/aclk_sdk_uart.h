/**
 * @File Name: aclk_sdk_uart.h
 * @brief: 
 * @Author: zhangmeiqing
 * @Version: 1.0.0
 * @Date: Thu 05 May 2016 07:40:36 PM PDT
 */
#ifndef __ACLK_SDK_UART_H__
#define __ACLK_SDK_UART_H__

#ifdef  __cpulsplus
extern "C" {
#endif

#define OCTEON_UART_REAL    1
//==================================================
//function declare
/**
* @brief aclk_uart_init 
*
* @param index: uart console index
*
* @return, success: return 0
*          failed: return -1
*/
int aclk_uart_init(int index);

/**
 * @brief aclk_uart_printf 
 *
 * @param format
 * @param ...
 *
 * @return, success: return the num of printf string
 *          failed: return -1;
 */
int aclk_uart_printf(const char *format, ... );

/**
 * @brief aclk_uart_putchar 
 *
 * @param ch: the read byte
 *
* @return, success: return 0
*          failed: return -1
 */
int aclk_uart_putchar(uint8_t ch);

/**
 * @brief aclk_uart_getchar 
 *
 * @param ch: the pointer of the read byte
 *
* @return, success: return 0
*          failed: return -1
 */
int aclk_uart_getchar(uint8_t *ch);

/**
 * @brief aclk_uart_getchar_nowait 
 *
 * @param ch: the pointer of the read byte
 *
* @return, success: return 0
*          failed: return -1
 */
int aclk_uart_getchar_nowait(uint8_t *ch);

//==================================================

///#undef  printf
///#define printf(format, ...)     aclk_uart_printf(format, ##__VA_ARGS__)
///
///#undef  putchar
///#define putchar(ch) aclk_uart_putchar(ch)

///#undef  getchar
///#define getchar()   aclk_uart_getchar()

///#undef  getchar_nowait
///#define getchar_nowait  aclk_uart_getchar_nowait()

#ifdef  __cpulsplus
}
#endif

#endif
