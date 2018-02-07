/*
 * @Filename: comm.h
 * @Brief   :
 * @Author  : zl 
 * @Version : 1.0.0
 * @Date    : 08/08/2016 03:51:22 PM
 */
#ifndef __Addgraph_H__
#define __Addgraph_H__
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "comm.h"

#ifdef  __cpulsplus
extern "C" {
#endif
#pragma pack(1)

#define ACLK_CMD_MAGIC_NUM  0xdeadbeef
#define ACLK_MODULE_PIDE_HFA  0x620    
#define ACLK_MODULE_XML_RULE  0x650
#define ACLK_CONNECT_CMD_SHOW_COUNTER   1
#define RULE_NUM_MAX 4096
#define MEM_SIZE    2000

/*
typedef struct aclk_dpi_cmd {
    unsigned int   magic_num;
    unsigned int   opcode;
    unsigned int   recode;
    unsigned int   len;
    unsigned char  data[0];
} aclk_dpi_cmd_t;
*/
typedef struct hfa_init{
    char graph_name[16];
    int  graph_size;
} hfa_init_t;

typedef struct pcie_send_end{
    char graph_end_flag[16];
    int  end_flag;
} pcie_send_end_t;

typedef enum {
    ACLK_HFA_CMD_INIT = 1,
    ACLK_HFA_CMD_ADD,
    ACLK_HFA_CMD_END
} aclk_hfa_cmd_t;

typedef enum {
    ACLK_XML_RULE_CMD_INIT = 1,
    ACLK_XML_RULE_CMD_ADD,
    ACLK_XML_RULE_CMD_END
} aclk_xml_rule_cmd_t;

typedef union {
    unsigned int   ipv4;
    unsigned short ipv6[8];
} ip_info_t;

typedef union {
    ip_info_t ip;
    struct {
        ip_info_t sip;
        ip_info_t dip;
    }sdip;
} aclk_pide_sdip_t;


typedef struct rule_ip_info_s{
    unsigned char    ip_ver;
    unsigned char    sdtype;           ///source or dest:0, source:1, dest:2, source and dest:3
    aclk_pide_sdip_t sdip;
} rule_ip_info_t;

typedef union {
    unsigned short port;
    struct {
        unsigned short sport;
        unsigned short dport;
    }sdport;
}aclk_pide_sdport_t;

typedef struct rule_port_info_s {
    unsigned char l4_proto;
    unsigned char sdtype;         ///source or dest:0, source:1, dest:2, source and dest:3
    aclk_pide_sdport_t sdport;
} rule_port_info_t;
 
typedef struct rule_key_info_s{
    unsigned char  k_type:4;
    unsigned char  k_order:4;
    unsigned char  k_len;
    unsigned short  key_id;
    unsigned short s_off;
    unsigned short e_off;
} rule_key_info_t;

typedef struct rule_pkt_info_s {
    unsigned char pkt_proto;
    unsigned char pkt_type;  
    unsigned char pkt_dir; 
    unsigned int  pkt_seq; 
} rule_pkt_info_t;

typedef struct rule_s {
    unsigned char rule_name[32];
    unsigned int rule_id;
    unsigned char level;
    unsigned char action_id;
    int ip_num;
    rule_ip_info_t ip[50];   
    int port_num;    
    rule_port_info_t port[200];   
    int key_num;
    rule_key_info_t key[10];
    int pkt_flag;
    rule_pkt_info_t pkt;
} rule_t;

typedef struct rule_name_id_s {
    unsigned char rule_name[32];
    unsigned int rule_id;
} rule_name_id_t;

typedef struct aclk_pide_config {
    uint16_t max_protos_in_one_ip;
    uint16_t max_protos_in_one_port;
    uint16_t max_protos_in_one_hfa_key;
    uint16_t special_hfa_key;
} aclk_pide_config_t;

extern rule_t *g_xml_rule_info;
extern rule_name_id_t rule_name_index[4096];

int aclk_load_rule_init(int sock , int octeon_id, unsigned short key_index_num);
int aclk_load_rule_info(int sock, int octeon_id, int xml_info_num);
int aclk_load_rule_graph(int sock, int octeon_id);
int aclk_load_config_info(int sock, int octeon_id);

#ifdef  __cpulsplus
}
#endif

#endif

