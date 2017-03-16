/**
 * @File Name: aclk_sdk_shell.c
 * @brief: 
 * @Author: zhangmeiqing
 * @Version: 1.0.0
 * @Date: Tue 10 May 2016 04:58:50 AM PDT
 */

#include "aclk_sdk.h"
#include "aclk.h"

CVMX_SHARED int g_shell_cmd_flag = 0;
CVMX_SHARED cvmx_spinlock_t g_shell_cmd_lock = CVMX_SPINLOCK_UNLOCKED_INITIALIZER;
CVMX_SHARED  aclk_shell_cmd_t g_shell_cmd_list[MAX_SHELL_CMD_NUM];

static char g_shell_cmd_history_buf[MAX_SHELL_CMD_HISTORY_NUM][MAX_SHELL_CMD_BUFF_SIZE];
static uint32_t g_shell_cmd_history_buf_idx_last;
static uint32_t g_shell_cmd_history_buf_idx_cur;
static uint32_t  g_shell_cmd_buf_idx;
static char g_shell_cmd_buf[MAX_SHELL_CMD_BUFF_SIZE];
static char g_escape_key_buf[8];
static int g_escape_key_buf_idx;
static int g_escape_key_status;

static void __printf_prompt(void)
{
    aclk_uart_printf("\n%s", SHELL_CMD_PROMPT);
    fflush(stdout);

    return;
}

void aclk_shell_cmd_help(int argc, char *argv[])
{
    int i;

    aclk_uart_printf("Available Commands: \n");
    for (i = 0; i < MAX_SHELL_CMD_NUM; i++) {
        if (0 == g_shell_cmd_list[i].name[0]) {
            continue;
        }
        aclk_uart_printf(" %-32s %-s\n", g_shell_cmd_list[i].name, g_shell_cmd_list[i].comment);
    }

    return ;
}

void aclk_hal_reset(int argc, char *argv[])
{
    cvmx_reset_octeon();

    return;
}

int aclk_shell_cmd_register(char *name, char *comment, void (*func)(int, char *[]))
{
    int i;

    if ((NULL == name) || (NULL == func)) {
        aclk_uart_printf("%s[%d]:Invlaid cmd name(%p) or func(%p)", __func__, __LINE__, name, func);
        return -1;
    }
    for (i = 0; i < MAX_SHELL_CMD_NUM; i++) {
        if (0 == strcmp(name, g_shell_cmd_list[i].name)) {
            break;
        }
    }
    if (i < MAX_SHELL_CMD_NUM) {
        aclk_uart_printf("%s[%d]:shell cmd[%s] has already registered.\n", __func__, __LINE__, name);
        return -1;
    }
    for (i = 0; i < MAX_SHELL_CMD_NUM; i++) {
        if (0 == g_shell_cmd_list[i].name[0]) {
            break;
        }
    }
    if (i >= MAX_SHELL_CMD_NUM) {
        aclk_uart_printf("%s[%d]:shell cmd list is full.\n", __func__, __LINE__);
        return -1;
    }
    strcpy(g_shell_cmd_list[i].name, name);
    strcpy(g_shell_cmd_list[i].comment, comment);
    g_shell_cmd_list[i].func = func;
    
    return 0;
}

int aclk_shell_cmd_unregister(char *name)
{
    int i;

    if ((NULL == name) || (0 == name[0])) {
        aclk_uart_printf("%s[%d]:Invlaid cmd name(%p)", __func__, __LINE__, name);
        return -1;
    }
    for (i = 0; i < MAX_SHELL_CMD_NUM; i++) {
        if (0 == strcmp(name, g_shell_cmd_list[i].name)) {
            break;
        }
    }
    if (i >= MAX_SHELL_CMD_NUM) {
        aclk_uart_printf("%s[%d]:shell cmd[%s] has not registered.\n", __func__, __LINE__, name);
        return -1;
    }
    memset(g_shell_cmd_list[i].name, 0x00, sizeof(g_shell_cmd_list[i].name));
    memset(g_shell_cmd_list[i].comment, 0x00, sizeof(g_shell_cmd_list[i].comment));
    g_shell_cmd_list[i].func = NULL;

    return 0;
}

