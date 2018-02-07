/**
 * @Filename: npcmd.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 10/11/2016 07:38:57 PM
 */
#include <stdio.h>

#include "zebra.h"
#include "loadxml.h"
#include "comm.h"

#define MEM_SIZE    2000
#define MAX_RULE_INFO_NUM   256
#define MAX_RULE_NUM    0xffff
#define MAX_NP_PORT_NUM 4

#define DEFAULT_SERVER  "127.0.0.1"
#define DEFAULT_PORT    8888

int g_sock_fd;
extern vector cmdvec;
static char *g_err_cmd_str = "Invalid";
static char *g_over_cmd_str = "over\r\n";
static char *g_more_cmd_str = "more";
static char *g_not_find_cmd_str = "not find\r\n";

struct cmd_node npx_node = {
    NPX_NODE,
    "cpux> ",
    0, NULL, NULL
};

struct cmd_node np0_node = {
    NP0_NODE,
    "cpu0> ",
    0, NULL, NULL
};

struct cmd_node np1_node = {
    NP1_NODE,
    "cpu1> ",
    0, NULL, NULL
};

/**
 * @brief aclk_cmd_send_recv 
 *
 * @param vty
 * @param octeon_id
 * @param cmd
 *
 * @return -1:error; 0:ok; 1: over
 */
int aclk_cmd_send_recv(struct vty *vty, int octeon_id, char *cmd)
{
    int recode, more_flag;
    int slen, rlen;
    aclk_dpi_cmd_t *req, *res;
    char sdata[MEM_SIZE], rdata[MEM_SIZE];

    do {
        more_flag = 0;
        memset(sdata, 0x00, MEM_SIZE);
        req = (aclk_dpi_cmd_t *)sdata;
        req->magic_num = htonl(ACLK_CMD_MAGIC_NUM);
        req->opcode = htonl(ACLK_MODULE_COMMAND | ACLK_COMMAND_CMD_SEND_STRING);
        req->recode = 0;
        slen = strlen(cmd);
        req->len = htonl(slen + sizeof(octeon_id));
        memcpy(req->data, cmd, slen);
        *((unsigned int *)(req->data + slen)) = htonl(octeon_id);
        slen += sizeof(octeon_id);
        rlen = MEM_SIZE;
        if (aclk_comm_send_and_recv(g_sock_fd, sdata, slen + sizeof(aclk_dpi_cmd_t), rdata, &rlen)) {
            vty_out(vty, "send or recv error. %s", VTY_NEWLINE);
            return -1;
        }
        res = (aclk_dpi_cmd_t *)rdata;
        recode = ntohl(res->recode);
        rlen = ntohl(res->len);
        if (recode || rlen > MEM_SIZE) {
            vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
            return -1;
        }
        if (0 == strncmp((char *)(res->data), g_err_cmd_str, strlen(g_err_cmd_str))) {
            if (rlen) {
                vty_out(vty, "%s", res->data);
            }
            ///vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
            return -1;
        }

        if ((rlen > (int)strlen(g_more_cmd_str)) && (0 == strncmp((char *)(res->data + rlen - strlen(g_more_cmd_str) - 1), g_more_cmd_str, strlen(g_more_cmd_str)))) {
            res->data[rlen - strlen(g_more_cmd_str) - 1] = '\0';
            more_flag = 1;
        }
        if ((rlen > (int)strlen(g_over_cmd_str)) && (0 == strncmp((char *)(res->data + rlen - strlen(g_over_cmd_str) - 1), g_over_cmd_str, strlen(g_over_cmd_str)))) {
            res->data[rlen - strlen(g_over_cmd_str) - 1] = '\0';
            vty_out(vty, "%s", res->data);
            return 1;
        } else if ((rlen > (int)strlen(g_not_find_cmd_str)) && (0 == strncmp((char *)(res->data + rlen - strlen(g_not_find_cmd_str) - 1), g_not_find_cmd_str, strlen(g_not_find_cmd_str)))) {
            vty_out(vty, "%s", res->data);
            return -1;
        } else {
            vty_out(vty, "%s", res->data);
        }
    } while (more_flag);

    return 0;
}

int aclk_conn_probe(struct vty *vty, unsigned int octeon_id)
{
    int recode;
    int slen, rlen;
    aclk_dpi_cmd_t *req, *res;
    char sdata[MEM_SIZE], rdata[MEM_SIZE];

    memset(sdata, 0x00, MEM_SIZE);
    req = (aclk_dpi_cmd_t *)sdata;
    req->magic_num = htonl(ACLK_CMD_MAGIC_NUM);
    req->opcode = htonl(ACLK_MODULE_COMMAND | ACLK_COMMAND_CMD_OCTEON_PROBE);
    req->recode = 0;
    slen = 0;
    req->len = htonl(sizeof(octeon_id));
    *((unsigned int *)(req->data + slen)) = htonl(octeon_id);
    slen += sizeof(octeon_id);
    rlen = MEM_SIZE;
    if (aclk_comm_send_and_recv(g_sock_fd, sdata, slen + sizeof(aclk_dpi_cmd_t), rdata, &rlen)) {
        vty_out(vty, "send or recv error. %s", VTY_NEWLINE);
        return -1;
    }
    res = (aclk_dpi_cmd_t *)rdata;
    recode = ntohl(res->recode);
    if (recode) {
        vty_out(vty, "octeon %d is down%s", octeon_id, VTY_NEWLINE);
    } else {
        vty_out(vty, "octeon %d is running!%s", octeon_id, VTY_NEWLINE);
    }

    return recode;
}

DEFUN(cpux, cpux_cmd, "cpux", "Turn on np all mode command\n")
{
    int recode;
    int node;
  
    node = 0x03;
    recode = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode |= aclk_conn_probe(vty, 0);
    }
    
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode |= aclk_conn_probe(vty, 1);
    }
    if (!recode) {
        vty->node = NPX_NODE;
    }

    return CMD_SUCCESS;
}

DEFUN(cpu0, cpu0_cmd, "cpu0", "Turn on np0 mode command\n")
{
    int recode;
    
    vty_out(vty, "np0:%s", VTY_NEWLINE);
    recode = aclk_conn_probe(vty, 0);
    if (!recode) {
        vty->node = NP0_NODE;
    }

    return CMD_SUCCESS;
}

DEFUN(cpu1, cpu1_cmd, "cpu1", "Turn on np1 mode command\n")
{
    int recode;
    
    vty_out(vty, "np1:%s", VTY_NEWLINE);
    recode = aclk_conn_probe(vty, 1);
    if (!recode) {
        vty->node = NP1_NODE;
    }

    return CMD_SUCCESS;
}

