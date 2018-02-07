/**
 * @Filename: test.c
 * @Brief   :
 * @Author  : zheng.lin
 * @Version : 1.0.0
 * @Date    : 07/28/2016 03:54:03 PM
 */
#include <stdio.h>

#include "loadxml.h"
#include "send_graph_rule.h"

rule_t *g_xml_rule_info;
aclk_pide_config_t max_rule_num;
rule_name_id_t rule_name_index[4096];
void hexdump(void *data, int len)
{
    int i;
    unsigned char *ptr = (unsigned char *)data;

    for (i = 0; i < len; i++) {
        printf("%02x ", ptr[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }

    return;
}

int aclk_load_rule_init(int sock , int octeon_id, unsigned short key_index_num)
{
    FILE *fp;
    int rlen;
    aclk_dpi_cmd_t *req, *res;
    uint32_t len, count;   
    int recode, graph_size;
    char sdata[MEM_SIZE], rdata[MEM_SIZE];

    count = 0;
    memset(sdata, 0x00, MEM_SIZE);
    req = (aclk_dpi_cmd_t *)sdata;    
    //memset(sdata, 0, sizeof(aclk_dpi_cmd_t));
    req->magic_num = htonl(ACLK_CMD_MAGIC_NUM);
    req->opcode = htonl(ACLK_MODULE_XML_RULE | ACLK_XML_RULE_CMD_INIT);    
    printf("init req->opcode:%X\n",req->opcode);
    req->recode = 0;
    len = sizeof(aclk_pide_config_t);
    req->len = htonl(len + sizeof(octeon_id));
    max_rule_num.max_protos_in_one_hfa_key = htons(8);
    max_rule_num.max_protos_in_one_ip = htons(8);
    max_rule_num.max_protos_in_one_port = htons(8);
    max_rule_num.special_hfa_key = htons(key_index_num);
    memcpy(req->data, &max_rule_num, len);
    *((unsigned int *)(req->data + len)) = htonl(octeon_id);
    len += sizeof(octeon_id);
    rlen = MEM_SIZE;
    if (aclk_comm_send_and_recv(sock, sdata, len + sizeof(aclk_dpi_cmd_t), rdata, &rlen)) {
        printf("send or recv error\n");
        close(sock);
        return -1;
    }
    res = (aclk_dpi_cmd_t *)rdata;
    recode = ntohl(res->recode);
    if (recode) {
        printf("hfa init error\n");
        close(sock);
        return -1;
    } 
    fp = fopen(XML_RULE_DEFAULT_PATH XML_RULE_GRAPH_FILE, "r");  
    fseek(fp,0,SEEK_END);
    graph_size = ftell(fp);
    memset(sdata, 0x00, MEM_SIZE);
    req = (aclk_dpi_cmd_t *)sdata;    
    //memset(sdata, 0, sizeof(aclk_dpi_cmd_t));
    req->magic_num = htonl(ACLK_CMD_MAGIC_NUM);
    req->opcode = htonl(ACLK_MODULE_PIDE_HFA | ACLK_HFA_CMD_INIT);   
    printf("init req->opcode:%X\n",req->opcode);
    req->recode = 0;
    len = sizeof(hfa_init_t);
    req->len = htonl(len + sizeof(octeon_id));
    hfa_init_t *ptr_init;
    ptr_init = (hfa_init_t *)(req->data);
    strcpy(ptr_init->graph_name, "graph");
    printf("graph size:%d\n", graph_size);
    ptr_init->graph_size = htonl(graph_size);
    *((unsigned int *)(req->data + len)) = htonl(octeon_id);
    len += sizeof(octeon_id);
    //memcpy(req->data, &ptr_init, sizeof(hfa_init_t));
    rlen = MEM_SIZE;   
    if (aclk_comm_send_and_recv(sock, sdata, len + sizeof(aclk_dpi_cmd_t), rdata, &rlen)) {
        printf("comm pci send or recv error\n");
        close(sock);
        return -1;
    }
    res = (aclk_dpi_cmd_t *)rdata;
    recode = ntohl(res->recode);
    if (recode) {
        printf("hfa init error\n");
        close(sock);
        return -1;
    } 
    fclose(fp);      

    return 0;
}

int aclk_load_config_info(int sock, int octeon_id)
{
    int recode, more_flag;
    int slen, rlen;
    aclk_dpi_cmd_t *req, *res;
    char sdata[MEM_SIZE], rdata[MEM_SIZE];
    FILE *fp_np_config;
    char cmd_str[128];
    if (0 == octeon_id)
    {
        fp_np_config = fopen(SYS_FLASH_PATH START_NP0_CONF_FILE_NAME,"r+");
    } else {
        fp_np_config = fopen(SYS_FLASH_PATH START_NP1_CONF_FILE_NAME,"r+");
    }
    if (NULL != fp_np_config) {
        //printf("open config info error");
        //return -1;   
        //} else {
        while (NULL != (fgets(cmd_str, 128, fp_np_config))) 
        {
            more_flag = 0;
            memset(sdata, 0x00, MEM_SIZE);
            req = (aclk_dpi_cmd_t *)sdata;
            req->magic_num = htonl(ACLK_CMD_MAGIC_NUM);
            req->opcode = htonl(ACLK_MODULE_COMMAND | ACLK_COMMAND_CMD_SEND_STRING);
            req->recode = 0;
            slen = strlen(cmd_str);
            req->len = htonl(slen + sizeof(octeon_id));
            memcpy(req->data, cmd_str, slen);
            *((unsigned int *)(req->data + slen)) = htonl(octeon_id);
            slen += sizeof(octeon_id);
            rlen = MEM_SIZE;
            if (aclk_comm_send_and_recv(sock, sdata, slen + sizeof(aclk_dpi_cmd_t), rdata, &rlen)) {
                printf("load config info error");
                return -1;
            }
            res = (aclk_dpi_cmd_t *)rdata;
            recode = ntohl(res->recode);
            rlen = ntohl(res->len);
            if (recode || rlen > MEM_SIZE) {
                printf("load config info error");
                return -1;
            }
            if (0 == strncmp((char *)(res->data), "Invalid", strlen("Invalid"))) {
                //if (rlen) {
                //    vty_out(vty, "%s", res->data);
                //}
                //vty_out(vty, "%s error%s", vty->buf, VTY_NEWLINE);
                return -1;
            }           
            memset(cmd_str, 0,sizeof(cmd_str));
        }
    }
    return 0;
}


int aclk_load_rule_info(int sock, int octeon_id, int xml_info_num)
{
    int rlen, ssize;
    aclk_dpi_cmd_t *req, *res;
    uint32_t len, rule_head_len, opcode, magic_num;   
    int recode, count;
    char sdata[MEM_SIZE], rdata[MEM_SIZE];
    
    count = 0;
    /*
    sock = aclk_sock_open(srv_ip, port);
    if (-1 == sock) {
        printf("aclk sock open error\n");
        return -1;
    }
    */
    do {     
        memset(sdata, 0x00, MEM_SIZE);
        req = (aclk_dpi_cmd_t *)sdata;
        //memset(sdata, 0, sizeof(aclk_dpi_cmd_t));
        req->magic_num = htonl(ACLK_CMD_MAGIC_NUM);
        req->opcode = htonl(ACLK_MODULE_XML_RULE | ACLK_XML_RULE_CMD_ADD);
        req->recode = 0;
        rule_head_len = 38;
        memcpy(req->data, &g_xml_rule_info[count], rule_head_len);
        *(int*)(req->data + rule_head_len) = htonl(g_xml_rule_info[count].ip_num);
        ssize = rule_head_len + 4;
        //printf("ssize:%d\n",ssize);
        if (0 != g_xml_rule_info[count].ip_num) {          
            memcpy(req->data + ssize, &g_xml_rule_info[count].ip, sizeof(rule_ip_info_t)*g_xml_rule_info[count].ip_num);
            ssize = ssize + sizeof(rule_ip_info_t)*g_xml_rule_info[count].ip_num;
            *(int*)(req->data + ssize) = htonl(g_xml_rule_info[count].port_num);
            ssize = ssize + 4 ;
            //printf("port num:%d,g_xml_rule_info[count].port_num:%d,ssize:%d\n",*(int*)(req->data + ssize -4 ),g_xml_rule_info[count].port_num,ssize);
            if (0 != g_xml_rule_info[count].port_num) {
                 memcpy(req->data + ssize, &g_xml_rule_info[count].port, sizeof(rule_port_info_t)*g_xml_rule_info[count].port_num);
                 *(int*)(req->data + ssize + sizeof(rule_port_info_t)*g_xml_rule_info[count].port_num) = htonl(g_xml_rule_info[count].key_num);
                 ssize = ssize + sizeof(rule_port_info_t)*g_xml_rule_info[count].port_num + 4 ;
                 if (0 != g_xml_rule_info[count].key_num) {
                     memcpy(req->data + ssize, &g_xml_rule_info[count].key, sizeof(rule_key_info_t)*g_xml_rule_info[count].key_num);                     
                     ssize = ssize + sizeof(rule_key_info_t)*g_xml_rule_info[count].key_num;
                     *(int*)(req->data + ssize) = htonl(g_xml_rule_info[count].pkt_flag);
                     ssize = ssize + 4 ;
                     if (1 == g_xml_rule_info[count].pkt_flag) {
                         memcpy(req->data + ssize, &g_xml_rule_info[count].pkt, sizeof(rule_pkt_info_t)); 
                         ssize = ssize + sizeof(rule_pkt_info_t);
                     }
                 } 
            } else {               
                *(int*)(req->data + ssize ) = htonl(g_xml_rule_info[count].key_num);
                ssize = ssize + 4;
                if (0 != g_xml_rule_info[count].key_num) {
                     memcpy(req->data + ssize, &g_xml_rule_info[count].key, sizeof(rule_key_info_t)*g_xml_rule_info[count].key_num);                      
                     ssize = ssize + sizeof(rule_key_info_t)*g_xml_rule_info[count].key_num;
                     *(int*)(req->data + ssize) = htonl(g_xml_rule_info[count].pkt_flag);
                     ssize = ssize + 4 ;
                     if (1 == g_xml_rule_info[count].pkt_flag) {
                         memcpy(req->data + ssize, &g_xml_rule_info[count].pkt, sizeof(rule_pkt_info_t)); 
                         ssize = ssize + sizeof(rule_pkt_info_t);
                     }
                }
            }
        } else {        
            *(int*)(req->data + ssize ) = htonl(g_xml_rule_info[count].port_num);
            ssize = ssize + 4;
            if (0 != g_xml_rule_info[count].port_num) {
                memcpy(req->data + ssize, &g_xml_rule_info[count].port, sizeof(rule_port_info_t)*g_xml_rule_info[count].port_num);                 
                *(int*)(req->data + ssize + sizeof(rule_port_info_t)*g_xml_rule_info[count].port_num) = htonl(g_xml_rule_info[count].key_num);
                ssize = ssize + sizeof(rule_port_info_t)*g_xml_rule_info[count].port_num + 4;
                if (0 != g_xml_rule_info[count].port_num) {
                     memcpy(req->data + ssize, &g_xml_rule_info[count].key, sizeof(rule_key_info_t)*g_xml_rule_info[count].key_num);                
                     ssize = ssize + sizeof(rule_key_info_t)*g_xml_rule_info[count].key_num;   
                     *(int*)(req->data + ssize) = htonl(g_xml_rule_info[count].pkt_flag);
                     ssize = ssize + 4 ;
                     if (1 == g_xml_rule_info[count].pkt_flag) {
                         memcpy(req->data + ssize, &g_xml_rule_info[count].pkt, sizeof(rule_pkt_info_t)); 
                         ssize = ssize + sizeof(rule_pkt_info_t);
                     }
                }
            } else {
                *(int*)(req->data + ssize ) = htonl(g_xml_rule_info[count].key_num);
                ssize = ssize + 4;
                if (0 != g_xml_rule_info[count].key_num) {
                     memcpy(req->data + ssize, &g_xml_rule_info[count].key, sizeof(rule_key_info_t)*g_xml_rule_info[count].key_num); 
                     ssize = ssize + sizeof(rule_key_info_t)*g_xml_rule_info[count].key_num;
                     *(int*)(req->data + ssize) = htonl(g_xml_rule_info[count].pkt_flag);
                     ssize = ssize + 4 ;
                     if (1 == g_xml_rule_info[count].pkt_flag) {
                         memcpy(req->data + ssize, &g_xml_rule_info[count].pkt, sizeof(rule_pkt_info_t)); 
                         ssize = ssize + sizeof(rule_pkt_info_t);
                     }
                }
            }
        }
        req->len = htonl(ssize + sizeof(octeon_id));
        *((unsigned int *)(req->data + ssize)) = htonl(octeon_id);
        ssize += sizeof(octeon_id);
        rlen = MEM_SIZE;        
        if (aclk_comm_send_and_recv(sock, sdata, ssize + sizeof(aclk_dpi_cmd_t), rdata, &rlen)) {
            printf("%s[%d]: comm send request(0x%x)\n", __func__, __LINE__, ACLK_MODULE_XML_RULE | ACLK_XML_RULE_CMD_ADD);
            return -1;
        }
        count++;
    } while (count < xml_info_num);
    memset(sdata, 0x00, MEM_SIZE);
    req = (aclk_dpi_cmd_t *)sdata;    
    //memset(sdata, 0, sizeof(aclk_dpi_cmd_t));
    req->magic_num = htonl(ACLK_CMD_MAGIC_NUM);
    req->opcode = htonl(ACLK_MODULE_XML_RULE | ACLK_XML_RULE_CMD_END);
    req->recode = 0;
    len = sizeof(pcie_send_end_t);
    req->len = htonl(len + sizeof(octeon_id));
    pcie_send_end_t ptr_rule_end;
    memset(&ptr_rule_end, 0, sizeof(pcie_send_end_t));    
    strcpy(ptr_rule_end.graph_end_flag, "xml_rule_end");
    memcpy(req->data, &ptr_rule_end, len);
    *((unsigned int *)(req->data + len)) = htonl(octeon_id);
    len += sizeof(octeon_id);
    rlen = MEM_SIZE;   
    if (aclk_comm_send_and_recv(sock, sdata, len + sizeof(aclk_dpi_cmd_t), rdata, &rlen)) {
        printf("comm pci send or recv error\n");
        close(sock);
        return -1;
    }
    res = (aclk_dpi_cmd_t *)rdata;
    len = ntohl(res->len);
    opcode = ntohl(res->opcode);
    magic_num = ntohl(res->magic_num);
    recode = ntohl(res->recode);
    printf("len %u, opcode:%x, magic_num:%x, recode:%d\n", len, opcode, magic_num, recode);
    printf("rlen:%d, len:%d\n", rlen, len);    
    
    //close(sock);   
    return 0;
}

int aclk_load_rule_graph(int sock, int octeon_id)
{
    FILE *fp;
    int graph_size;
    int rlen, end_size, ssize;
    aclk_dpi_cmd_t *req, *res;
    uint32_t len, opcode, magic_num, count;
    int recode;
    char sdata[MEM_SIZE], rdata[MEM_SIZE];
    
    count = 0;

    fp = fopen(XML_RULE_DEFAULT_PATH XML_RULE_GRAPH_FILE, "r");  
    fseek(fp,0,SEEK_END);
    graph_size = ftell(fp);
    printf("file size:%d\n", graph_size);
    fseek(fp,0,SEEK_SET);
    end_size = (int)(graph_size%1024);
    count = (int)(graph_size/1024) + 1;   
    printf("read hfa end_size:%d\n",end_size);
    /*
    sock = aclk_sock_open(srv_ip, port);
    if (-1 == sock) {
        printf("aclk sock open error\n");
        return -1;
    }
    */
    do {
        memset(sdata, 0x00, MEM_SIZE);
        req = (aclk_dpi_cmd_t *)sdata;
        //memset(sdata, 0, sizeof(aclk_dpi_cmd_t));
        req->magic_num = htonl(ACLK_CMD_MAGIC_NUM);
        req->opcode = htonl(ACLK_MODULE_PIDE_HFA | ACLK_HFA_CMD_ADD);
        req->recode = 0;
        if (count > 1) {
            ssize = 1024;
            ///req->len = htonl(ssize);
        } else {
            ssize = end_size;
            ///req->len = htonl(ssize);
        }            
        if (1 != fread(req->data, ssize, 1, fp)) {
            printf("read hfa.out file error\n");
            break;
        }
        req->len = htonl(ssize + sizeof(octeon_id));
        *((unsigned int *)(req->data + ssize)) = htonl(octeon_id);
        ssize += sizeof(octeon_id);
        rlen = MEM_SIZE;        
        if (aclk_comm_send_and_recv(sock, sdata, ssize + sizeof(aclk_dpi_cmd_t), rdata, &rlen)) {
            printf("%s[%d]: comm send request(0x%x)\n", __func__, __LINE__, ACLK_MODULE_PIDE_HFA | ACLK_HFA_CMD_ADD);
            return -1;
        }
        count--;
    } while(count > 0);     
    printf("pcie send graph\n");
    memset(sdata, 0x00, MEM_SIZE);
    req = (aclk_dpi_cmd_t *)sdata;    
    //memset(sdata, 0, sizeof(aclk_dpi_cmd_t));
    req->magic_num = htonl(ACLK_CMD_MAGIC_NUM);
    req->opcode = htonl(ACLK_MODULE_PIDE_HFA | ACLK_HFA_CMD_END);
    req->recode = 0;
    len = sizeof(pcie_send_end_t);
    ///req->len = htonl(len);
    pcie_send_end_t ptr_end;
    memset(&ptr_end, 0, sizeof(pcie_send_end_t));    
    strcpy(ptr_end.graph_end_flag, "graph_end");
    memcpy(req->data, &ptr_end, sizeof(pcie_send_end_t));
    req->len = htonl(len + sizeof(octeon_id));
    *((unsigned int *)(req->data + len)) = htonl(octeon_id);
    len += sizeof(octeon_id);
    rlen = MEM_SIZE;   
    if (aclk_comm_send_and_recv(sock, sdata, len + sizeof(aclk_dpi_cmd_t), rdata, &rlen)) {
        printf("comm pci send or recv error\n");
        close(sock);
        return -1;
    }
    res = (aclk_dpi_cmd_t *)rdata;
    len = ntohl(res->len);
    opcode = ntohl(res->opcode);
    magic_num = ntohl(res->magic_num);
    recode = ntohl(res->recode);
    printf("len %u, opcode:%x, magic_num:%x, recode:%d\n", len, opcode, magic_num, recode);
    printf("rlen:%d, len:%d\n", rlen, len);    
    fclose(fp);
    
    //close(sock); 
    return 0;
}

