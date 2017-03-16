/**
 * @File Name: aclk_sdk_shell.h
 * @brief: 
 * @Author: zhangmeiqing
 * @Version: 1.0.0
 * @Date: Thu 05 May 2016 07:40:36 PM PDT
 */
#ifndef __ACLK_SDK_SHELL_H__
#define __ACLK_SDK_SHELL_H__

#ifdef  __cpulsplus
extern "C" {
#endif

#define CTRL_C      0x03
#define BACKSPACE   0x08
#define TAB         0x09
#define LF          0x0a
#define CR          0x0d
#define ESC         0x1B
#define DELETE      0x7F

#define MAX_SHELL_CMD_NAME_SIZE 32
#define MAX_SHELL_CMD_NUM       128
#define MAX_SHELL_CMD_ARGVS     32
#define SHELL_CMD_PROMPT        "ACCELINK>>"

///for record history
#define MAX_SHELL_CMD_BUFF_SIZE     256
#define MAX_SHELL_CMD_HISTORY_NUM   64

typedef struct aclk_shell_cmd {
    char name[MAX_SHELL_CMD_NAME_SIZE];
    char comment[MAX_SHELL_CMD_NAME_SIZE];
    void (*func)(int, char *[]);
} aclk_shell_cmd_t;

extern CVMX_SHARED cvmx_spinlock_t g_shell_cmd_lock;
extern CVMX_SHARED int g_shell_cmd_flag;

int aclk_shell_cmd_register(char *name, char *comment, void (*func)(int, char *[]));
int aclk_shell_cmd_unregister(char *name);
void aclk_shell_execute_cmd(char *buf);

int aclk_shell_init(void);
int aclk_shell_loop(void);

#ifdef  __cpulsplus
}
#endif

#endif