DEFUN(np_exit, np_exit_cmd, "exit", "Exit current mode and down to previous mode\n")
{
    switch (vty->node) {
        case NPX_NODE:
        case NP0_NODE:
        case NP1_NODE:
            vty->node = VIEW_NODE;
            break;
        default:
            break;
    }

    return CMD_SUCCESS;
}

DEFUN(np_list, np_list_cmd, "list", "Print command list\n")
{
    unsigned int i;
    struct cmd_node *cnode = vector_slot(cmdvec, vty->node);
    struct cmd_element *cmd;

    for (i = 0; i < vector_max(cnode->cmd_vector); i++) 
        if ((cmd = vector_slot(cnode->cmd_vector, i)) != NULL)
            vty_out(vty, "  %s%s", cmd->string, VTY_NEWLINE);
    return CMD_SUCCESS;
}

DEFUN(show_np_status, show_np_status_cmd, "show np status", "show np run status")
{
    int node;
    int recode0, recode1;

    if (vty->node == NP0_NODE) {
        node = 0x01;    
    } else if (vty->node == NP1_NODE) {
        node = 0x02;   
    } else {
        node = 0x03;    
    }
   
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_conn_probe(vty, 0);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_conn_probe(vty, 1);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}
ALIAS(show_np_status, display_np_status_cmd, "display np status", "Displays np run status")