int aclk_shell_init(void)
{
    memset(g_shell_cmd_list, 0x00, sizeof(g_shell_cmd_list));
    g_shell_cmd_buf_idx = 0;
    memset(g_shell_cmd_buf, 0x00, sizeof(g_shell_cmd_buf));
    memset(g_shell_cmd_history_buf, 0x00, sizeof(g_shell_cmd_history_buf));
    g_shell_cmd_history_buf_idx_last = 1;
    g_shell_cmd_history_buf_idx_cur = 1;
    memset(g_escape_key_buf, 0x00, sizeof(g_escape_key_buf));
    g_escape_key_buf_idx = 0;
    g_escape_key_status = 0;
    g_shell_cmd_flag = 0;

    ///register help command
    aclk_shell_cmd_register("help", "show all command", aclk_shell_cmd_help);
    
    aclk_shell_cmd_register("reset", "reboot system", aclk_hal_reset);
    aclk_shell_cmd_register("reboot", "reboot system", aclk_hal_reset);

    return 0;
}

void aclk_shell_execute_cmd(char *buf)
{
    int i, len, flag;
    int argc, idx, quotes;
    char *argv[MAX_SHELL_CMD_ARGVS];
    char buf_t[MAX_SHELL_CMD_BUFF_SIZE];

    flag = 0;
    idx = 0;
    argc = 0;
    quotes = 0;
    memset(buf_t, 0x00, MAX_SHELL_CMD_BUFF_SIZE);
    memcpy(buf_t, buf, MAX_SHELL_CMD_BUFF_SIZE);
    len = strlen(buf_t);
    for (i = 0; i < len; i++) {
        if (!flag) {
            ///while (((i + 1) < len) && isspace(buf_t[i]) && isspace(buf_t[i + 1])) {
            while (isspace(buf_t[i])) {
                i++;
            }
            argv[argc] = &(buf_t[i]);
            flag = 1;
        }
        if (buf_t[i] == '"') {
            ///printf("quotes:%d, i:%d\n", quotes, i);
            quotes ^= 0x01;
        }
        if (!quotes && isspace(buf_t[i])) {
            buf_t[i] = '\0';
            argc++;
            flag = 0;
            while (((i + 1) < len) && isspace(buf_t[i]) && isspace(buf_t[i + 1])) {
                i++;
            }
        }
    }
    if (flag) {
        argc++;
    }
    ///printf("argc:%d\n", argc);
    ///for (i = 0; i < argc; i++) {
    ///    printf("argv[%d]:%s\n", i, argv[i]);
    ///}

    for (i = 0; i < MAX_SHELL_CMD_NUM && argc; i++) {
        if (0 == strcmp(g_shell_cmd_list[i].name, argv[0])) {
            break;
        }
    }
    if (i >= MAX_SHELL_CMD_NUM) {
        aclk_uart_printf("Invalid command\n");
        return;
    }
    if (!(g_shell_cmd_list[i].func)) {
        aclk_uart_printf("Invalid register func\n");
        return;
    }
    g_shell_cmd_list[i].func(argc, argv);

    return;
}