DEFUN(load_np_rule, load_np_rule_cmd, "load np rule", "load np xml rule ")
{
    int node;

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        if (load_rule(g_sock_fd, 0)) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        if (load_rule(g_sock_fd, 1)) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_np_pool, show_np_pool_cmd, "show np pool", "Displays np pool number")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "hal show pool";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }

    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_np_packet, show_np_packet_cmd, "show np packet", "Displays np packet number")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "hal show packet";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }

    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(clear_np_packet, clear_np_packet_cmd, "clear np packet", "clear np input and output count")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "hal clear packet";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }

    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_np_pip, show_np_pip_cmd, "show np pip", "Displays np input output drop stat")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "hal show stat";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }

    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_np_pip_stat, show_np_pip_stat_cmd, "show pip stat", "Displays np pip statistics")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "hal show stat detail";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }

    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_np_pip_stat_port, show_np_pip_stat_port_cmd, "show pip stat PORT",
        "Displays np pip port statistics\r\n"
        "module name\n"
        "module function name\n"
        "port <2112, 2368, 2624, 3136>\n")
{
    int node;
    int recode0, recode1;
    char *cmd_tmp;
    char cmd_str[128] = "hal show stat detail ";

    cmd_tmp = strstr(vty->buf, "stat");
    cmd_tmp += strlen("stat");
    sprintf(cmd_str + strlen(cmd_str), "%s", cmd_tmp);

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }

    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(clear_np_pip, clear_np_pip_cmd, "clear np pip", "clear np input output drop stat")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "hal clear stat";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_np_port, show_np_port_cmd, "show np port", "show np port status")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "hal show port";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_np_info, show_np_info_cmd, "show np info", "show np system info")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "hal identify";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_np_vlanout_all, show_np_vlanout_all_cmd, "show np vlanout", 
        "show np vlanout stat all\r\n")
{
    int node;
    int recode0, recode1;
    char cmd_str[128] = "action show vlanout all";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(show_np_vlanout_vlanid, show_np_vlanout_vlanid_cmd, "show np vlanout VLANID", 
        "show np vlanout stat by vlanid\r\n"
        "module name\n"
        "module function name\n"
        "vlan id <1~4095>\n")
{
    int node;
    int recode0, recode1;
    char *cmd_tmp;
    int vlanid;
    char cmd_str[128] = "action show vlanout ";

    cmd_tmp = strstr(vty->buf, "vlanout");
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invlaid vlanout command%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    cmd_tmp += strlen("vlanout");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if ((NULL == cmd_tmp) || !isdigit(*cmd_tmp)) {
        vty_out(vty, "Invlaid vlanout command%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    vlanid = atoi(cmd_tmp);
    if ((vlanid <= 0) || (vlanid > 4095)) {
        vty_out(vty, "Invlaid vlanout vlanid(%d)%s", vlanid, VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    sprintf(cmd_str + strlen(cmd_str), "%d", vlanid);
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(clear_np_vlanout_all, clear_np_vlanout_all_cmd, "clear np vlanout", 
        "clear np vlanout stat all\r\n")
{
    int node;
    int recode0, recode1;
    char cmd_str[128] = "action clear vlanout all";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(clear_np_vlanout_vlanid, clear_np_vlanout_vlanid_cmd, "clear np vlanout VLANID", 
        "clear np vlanout stat by vlanid\r\n"
        "module name\n"
        "module function name\n"
        "vlan id <1~4095>\n")
{
    int node, vlanid;
    int recode0, recode1;
    char* cmd_tmp;
    char cmd_str[128] = "action clear vlanout ";

    cmd_tmp = strstr(vty->buf, "vlanout ");
    cmd_tmp += strlen("vlanout ");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (cmd_tmp) {
        vlanid = atoi(cmd_tmp);
    } else {
        vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    if ((vlanid <= 0) || (vlanid > 4095)) {
        vty_out(vty, "Invlaid vlanout vlanid(%d)%s", vlanid, VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    sprintf(cmd_str + strlen(cmd_str), "%d", vlanid);
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(show_np_packet_drop, show_np_packet_drop_cmd, "show np drop packet", 
        "show np drop packet stat\r\n")
{
    int node;
    int recode0, recode1;
    char cmd_str[128] = "hal show drop";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(clear_np_packet_drop, clear_np_packet_drop_cmd, "clear np drop packet", 
        "clear np drop packet stat\r\n")
{
    int node;
    int recode0, recode1;
    char cmd_str[128] = "hal clear drop";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(show_decap_num, show_decap_num_cmd, "show decap stat", "Displays pkt decap number")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "decap show";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(clear_decap_stat, clear_decap_stat_cmd, "clear decap stat", "clear decap count stat")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "decap clear";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_connect_timeout, show_connect_timeout_cmd, "show connect timeout", "Displays connect timeout")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "connect show timeout";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}


DEFUN(set_connect_timeout, set_connect_timeout_cmd, "set connect timeout TIMES", 
        "SET connect timeout times(sec)\r\n"
        "module name\n"
        "connect table property\n"
        "timeout times <30~180>\n"
        )
{
    int node;
    int recode0, recode1;
    char *cmd_tmp;
    FILE *fp_np_config;
    FILE *fp_np_config_tmp;
    char cmd_str[128] = "connect set ";
    char rule_str[128] = {0};
    char rule_str_tmp[128] = {0};
    int  rule_flag = 0;
    cmd_tmp = strstr(vty->buf, "timeout");
    if(sizeof(vty->buf) > 64){
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + 12, "%s", cmd_tmp);                            
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                memcpy(rule_str_tmp, cmd_str, 19);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, 19)) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP0);
                system(START_CONFIG_FILE_BACK_NP0);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                memcpy(rule_str_tmp, cmd_str, 19);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 32, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, 19)) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP1);
                system(START_CONFIG_FILE_BACK_NP1);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    return CMD_SUCCESS;
}


DEFUN(show_connect_stat, show_connect_stat_cmd, "show connect stat", "Displays connect stat")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "connect show stat";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_connect_table, show_connect_table_cmd, "show connect table", "Displays connect table")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "connect show table";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_app_stat, show_app_stat_cmd, "show app stat", "Displays basic manager stat")
{
    int node;
    int start;
    int recode0, recode1;
    char *tmp_str = "manager show stat";
    char cmd_str[128];

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        for (start = 0; start < MAX_RULE_NUM; start += 5) {
            memset(cmd_str, 0x00, 128);
            sprintf(cmd_str, "%s start=%d, num=5", tmp_str, start);
            recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
            if (1 == recode0) {
                recode0 = 0;
                break;
            }
            if (-1 == recode0) {
                vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
                break;
            }
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        for (start = 0; start < MAX_RULE_NUM; start += 5) {
            memset(cmd_str, 0x00, 128);
            sprintf(cmd_str, "%s start=%d, num=5", tmp_str, start);
            recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
            if (1 == recode1) {
                recode1 = 0;
                break;
            }
            if (-1 == recode1) {
                vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
                break;
            }
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_app_stat_rulename, show_app_stat_rulename_cmd, "show app name NAME",
        "Displays manager stat by rulename\r\n"
        "module name\n"
        "rule property\n"
        "rule name in rule xml file")
{
    int node;
    int recode0, recode1;
    char *cmd_tmp;
    char cmd_str[128] = "manager show ";

    cmd_tmp = strstr(vty->buf, "name");
    if(sizeof(vty->buf) > 64){
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + 13, "%s", cmd_tmp);                            
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_app_stat_rulenum, show_app_stat_rulenum_cmd, "show app id ID",
        "Displays manager stat by rulenum\r\n"
        "module name\n"
        "rule property\n"
        "rule id in rule xml file\n")
{
    int node;
    int recode0, recode1;
    char *cmd_tmp;
    char cmd_str[128] = "manager show ";

    cmd_tmp = strstr(vty->buf, "id");
    if(sizeof(vty->buf) > 64){
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + 13, "%s", cmd_tmp);                            
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(show_app_match_top, show_app_match_top_cmd, "show app top", "Displays top ten apps")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "manager show top";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(clear_app_stat, clear_app_stat_cmd, "clear app stat", "clear app matches count")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "manager clear stat";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(show_hfa_stat, show_hfa_stat_cmd, "show hfa stat", "Displays hfa stat")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "hfa show stat";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}



DEFUN(show_rule_rulename, show_rule_rulename_cmd, "show rule name NAME",
        "Displays rule info by rule name\r\n"
        "module name\n"
        "rule property\n"
        "rule name in rule xml file\n")
{
    int node, start;
    int recode0, recode1;
    char *cmd_tmp = "rule show ", *ptr;
    char cmd_str[128];

    if(sizeof(vty->buf) > 64){
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    ptr = strstr(vty->buf, "name");
    ptr += strlen("name");
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }

    recode0 = 0;
    if (node & 0x01) {
        for (start = 0; start < MAX_RULE_INFO_NUM; start += 5) {
            memset(cmd_str, 0x00, 128);
            sprintf(cmd_str, "%srulename%s start=%d, num=5", cmd_tmp, ptr, start);
            recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
            if (1 == recode0) {
                recode0 = 0;
                break;
            }
            if (-1 == recode0) {
                vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
                break;
            }
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        for (start = 0; start < MAX_RULE_INFO_NUM; start += 5) {
            memset(cmd_str, 0x00, 128);
            sprintf(cmd_str, "%srulename%s start=%d, num=5", cmd_tmp, ptr, start);
            recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
            if (1 == recode1) {
                recode1 = 0;
                break;
            }
            if (-1 == recode1) {
                vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
                break;
            }
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_rule_ruleid, show_rule_ruleid_cmd, "show rule id ID",
        "Displays rule info by rule id\r\n"
        "module name\n"
        "rule property\n"
        "rule id in rule xml file\n")
{
    int node, start;
    int recode0, recode1;
    char *cmd_tmp = "rule show ", *ptr;
    char cmd_str[128];

    if(sizeof(vty->buf) > 64){
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    ptr = strstr(vty->buf, "id");
    ptr += strlen("id");
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }

    recode0 = 0;
    if (node & 0x01) {
        for (start = 0; start < MAX_RULE_INFO_NUM; start += 5) {
            memset(cmd_str, 0x00, 128);
            sprintf(cmd_str, "%sruleid%s start=%d, num=5", cmd_tmp, ptr, start);
            recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
            if (1 == recode0) {
                recode0 = 0;
                break;
            }
            if (-1 == recode0) {
                vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
                break;
            }
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        for (start = 0; start < MAX_RULE_INFO_NUM; start += 5) {
            memset(cmd_str, 0x00, 128);
            sprintf(cmd_str, "%sruleid%s start=%d, num=5", cmd_tmp, ptr, start);
            recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
            if (1 == recode1) {
                recode1 = 0;
                break;
            }
            if (-1 == recode1) {
                vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
                break;
            }
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_rule_appidx, show_rule_appidx_cmd, "show rule appidx APPIDX",
        "Displays rule list by rule appidx\r\n"
        "module name\n"
        "rule property\n"
        "rule appidx in np rule array\n")
{
    int node;
    int recode0, recode1;
    char *cmd_tmp;
    char cmd_str[128] = "rule show ";

    cmd_tmp = strstr(vty->buf, "appidx");
    if(sizeof(vty->buf) > 64){
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + 10, "%s", cmd_tmp);                            
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_rule_rulenum, show_rule_rulenum_cmd, "show rule num", "Displays rule num")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "rule show num";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(set_rule_ruleid, set_rule_ruleid_cmd, "set rule id ID action ACTIONID",
        "Set rule info by rule id\r\n"
        "module name\n"
        "rule property\n"
        "rule id in rule xml file\n"
        "rule action property\n"
        "action id(0~255)\n")
{
    int node;
    int recode0, recode1;
    char rulename[64];
    FILE *fp_np_config;
    FILE *fp_np_config_tmp;
    char rule_str[128] = {0};
    char rule_str_tmp[128] = {0};
    int  rule_flag = 0;
    //char *actionid_tmp;
    int actionid, ruleid = 0;
    char *cmd_tmp;
    char cmd_str[128] = "rule config ruleid=";
    char cmd_rulename_tmp[128] = "rule config rulename=";
    //char cmd_str[128];
    //memset(ruleid, 0x00, 8);
    cmd_tmp = strstr(vty->buf, "id");
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    cmd_tmp += strlen("id");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    ruleid = atoi(cmd_tmp);
    rule_name_str_transfer(rulename, (unsigned int)ruleid);
    cmd_tmp = strstr(vty->buf, "action");
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    cmd_tmp += strlen("action");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    actionid = atoi(cmd_tmp);
    if ((actionid < 0) || (actionid > 255)) {
        vty_out(vty, "Invalid actionid%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    sprintf(cmd_str + strlen(cmd_str), "\"%d\" actionid=\"%d\"", ruleid, actionid);
    ///printf("cmd_str:%s\n", cmd_str);

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                sprintf(rule_str_tmp, "%s\"%s\" actionid", cmd_rulename_tmp, rulename);
                memset(cmd_str, 0, sizeof(cmd_str));
                sprintf(cmd_str, "%s\"%s\" actionid=\"%d\"", cmd_rulename_tmp, rulename, actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(rule_str_tmp))) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP0);
                system(START_CONFIG_FILE_BACK_NP0);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                sprintf(rule_str_tmp, "%s\"%s\" actionid", cmd_rulename_tmp, rulename);
                memset(cmd_str, 0, sizeof(cmd_str));
                sprintf(cmd_str, "%s\"%s\" actionid=\"%d\"", cmd_rulename_tmp, rulename, actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(rule_str_tmp))) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP1);
                system(START_CONFIG_FILE_BACK_NP1);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(set_rule_level_onoff, set_rule_level_onoff_cmd, "set rule level ENABLE", "set rule level on or off"
            "module name\n"
            "level property\n"
            "level on or off\n"            
            "ENABLE <on,off>\n")
{
    int node;
    int recode0, recode1;
    char *cmd_tmp;
    FILE *fp_np_config;
    FILE *fp_np_config_tmp;
    char rule_str[128] = {0};
    char rule_str_tmp[128] = {0};
    int  rule_flag = 0;
    char cmd_str[128] = "rule set ";

    cmd_tmp = strstr(vty->buf, "level");
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + strlen(cmd_str), "%s", cmd_tmp);                            
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                memcpy(rule_str_tmp, cmd_str, 14);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, 14)) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(cmd_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP0);
                system(START_CONFIG_FILE_BACK_NP0);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                memcpy(rule_str_tmp, cmd_str, 14);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, 14)) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP1);
                system(START_CONFIG_FILE_BACK_NP1);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    return CMD_SUCCESS;
}

DEFUN(set_rule_level_rulename, set_rule_level_rulename_cmd, "set rule name NAME level LEVELID",
        "Set rule info by rule name\r\n"
        "module name\n"
        "rule name property\n"
        "rule name in rule xml file\n"
        "rule level property\n"
        "level value:0-9\n")
{
    int node;
    int recode0, recode1;
    int levelid;
    char *cmd_tmp, *ptr;
    char rulename[64];
    FILE *fp_np_config;
    FILE *fp_np_config_tmp;
    char rule_str[128] = {0};
    char rule_str_tmp[128] = {0};
    int  rule_flag = 0;
    char cmd_str[128] = "rule config rulename=";

    memset(rulename, 0x00, 64);
    cmd_tmp = strstr(vty->buf, "name");
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_tmp += strlen("name");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    ptr = cmd_tmp;
    while (ptr && !isspace(*ptr)) {
        ptr++;
    }
    if(NULL == ptr){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    memcpy(rulename, cmd_tmp, ptr - cmd_tmp);

    cmd_tmp = strstr(vty->buf, "level");
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_tmp += strlen("level");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    levelid = atoi(cmd_tmp);
    if ((levelid < 0) || (levelid > 9)) {
        vty_out(vty, "Invalid level%s",  VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + strlen(cmd_str), "\"%s\" level=\"%d\"",rulename, levelid);   

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                sprintf(rule_str_tmp, "%s\"%s\" level", cmd_tmp, rulename);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(rule_str_tmp))) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP0);
                system(START_CONFIG_FILE_BACK_NP0);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                sprintf(rule_str_tmp, "%s\"%s\" level", cmd_tmp, rulename);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(rule_str_tmp))) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP1);
                system(START_CONFIG_FILE_BACK_NP1);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}
DEFUN(set_rule_level_ruleid, set_rule_level_ruleid_cmd, "set rule id ID level LEVELID",
        "Set rule info by rule name\r\n"
        "module name\n"
        "rule id property\n"
        "rule id in rule xml file\n"
        "rule level property\n"
        "level value:0-9\n")
{
    int node;
    int recode0, recode1;
    int ruleid, levelid;
    char *cmd_tmp;
    char cmd_str[128] = "rule config ruleid=";

    cmd_tmp = strstr(vty->buf, "id ");
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_tmp += strlen("id ");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    ruleid = atoi(cmd_tmp);

    cmd_tmp = strstr(vty->buf, "level");
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_tmp += strlen("level");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    levelid = atoi(cmd_tmp);
    if ((levelid < 0) || (levelid > 9)) {
        vty_out(vty, "Invalid level%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + strlen(cmd_str), "\"%d\" level=\"%d\"",ruleid, levelid);   

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}
DEFUN(set_rule_rulename, set_rule_rulename_cmd, "set rule name NAME action ACTIONID",
        "Set rule info by rule name\r\n"
        "module name\n"
        "rule name property\n"
        "rule name in rule xml file\n"
        "rule action property\n"
        "action id(0~255)\n")
{
    int node;
    int recode0, recode1;
    int actionid;
    char *cmd_tmp, *ptr;
    char rulename[64];
    FILE *fp_np_config;
    FILE *fp_np_config_tmp;
    char rule_str[128] = {0};
    char rule_str_tmp[128] = {0};
    int  rule_flag = 0;
    char cmd_str[128] = "rule config rulename=";

    memset(rulename, 0x00, 64);
    cmd_tmp = strstr(vty->buf, "name");
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_tmp += strlen("name");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    ptr = cmd_tmp;
    while (ptr && !isspace(*ptr)) {
        ptr++;
    }
    if (NULL == ptr) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    memcpy(rulename, cmd_tmp, ptr - cmd_tmp);

    cmd_tmp = strstr(vty->buf, "action");
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_tmp += strlen("action");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    actionid = atoi(cmd_tmp);
    if ((actionid < 0) || (actionid > 255)) {
        vty_out(vty, "Invalid actionid%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + strlen(cmd_str), "\"%s\" actionid=\"%d\"", rulename, actionid);   

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                sprintf(rule_str_tmp, "rule config rulename=\"%s\" actionid", rulename);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(rule_str_tmp))) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP0);
                system(START_CONFIG_FILE_BACK_NP0);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                sprintf(rule_str_tmp, "rule config rulename=\"%s\" actionid", rulename);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(rule_str_tmp))) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP1);
                system(START_CONFIG_FILE_BACK_NP1);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