static int aclk_shell_run(unsigned char ch)
{
    unsigned int i;

    switch (ch) {
    case CR:
    case LF:
        g_shell_cmd_buf[g_shell_cmd_buf_idx] = '\0';
        g_shell_cmd_buf_idx = 0;
        aclk_uart_printf("\n");

        if (g_shell_cmd_buf[0]) {
            cvmx_spinlock_lock(&g_shell_cmd_lock);
            aclk_shell_execute_cmd(g_shell_cmd_buf);
            cvmx_spinlock_unlock(&g_shell_cmd_lock);
            if (g_shell_cmd_history_buf_idx_last) {
                if (strcmp(g_shell_cmd_history_buf[(g_shell_cmd_history_buf_idx_last - 1) % MAX_SHELL_CMD_HISTORY_NUM], g_shell_cmd_buf)) {
                    strcpy(g_shell_cmd_history_buf[g_shell_cmd_history_buf_idx_last % MAX_SHELL_CMD_HISTORY_NUM], g_shell_cmd_buf);
                    g_shell_cmd_history_buf_idx_last++;
                }
            }
            g_shell_cmd_history_buf_idx_cur = g_shell_cmd_history_buf_idx_last;
        }
        g_escape_key_status = 0;
        __printf_prompt();
        break;
    case CTRL_C:
        memset(g_shell_cmd_buf, 0x00, sizeof(g_shell_cmd_buf));
        g_shell_cmd_buf_idx = 0;

        g_escape_key_status = 0;
        __printf_prompt();
        break;
    case BACKSPACE:
    case DELETE:
        if (g_shell_cmd_buf_idx == 0) {
            aclk_uart_printf("\a");
        } else {
            g_shell_cmd_buf_idx--;
            g_shell_cmd_buf[g_shell_cmd_buf_idx] = 0;
            aclk_uart_printf("\b \b");
        }
        fflush(stdout);
        g_escape_key_status = 0;
        break;
    case TAB:
        aclk_uart_printf("\a");
        fflush(stdout);
        g_escape_key_status = 0;
        break;
    case ESC:
        g_escape_key_status = 1;
        g_escape_key_buf_idx = 0;
        g_escape_key_buf[g_escape_key_buf_idx] = '0';
        break;
    default:
        if (g_escape_key_status) {
            g_escape_key_buf[g_escape_key_buf_idx] = ch;
            g_escape_key_buf_idx++;
            g_escape_key_buf[g_escape_key_buf_idx] = '\0';
            if (g_escape_key_buf_idx == 1) {
                if (g_escape_key_buf[0] != 0x5b) {
                    g_escape_key_status = 0;
                }
            } else if (g_escape_key_buf_idx == 2) {
                if ((g_escape_key_buf[1] != 0x41) && (g_escape_key_buf[1] != 0x42)) {
                    g_escape_key_status = 0;
                } else if (g_escape_key_buf[1] != 0x42) {
                    if (g_shell_cmd_history_buf_idx_cur != g_shell_cmd_history_buf_idx_last) {
                        for (i = 0; i < g_shell_cmd_buf_idx; i++)  {
                            aclk_uart_printf("\b \b");
                        }
                        g_shell_cmd_buf_idx = 0;
                    }
                    g_shell_cmd_history_buf_idx_cur--;
                    if (g_shell_cmd_history_buf_idx_cur < g_shell_cmd_history_buf_idx_last) {
                        if (g_shell_cmd_history_buf[g_shell_cmd_history_buf_idx_cur % MAX_SHELL_CMD_HISTORY_NUM][0]) {
                            strcpy(g_shell_cmd_buf, g_shell_cmd_history_buf[g_shell_cmd_history_buf_idx_cur % MAX_SHELL_CMD_HISTORY_NUM]);
                            g_shell_cmd_buf_idx = strlen(g_shell_cmd_buf);
                            aclk_uart_printf("%s", g_shell_cmd_buf);
                        }
                    }
                } else {
                    if (g_shell_cmd_history_buf_idx_cur != g_shell_cmd_history_buf_idx_last) {
                        for (i = 0; i < g_shell_cmd_buf_idx; i++)  {
                            aclk_uart_printf("\b \b");
                        }
                        g_shell_cmd_buf_idx = 0;
                    }
                    g_shell_cmd_history_buf_idx_cur++;
                    if (g_shell_cmd_history_buf_idx_cur < g_shell_cmd_history_buf_idx_last) {
                        if (g_shell_cmd_history_buf[g_shell_cmd_history_buf_idx_cur % MAX_SHELL_CMD_HISTORY_NUM][0]) {
                            strcpy(g_shell_cmd_buf, g_shell_cmd_history_buf[g_shell_cmd_history_buf_idx_cur % MAX_SHELL_CMD_HISTORY_NUM]);
                            g_shell_cmd_buf_idx = strlen(g_shell_cmd_buf);
                            aclk_uart_printf("%s", g_shell_cmd_buf);
                        }
                    }
                }
            }
        } else {
            if (isprint(ch)) {
                g_shell_cmd_buf[g_shell_cmd_buf_idx] = ch;
                g_shell_cmd_buf_idx++;
                aclk_uart_printf("%c", ch);
            }
            g_escape_key_status = 0;
        }
        break;
    }

    return 0;
}

int aclk_shell_loop(void)
{
    int ret;
    uint64_t cycles, last_cycles, hz, sec;
    unsigned char ch;
    cvmx_wqe_t *packet = NULL;

    hz = (uint64_t)(cvmx_sysinfo_get()->cpu_clock_hz);
    sec = 2 * hz;
    last_cycles = cvmx_get_cycle();
    __printf_prompt();
    do {
        ret = aclk_uart_getchar_nowait(&ch);
        if (!ret) {
            aclk_shell_run(ch);
        } else {
            aclk_cmd_process();
        }
        cycles = cvmx_get_cycle();
        if (cycles - last_cycles > sec) {
            aclk_dpi_pip_stat();
        }
        if (ACLK_HAL_PACKET_RECEIVE_OK == aclk_hal_packet_recv((void **)(&packet))) {
            if (aclk_dpi_process_command(packet)) {
                aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: dpi process command error\n", __func__, __LINE__);
            }
        }
    } while (1);

    return 0;
}