int action_port_str_transfer(char *src, char dst[128])
{
    int i, len, flag;
    int start, end;
    char *ptr;

    len = 0;
    flag = 0;
    memset(dst, 0x00, 128);
    ptr = src;
    while (ptr && isspace(*ptr)) {
        ptr++;
    }
    while (ptr && *ptr) {
        if (isspace(*ptr)) {
            break;
        }
        start = atoi(ptr);
        while (ptr && isdigit(*ptr)) {
            ptr++;
        }
        if ((NULL == ptr) || (ptr && ((*ptr == ',') || isspace(*ptr) || (*ptr == '\0')))) {
            if ((start < 9) || (start > 23)) { ///valid port num 9-23
                return -1;
            }
            if (0 == flag) {
                flag = 1;
                len += sprintf(dst + len, "xe%d", start - 1);
            } else {
                len += sprintf(dst + len, ",xe%d", start - 1);
            }
            if ((NULL == ptr) || (*ptr == '\0') || isspace(*ptr)) {
                break;
            }
            ptr++;

            continue;
        }
        if (ptr && (*ptr == '-')) {
            ptr++;
            end = atoi(ptr);
            while (ptr && isdigit(*ptr)) {
                ptr++;
            }
            for (i = start; i <= end; i++) {
                if ((i < 9) || (i > 23)) { ///valid port num 9-23
                    return -1;
                }
                if (0 == flag) {
                    flag = 1;
                    len += sprintf(dst + len, "xe%d", i - 1);
                } else {
                    len += sprintf(dst + len, ",xe%d", i - 1);
                }
            }
            if (ptr && (*ptr == ',')) {
                ptr++;
            }
            if ((NULL == ptr) || (*ptr == '\0') || isspace(*ptr)) {
                break;
            }
        }
    }
    //printf("dst:%s\n", dst);
    if (!flag) {
        return -1;
    }

    return 0;
}


DEFUN(set_action_drop, set_action_drop_cmd, "set action ACTIONID drop",
        "Set action <0~255> drop\r\n"
        "module name\n"
        "action id <0~255>\n"
        "action behavior\n")
{
    int node;
    int recode0, recode1;
    int actionid;
    char *cmd_tmp;
    FILE *fp_np_config;
    FILE *fp_np_config_tmp;
    char rule_str[128] = {0};
    char rule_str_tmp[128] = {0};
    int  rule_flag = 0;
    char cmd_str[128] = "action set ";

    cmd_tmp = strstr(vty->buf, "action ");
    cmd_tmp += strlen("action ");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    actionid = atoi(cmd_tmp);
    if ((actionid < 0) || (actionid > 255)) {
        vty_out(vty, "Invalid actionid%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + 11, "%d drop", actionid);
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                //memcpy(rule_str_tmp, cmd_str, strlen(cmd_str) - 5);
                sprintf(rule_str_tmp, "action set %d ", actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(cmd_str) - 5)) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP0);
                system(START_CONFIG_FILE_BACK_NP0);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                //memcpy(rule_str_tmp, cmd_str, strlen(cmd_str) - 5);
                sprintf(rule_str_tmp, "action set %d ", actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(cmd_str) - 5)) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP1);
                system(START_CONFIG_FILE_BACK_NP1);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    return CMD_SUCCESS;
}

DEFUN(set_action_stat_port, set_action_stat_port_cmd,
        "set action ACTIONID transmit port PORTID",
        "Set action <0~255> transmit port <9~23>\r\n"
        "module name\n"
        "action id <0~255>\n"
        "action behavior\n"
        "action property\n"
        "bcm port (valid port 9~23)")
{
    int node;
    int recode0, recode1;
    int actionid;
    char tmp_str[128];
    char *cmd_tmp, *cmd_port;
    FILE *fp_np_config;
    FILE *fp_np_config_tmp;
    char rule_str[128] = {0};
    char rule_str_tmp[128] = {0};
    int  rule_flag = 0;
    char cmd_str[128] = "action set ";

    cmd_tmp = strstr(vty->buf, "action ");
    cmd_tmp += strlen("action ");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    actionid = atoi(cmd_tmp);
    if ((actionid < 0) || (actionid > 255)) {
        vty_out(vty, "Invalid actionid%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
//    str_data[i] = '\0';
    cmd_port = strstr(vty->buf, "port ");
    if(NULL == cmd_port) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_port += 5;
    if(NULL == cmd_port) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    memset(tmp_str, 0x00, 128);
    if (action_port_str_transfer(cmd_port, tmp_str)) {
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        vty_out(vty, "valid port:9-23%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + strlen(cmd_str), "%d transmit port=%s", actionid, tmp_str);
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                //memcpy(rule_str_tmp, cmd_str, strlen(cmd_tmp) + 11);
                sprintf(rule_str_tmp, "action set %d ", actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(cmd_tmp) + 11)) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP0);
                system(START_CONFIG_FILE_BACK_NP0);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                //memcpy(rule_str_tmp, cmd_str, strlen(cmd_tmp) + 11);
                sprintf(rule_str_tmp, "action set %d ", actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(cmd_tmp) + 11)) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP1);
                system(START_CONFIG_FILE_BACK_NP1);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(set_action_stat_port_hash_item, set_action_stat_port_hash_item_cmd,
        "set action ACTIONID transmit port PORTID hash HASH HASHITEM",
        "Set action <0~255> transmit port <9~23> hash hwcrc sdsdp\r\n"
        "module name\n"
        "action id <0~255>\n"
        "action behavior\n"
        "action property\n"
        "bcm port (valid port 9~23)\n"
        "action transmit property\n"
        "transmit hash method(rr, hwcrc)\n"
        "hash item:(sip, dip, sdip, sport, dport, ss, dd,sdsdp)")
{
    int node;
    int recode0, recode1;
    int actionid;
    char str_data[128], tmp_str[128];
    FILE *fp_np_config;
    FILE *fp_np_config_tmp;
    char rule_str[128] = {0};
    char rule_str_tmp[128] = {0};
    int  rule_flag = 0;
    char method[16], item[16]; 
    char *cmd_tmp, *cmd_port, *ptr;
    char cmd_str[128] = "action set ";

    cmd_tmp = strstr(vty->buf, "action ");
    if (NULL == cmd_tmp) {
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_tmp += strlen("action ");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    actionid = atoi(cmd_tmp);
    if ((actionid < 0) || (actionid > 255)) {
        vty_out(vty, "Invalid actionid%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_port = strstr(vty->buf, "port ");
    if (NULL == cmd_tmp) {
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_port += strlen("port ");
    if (NULL == cmd_tmp) {
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    if(sizeof(vty->buf) > 64) {
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    memset(tmp_str, 0x00, 128);
    if (action_port_str_transfer(cmd_port, tmp_str)) {
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        vty_out(vty, "valid port:9-23%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_tmp = strstr(vty->buf, "hash ");
    cmd_tmp += strlen("hash ");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    ptr = cmd_tmp;
    while (ptr && isalpha(*ptr)) {
        ptr++;
    }
    memset(method, 0x00, 16);
    memset(item, 0x00, 16);
    memcpy(method, cmd_tmp, ptr - cmd_tmp);
    cmd_tmp = ptr;
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + strlen(cmd_str), "%d transmit port=%s hash_method=%s hash_item=%s", actionid, tmp_str, method, cmd_tmp);
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                //memcpy(rule_str_tmp, cmd_str, strlen(str_data) + 11);
                sprintf(rule_str_tmp, "action set %d ", actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(str_data) + 11)) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP0);
                system(START_CONFIG_FILE_BACK_NP0);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                //memcpy(rule_str_tmp, cmd_str, strlen(str_data) + 11);
                sprintf(rule_str_tmp, "action set %d ", actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(str_data) + 11)) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP1);
                system(START_CONFIG_FILE_BACK_NP1);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(set_action_stat_port_flow, set_action_stat_port_flow_cmd,
        "set action ACTIONID mirror vlanid VLANID flow (all|up|down)",
        "Set action <0~255> mirror vlanid <1~4069> flow (all|up|down)\r\n"
        "module name\n"
        "action id <0~255>\n"
        "action behavior\n"
        "action property\n"
        "vlanid(1~4069) add by np"
        "action property\n"
        "flow direction(all,up,down)")
{
    int node;
    int recode0, recode1;
    int actionid, vlanid;
    char *cmd_tmp, *flow;
    FILE *fp_np_config;
    FILE *fp_np_config_tmp;
    char rule_str[128] = {0};
    char rule_str_tmp[128] = {0};
    int  rule_flag = 0;
    char cmd_str[128] = "action set ";

    cmd_tmp = strstr(vty->buf, "action ");
    cmd_tmp += strlen("action ");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    actionid = atoi(cmd_tmp);
    if ((actionid < 0) || (actionid > 255)) {
        vty_out(vty, "Invalid actionid(%d)%s", actionid, VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    cmd_tmp = strstr(vty->buf, "vlanid ");
    cmd_tmp += strlen("vlanid ");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (!isdigit(*cmd_tmp)) {
        vty_out(vty, "Invalid action command%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    vlanid = atoi(cmd_tmp);
    if ((vlanid <= 0) || (vlanid > 4069)) {
        vty_out(vty, "Invalid vlanid(%d)%s", vlanid, VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    flow = strstr(vty->buf, "flow");
    if (flow) {
        flow += strlen("flow");
        while(isspace(*flow)) {
            flow++;
        }
        sprintf(cmd_str + 11, "%d mirror vlanid=%d flow_dir=%s", actionid, vlanid, flow);
    } else {
        sprintf(cmd_str + 11, "%d mirror vlanid=%d", actionid, vlanid);
    }
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
   
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                sprintf(rule_str_tmp, "action set %d ", actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(rule_str_tmp))) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP0);
                system(START_CONFIG_FILE_BACK_NP0);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
            if (NULL == fp_np_config) {
                fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"w+");
                fprintf(fp_np_config, "%s\n", cmd_str);
                fclose(fp_np_config);    
            } else {
                sprintf(rule_str_tmp, "action set %d ", actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(rule_str_tmp))) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);    
                    } else {
                        fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                        rule_flag = 1;
                    }  
                    memset(rule_str, 0, sizeof(rule_str));
                }
                if (0 == rule_flag) {
                    fprintf(fp_np_config_tmp, "%s\n", cmd_str);
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP1);
                system(START_CONFIG_FILE_BACK_NP1);
                system("sync");
            }
            vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}


DEFUN(show_action_stat_all, show_action_stat_all_cmd, "show action info", "Displays all action info")
{
    int node;
    int recode0, recode1;
    char *cmd_str = "action show info all";

    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(show_action_stat_actionid, show_action_stat_actionid_cmd, "show action info ACTIONID",
        "Displays all action info\r\n"
        "module name\n"
        "module function name\n"
        "action id <0~255>\n")
{
    int node;
    int recode0, recode1;
    int actionid;
    char* cmd_tmp;
    char cmd_str[128] = "action show info ";

    cmd_tmp = strstr(vty->buf, "info");
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_tmp += strlen("info ");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    actionid = atoi(cmd_tmp);
    if ((actionid < 0) || (actionid > 255)) {
        vty_out(vty, "Invalid actionid%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + strlen(cmd_str), "%d", actionid);                            
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }
    
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        }
    }

    if (!recode0 && !recode1) {
        vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN(delete_action_actionid, delete_action_actionid_cmd, "delete action ACTIONID", 
        "delete action by action id\r\n"
        "module name\n"
        "action id <0~255>\n")
{
    int node;
    int recode0, recode1;
    int actionid;
    char* cmd_tmp;
    FILE *fp_np_config;
    FILE *fp_np_config_tmp;
    char rule_str[128] = {0};
    char rule_str_tmp[128] = {0};
    char cmd_str[128] = "action delete ";

    cmd_tmp = strstr(vty->buf, "action ");
    if(NULL == cmd_tmp){
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    cmd_tmp += strlen("action ");
    while (cmd_tmp && isspace(*cmd_tmp)) {
        cmd_tmp++;
    }
    if (NULL == cmd_tmp) {
        vty_out(vty, "Invalid command%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    actionid = atoi(cmd_tmp);
    if ((actionid < 0) || (actionid > 255)) {
        vty_out(vty, "Invalid actionid%s", VTY_NEWLINE);
        return CMD_SUCCESS;    
    }
    sprintf(cmd_str + strlen(cmd_str), "%d", actionid);
    if (vty->node == NPX_NODE) {
        node = 0x03;
    } else if(vty->node == NP0_NODE) {
        node = 0x01;
    } else {
        node = 0x02;
    }
    
    recode0 = 0;
    if (node & 0x01) {
        vty_out(vty, "np0:%s", VTY_NEWLINE);
        recode0 = aclk_cmd_send_recv(vty, 0, cmd_str);
        if (recode0) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
            if (NULL != fp_np_config) {
                sprintf(rule_str_tmp, "action set %d", actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(rule_str_tmp))) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);
                    } 
                    memset(rule_str, 0, sizeof(rule_str));
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP0);
                system(START_CONFIG_FILE_BACK_NP0);
                system("sync");
                vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
            }
        }
    }
    recode1 = 0;
    if (node & 0x02) {
        vty_out(vty, "np1:%s", VTY_NEWLINE);
        recode1 = aclk_cmd_send_recv(vty, 1, cmd_str);
        if (recode1) {
            vty_out(vty, "%s failed%s", vty->buf, VTY_NEWLINE);
        } else {
            fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
            if (NULL != fp_np_config) {
                sprintf(rule_str_tmp, "action set %d", actionid);
                fp_np_config_tmp = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME_TMP,"w+"); 
                while(NULL != (fgets(rule_str, 128, fp_np_config)))
                {
                    if (0 != strncmp(rule_str, rule_str_tmp, strlen(rule_str_tmp))) {
                        fprintf(fp_np_config_tmp, "%s", rule_str);
                    } 
                    memset(rule_str, 0, sizeof(rule_str));
                }
                fclose(fp_np_config);
                fclose(fp_np_config_tmp);
                memset(rule_str, 0, sizeof(rule_str_tmp));
                system(START_CONFIG_FILE_DEL_NP1);
                system(START_CONFIG_FILE_BACK_NP1);
                system("sync");
                vty_out(vty, "%s ok%s", vty->buf, VTY_NEWLINE);
            }
        }
    }

    return CMD_SUCCESS;
}
void np_init(void)
{

    install_node(&npx_node, NULL);
    install_node(&np0_node, NULL);
    install_node(&np1_node, NULL);

    ///view node command
    install_element(VIEW_NODE, &cpux_cmd);
    install_element(VIEW_NODE, &cpu0_cmd);
    install_element(VIEW_NODE, &cpu1_cmd);
    install_element(VIEW_NODE, &display_np_status_cmd);
   
    ///npx command
    install_element(NPX_NODE, &show_np_status_cmd);
    install_element(NPX_NODE, &np_exit_cmd);
    install_element(NPX_NODE, &np_list_cmd);

    /// load rule command
    install_element(NPX_NODE, &load_np_rule_cmd);

    ///npx hal module command
    install_element(NPX_NODE, &show_np_pool_cmd);      
    install_element(NPX_NODE, &show_np_packet_cmd);
    install_element(NPX_NODE, &clear_np_packet_cmd);
    install_element(NPX_NODE, &show_np_pip_cmd);
    install_element(NPX_NODE, &show_np_pip_stat_cmd);
    install_element(NPX_NODE, &show_np_pip_stat_port_cmd);
    install_element(NPX_NODE, &clear_np_pip_cmd);
    install_element(NPX_NODE, &show_np_port_cmd);
    install_element(NPX_NODE, &show_np_info_cmd);
    install_element(NPX_NODE, &show_np_vlanout_all_cmd);
    install_element(NPX_NODE, &show_np_vlanout_vlanid_cmd);
    install_element(NPX_NODE, &clear_np_vlanout_all_cmd);
    install_element(NPX_NODE, &clear_np_vlanout_vlanid_cmd);
    install_element(NPX_NODE, &show_np_packet_drop_cmd);
    install_element(NPX_NODE, &clear_np_packet_drop_cmd);

    ///npx decap module command
    install_element(NPX_NODE, &show_decap_num_cmd);
    install_element(NPX_NODE, &clear_decap_stat_cmd); 

    ///npx connect module command
    install_element(NPX_NODE, &show_connect_timeout_cmd);
    install_element(NPX_NODE, &set_connect_timeout_cmd);
    install_element(NPX_NODE, &show_connect_stat_cmd);
    install_element(NPX_NODE, &show_connect_table_cmd);

    ///npx manager module command
    install_element(NPX_NODE, &show_app_stat_cmd);
    install_element(NPX_NODE, &show_app_stat_rulename_cmd);
    install_element(NPX_NODE, &show_app_stat_rulenum_cmd);  
    install_element(NPX_NODE, &show_app_match_top_cmd);
    install_element(NPX_NODE, &clear_app_stat_cmd); 

    ///npx hfa module command
    install_element(NPX_NODE, &show_hfa_stat_cmd);
    
    ///npx rule module command
    install_element(NPX_NODE, &show_rule_rulename_cmd);
    install_element(NPX_NODE, &show_rule_ruleid_cmd);
    install_element(NPX_NODE, &show_rule_appidx_cmd);
    install_element(NPX_NODE, &show_rule_rulenum_cmd);
    install_element(NPX_NODE, &set_rule_ruleid_cmd);
    install_element(NPX_NODE, &set_rule_rulename_cmd);
    install_element(NPX_NODE, &set_rule_level_rulename_cmd);
    install_element(NPX_NODE, &set_rule_level_ruleid_cmd);
    install_element(NPX_NODE, &set_rule_level_onoff_cmd);

    ///npx action module command
    install_element(NPX_NODE, &show_action_stat_all_cmd);
    install_element(NPX_NODE, &show_action_stat_actionid_cmd);
    install_element(NPX_NODE, &set_action_drop_cmd);
    install_element(NPX_NODE, &set_action_stat_port_cmd);
    install_element(NPX_NODE, &set_action_stat_port_hash_item_cmd);
    install_element(NPX_NODE, &set_action_stat_port_flow_cmd);
    install_element(NPX_NODE, &delete_action_actionid_cmd);
    
    ///np0 command
    install_element(NP0_NODE, &show_np_status_cmd);
    install_element(NP0_NODE, &np_exit_cmd);
    install_element(NP0_NODE, &np_list_cmd);
    
    /// load rule command
    install_element(NP0_NODE, &load_np_rule_cmd);

    ///np0 hal module command
    install_element(NP0_NODE, &show_np_pool_cmd);   
    install_element(NP0_NODE, &show_np_packet_cmd);  
    install_element(NP0_NODE, &clear_np_packet_cmd);  
    install_element(NP0_NODE, &show_np_pip_cmd);
    install_element(NP0_NODE, &show_np_pip_stat_cmd);
    install_element(NP0_NODE, &show_np_pip_stat_port_cmd);
    install_element(NP0_NODE, &clear_np_pip_cmd);
    install_element(NP0_NODE, &show_np_port_cmd);
    install_element(NP0_NODE, &show_np_info_cmd);
    install_element(NP0_NODE, &show_np_vlanout_all_cmd);
    install_element(NP0_NODE, &show_np_vlanout_vlanid_cmd);
    install_element(NP0_NODE, &clear_np_vlanout_all_cmd);
    install_element(NP0_NODE, &clear_np_vlanout_vlanid_cmd);
    install_element(NP0_NODE, &show_np_packet_drop_cmd);
    install_element(NP0_NODE, &clear_np_packet_drop_cmd);

    /// np decap module command
    install_element(NP0_NODE, &show_decap_num_cmd);
    install_element(NP0_NODE, &clear_decap_stat_cmd); 
    
    /// np connect module command
    install_element(NP0_NODE, &show_connect_timeout_cmd);
    install_element(NP0_NODE, &set_connect_timeout_cmd);
    install_element(NP0_NODE, &show_connect_stat_cmd);
    install_element(NP0_NODE, &show_connect_table_cmd);

    /// np manager module command
    install_element(NP0_NODE, &show_app_stat_cmd);    
    install_element(NP0_NODE, &show_app_stat_rulename_cmd);
    install_element(NP0_NODE, &show_app_stat_rulenum_cmd);
    install_element(NP0_NODE, &clear_app_stat_cmd); 
    install_element(NP0_NODE, &show_app_match_top_cmd);
   
    ///np hfa module command
    install_element(NP0_NODE, &show_hfa_stat_cmd);
   
    ///np rule module command
    install_element(NP0_NODE, &show_rule_rulename_cmd);
    install_element(NP0_NODE, &show_rule_ruleid_cmd);
    install_element(NP0_NODE, &show_rule_appidx_cmd);
    install_element(NP0_NODE, &show_rule_rulenum_cmd);
    install_element(NP0_NODE, &set_rule_ruleid_cmd);
    install_element(NP0_NODE, &set_rule_rulename_cmd);
    install_element(NP0_NODE, &set_rule_level_rulename_cmd);
    install_element(NP0_NODE, &set_rule_level_ruleid_cmd);
    install_element(NP0_NODE, &set_rule_level_onoff_cmd);
    
    ///np action module command
    install_element(NP0_NODE, &show_action_stat_all_cmd);
    install_element(NP0_NODE, &show_action_stat_actionid_cmd);
    install_element(NP0_NODE, &set_action_drop_cmd);
    install_element(NP0_NODE, &set_action_stat_port_cmd);
    install_element(NP0_NODE, &set_action_stat_port_hash_item_cmd);
    install_element(NP0_NODE, &set_action_stat_port_flow_cmd);
    install_element(NP0_NODE, &delete_action_actionid_cmd);

    ///np1 command
    install_element(NP1_NODE, &show_np_status_cmd);
    install_element(NP1_NODE, &np_exit_cmd);
    install_element(NP1_NODE, &np_list_cmd);
    install_element(NP1_NODE, &load_np_rule_cmd);   
    
    ///np hal module command
    install_element(NP1_NODE, &show_np_pool_cmd);   
    install_element(NP1_NODE, &show_np_packet_cmd);
    install_element(NP1_NODE, &clear_np_packet_cmd);
    install_element(NP1_NODE, &show_np_pip_cmd);
    install_element(NP1_NODE, &show_np_pip_stat_cmd);
    install_element(NP1_NODE, &show_np_pip_stat_port_cmd);
    install_element(NP1_NODE, &clear_np_pip_cmd);
    install_element(NP1_NODE, &show_np_port_cmd);
    install_element(NP1_NODE, &show_np_info_cmd);
    install_element(NP1_NODE, &show_np_vlanout_all_cmd);
    install_element(NP1_NODE, &show_np_vlanout_vlanid_cmd);
    install_element(NP1_NODE, &clear_np_vlanout_all_cmd);
    install_element(NP1_NODE, &clear_np_vlanout_vlanid_cmd);
    install_element(NP1_NODE, &show_np_packet_drop_cmd);
    install_element(NP1_NODE, &clear_np_packet_drop_cmd);
    
    ///np decap module command
    install_element(NP1_NODE, &show_decap_num_cmd);
    install_element(NP1_NODE, &clear_decap_stat_cmd); 
    
    ///np connect module command
    install_element(NP1_NODE, &show_connect_timeout_cmd);
    install_element(NP1_NODE, &set_connect_timeout_cmd);
    install_element(NP1_NODE, &show_connect_stat_cmd);
    install_element(NP1_NODE, &show_connect_table_cmd);
    
    ///np manager module command
    install_element(NP1_NODE, &show_app_stat_cmd);
    install_element(NP1_NODE, &show_app_stat_rulename_cmd);
    install_element(NP1_NODE, &show_app_stat_rulenum_cmd); 
    install_element(NP1_NODE, &show_app_match_top_cmd);
    install_element(NP1_NODE, &clear_app_stat_cmd);
    
    ///np hfa module command
    install_element(NP1_NODE, &show_hfa_stat_cmd);
   
    ///np rule module command
    install_element(NP1_NODE, &show_rule_rulename_cmd);
    install_element(NP1_NODE, &show_rule_ruleid_cmd);
    install_element(NP1_NODE, &show_rule_appidx_cmd);
    install_element(NP1_NODE, &show_rule_rulenum_cmd);
    install_element(NP1_NODE, &set_rule_ruleid_cmd);
    install_element(NP1_NODE, &set_rule_rulename_cmd);
    install_element(NP1_NODE, &set_rule_level_rulename_cmd);
    install_element(NP1_NODE, &set_rule_level_ruleid_cmd);
    install_element(NP1_NODE, &set_rule_level_onoff_cmd);
    
    ///np action module command
    install_element(NP1_NODE, &show_action_stat_all_cmd);
    install_element(NP1_NODE, &show_action_stat_actionid_cmd);
    install_element(NP1_NODE, &set_action_drop_cmd);
    install_element(NP1_NODE, &set_action_stat_port_cmd);
    install_element(NP1_NODE, &set_action_stat_port_hash_item_cmd);
    install_element(NP1_NODE, &set_action_stat_port_flow_cmd);
    install_element(NP1_NODE, &delete_action_actionid_cmd);
    
    g_sock_fd = aclk_sock_open(DEFAULT_SERVER, DEFAULT_PORT);
    if (-1 == g_sock_fd) {
        printf("aclk sock open error\n");
        exit(-1);
    }
    printf("socket connect ok\n");

    return;
}
