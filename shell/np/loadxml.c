#include <stdio.h>
#include <string.h>

#include "send_graph_rule.h"
#include "loadxml.h"

unsigned int g_rule_info_num = 0;
unsigned int name_index_num = 0;
unsigned int g_key_rule_num = 0;
static char **g_xml_key_rule;

void keystr_rule(FILE *fp_exp, char *key, int key_offset)
{
    char *key_tmp0=NULL;
    char key_tmp1[30]={0};
    int key_len=0;
    int key_tmp_len0=0;
    int key_tmp_len1=0;
    key_tmp0 =strstr(key,"[");
    if (NULL == key_tmp0) {
        if (1 == key_offset) {
            fprintf(fp_exp, "{@0}\\x{%s}\n",key+2);  
        //} else if (0 == (key_offset + strlen(key + 2) / 2)) {
        //    fprintf(fp_exp, "\\x{%s}$\n",key+2);
        } else {
            fprintf(fp_exp, "\\x{%s}\n",key+2);
        }
    } else {
        key_tmp0 =strstr(key,"[");
        key_tmp_len0 = strlen(key_tmp0);
        key_len = strlen(key);
        if (key_len == key_tmp_len0 + 2) {
            if (1 == key_offset) {
                fprintf(fp_exp, "{@0}[\\x%c%c\\x%c%c]",*(key_tmp0+1),*(key_tmp0+2),*(key_tmp0+3),*(key_tmp0+4));
            } else {            
                fprintf(fp_exp, "[\\x%c%c\\x%c%c]",*(key_tmp0+1),*(key_tmp0+2),*(key_tmp0+3),*(key_tmp0+4));
            }
            key_tmp0 = key_tmp0 + 6;
            while (NULL != key_tmp0) {
                key_len = strlen(key_tmp0);
                strcpy(key_tmp1,key_tmp0);
                if (NULL == strstr(key_tmp0,"[")){
                    //if (0 == (key_offset + strlen(key + 2) / 2)) {
                    //    fprintf(fp_exp, "\\x{%s}$",key_tmp0); 
                    //} else {                   
                        fprintf(fp_exp, "\\x{%s}",key_tmp0); 
                    //}
                    break;
                }else {
                    key_tmp0 = strstr(key_tmp0,"[");
                    key_tmp_len0 = strlen(key_tmp0);
                    if (key_len == key_tmp_len0) {  
                        fprintf(fp_exp, "[\\x%c%c\\x%c%c]",*(key_tmp0+1),*(key_tmp0+2),*(key_tmp0+3),*(key_tmp0+4));
                        key_tmp0 = key_tmp0 +6;
                    } else {
                        strtok(key_tmp1,"[");
                        fprintf(fp_exp, "\\x{%s}",key_tmp1);
                        fprintf(fp_exp, "[\\x%c%c\\x%c%c]",*(key_tmp0+1),*(key_tmp0+2),*(key_tmp0+3),*(key_tmp0+4));
                        key_tmp0 = key_tmp0  + 6;
                        if (NULL== strstr(key_tmp0,"[")) {
                            //if (0 == (key_offset + strlen(key + 2) / 2)) {
                            //    fprintf(fp_exp, "$");    
                            //}
                            break;
                        }
                    }
                }
            }
            fprintf(fp_exp, "\n");
        } else {
            key_tmp_len1 = key_len - key_tmp_len0;
            memset(key_tmp1,0 ,30);
            memcpy(key_tmp1,key,key_tmp_len1);
            if (1 == key_offset) {
                fprintf(fp_exp, "{@0}\\x{%s}",key_tmp1+2);
            } else {              
                fprintf(fp_exp, "\\x{%s}",key_tmp1+2);
            }
            while(NULL != key_tmp0) {
                key_len = strlen(key_tmp0);
                strcpy(key_tmp1,key_tmp0);
                if (NULL == strstr(key_tmp0,"[")) {
                    //if (0 == (key_offset + strlen(key + 2) / 2)) {
                    //    fprintf(fp_exp, "\\x{%s}$",key_tmp0);
                    //} else {
                        fprintf(fp_exp, "\\x{%s}",key_tmp0);
                    //}
                    break;
                } else {
                    key_tmp0 = strstr(key_tmp0,"[");
                    key_tmp_len0 = strlen(key_tmp0);
                    if (key_len == key_tmp_len0) {  
                        fprintf(fp_exp, "[\\x%c%c\\x%c%c]",*(key_tmp0+1),*(key_tmp0+2),*(key_tmp0+3),*(key_tmp0+4));
                        key_tmp0 = key_tmp0 +6;
                    } else {
                        strtok(key_tmp1,"[");
                        fprintf(fp_exp, "\\x{%s}",key_tmp1);
                        fprintf(fp_exp, "[\\x%c%c\\x%c%c]",*(key_tmp0+1),*(key_tmp0+2),*(key_tmp0+3),*(key_tmp0+4));
                        key_tmp0 = key_tmp0  + 6;
                        if (NULL== strstr(key_tmp0,"[")) {
                            //if (0 == (key_offset + strlen(key + 2) / 2)) {
                            //    fprintf(fp_exp, "$");
                            //}
                            break;
                        }
                    }
                }
            }
            fprintf(fp_exp, "\n");
        }
    }

    return;
}

int ipv4str_process(char *ptr, unsigned int *ip, unsigned int *ip_num)
{    
    char *tmp;    
    char startip[20], endip[20];    
    unsigned int i, start, end;    
    struct in_addr addr;       
    *ip_num = 0;    
    while (*ptr) {        
        memset(startip, 0x00, 20);        
        memset(endip, 0x00, 20);                
        if (isspace(*ptr)) {            
            ++ptr;            
            continue;        
        }        
        tmp = ptr;        
        while (*tmp && (*tmp != ',') && (*tmp != '-') && (*tmp != '\0')) {            
            tmp++;        
        }       
        memcpy(startip, ptr, tmp - ptr);        
        //printf("startip:%s\n", startip);        
        if (0 == inet_aton(startip, &addr)) {            
            printf("Invalid ip str\n");            
            return -1;        
        }        
        start = inet_network(startip);        
        ip[*ip_num] = start;        
        (*ip_num)++;
        if (*ip_num >= 50) {
            printf ("ip rang set too big set it less than 50!!!\n");
            return -1;
        }
        if (*tmp == '\0') {            
            break;        
        }        
        ptr = tmp;        
        if (*ptr == ',') {            
            ptr++;           
            continue;        
        }        
        //printf("*ptr:%c\n", *ptr);        
        if (*ptr == '-') {            
            ptr++;            
            tmp = ptr;            
            while (*tmp && (*tmp != ',') && (*tmp != '-') && (*tmp != '\0')) {                
                tmp++;            
            }            
            if (*tmp == '-') {                
                printf("Invalid ip str\n");                
                return -1;            
            }            
            memcpy(endip, ptr, tmp - ptr);            
            //printf("endip:%s\n", endip);            
            if (0 == inet_aton(endip, &addr)) {                
                printf("Invalid ip str\n");                
                return -1;            
            }            
            end = inet_network(endip);            
            //printf("startip:%x, endip:%x\n", start, end);            
            for (i = start + 1; i <= end; i++) {                 
                ip[*ip_num] = i;                
                (*ip_num)++;
                if (*ip_num >= 50) {
                    printf ("ip rang set too big set it less than 50!!!\n");
                    return -1;
                }
            }            
            if (*tmp == '\0') {               
                break;            
            }            
            ptr = tmp;           
            ptr++;        
        }    
    }    
    return 0;
}

int ipv6str_process(char *ptr, unsigned short ip[][8], unsigned int *ip_num)
{    
    char *tmp;    
    char buf[8][10];
    char startip[40], endip[40];    
    unsigned int i;
    unsigned short start[8], end[8];    
    *ip_num = 0;
    while (*ptr) {        
        memset(startip, 0x00, 40);        
        memset(endip, 0x00, 40);                
        if (isspace(*ptr)) {            
            ++ptr;            
            continue;        
        }        
        tmp = ptr;        
        while (*tmp && (*tmp != ',') && (*tmp != '-') && (*tmp != '\0')) {            
            tmp++;        
        }       
        memcpy(startip, ptr, tmp - ptr);  
        //printf("startip %s,len:%d\n\n",startip,strlen(startip));
        sscanf(startip, "%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
        for (i = 0; i < 8; i++) {
            start[i] = strtol(buf[i], NULL, 16);
        }

        memcpy(ip[*ip_num], start, 16);
        //printf("start[6]:%x, start[7]:%x\n", start[6], start[7]);      
        (*ip_num)++;

        if (*ip_num >= 50) {
            printf ("ip rang set too big set it less than 50 !!!\n");
            return -1;
        }
        if (*tmp == '\0') {            
            break;        
        } 
        ptr = tmp;        
        if (*ptr == ',') {            
            ptr++;           
            continue;        
        }        
        //printf("*ptr:%c\n", *ptr);        
        if (*ptr == '-') {            
            ptr++;            
            tmp = ptr;            
            while (*tmp && (*tmp != ',') && (*tmp != '-') && (*tmp != '\0')) {                
                tmp++;            
            }            
            if (*tmp == '-') {                
                printf("Invalid ip str\n");                
                return -1;            
            }            
            memcpy(endip, ptr, tmp - ptr);             
            sscanf(endip, "%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
            for (i = 0; i < 8; i++) {
                end[i] = strtol(buf[i], NULL, 16);
            }
            for (i = start[7] + 1; i <= end[7]; i++) {  
                ip[*ip_num][7] = i;
                memcpy(ip[*ip_num], end, 14);               
                (*ip_num)++;
                if (*ip_num >= 50) {
                    printf ("ip rang set too big set it less than 50!!!\n");
                    return -1;
                }
            }            
            if (*tmp == '\0') {               
                break;            
            }            
            ptr = tmp;           
            ptr++;        
        }    
    }    
    return 0;
}


int portstr_process(char *ptr, uint16_t *port, uint16_t *port_num)
{    
    char *tmp;    
    char startport[20], endport[20];    
    unsigned int i, start, end;    
    
    /*
       if (*ptr == '"') {        
       printf("ptr[0] == \"\n");        
       ++ptr;    
       }    
       */
    *port_num = 0;    
    while (*ptr) {        
        memset(startport, 0x00, 20);        
        memset(endport, 0x00, 20);        
        if (isspace(*ptr)) {            
            ++ptr;            
            continue;        
        }        
        tmp = ptr;        
        while (*tmp && (*tmp != ',') && (*tmp != '-') && (*tmp != '\0')) {            
            tmp++;        
        }        
        memcpy(startport, ptr, tmp - ptr);        
        //printf("startport:%s\n", startport);        
        start = atoi(startport);        
        port[*port_num] = start;       
        (*port_num)++;  
        if (*port_num >= 200) {
            printf ("port rang set too big set it less than 200!!!\n");
            return -1;
        }
        if (*tmp == '\0') {           
            break;        
        }       
        ptr = tmp;       
        if (*ptr == ',') {        
            ptr++;            
            continue;        
        }        ///printf("*ptr:%c\n", *ptr);        
        if (*ptr == '-') {            
            ptr++;            
            tmp = ptr;            
            while (*tmp && (*tmp != ',') && (*tmp != '-') && (*tmp != '\0')) {                
                tmp++;            
            }           
            if (*tmp == '-') {                
                printf("Invalid port str\n");                
                return -1;            
            }            
            memcpy(endport, ptr, tmp - ptr);            
            //printf("endport:%s\n", endport);           
            end = atoi(endport);            
            //printf("startport:%x, endport:%x\n", start, end);            
            for (i = start + 1; i <= end; i++) {                 
                port[*port_num] = i;                
                (*port_num)++;  
                if (*port_num >= 200) {
                    printf ("port rang set too big set it less than 200!!!\n");
                    return -1;
                }
            }            
            if (*tmp == '\0') {                
                break;            
            }            
            ptr = tmp;           
            ptr++;        
        }    
    }    
    return 0;
}


int parseInfo(FILE *fp_exp, xmlDocPtr doc, xmlNodePtr cur, unsigned short key_index_num)
{
    unsigned char* level = NULL; 
    unsigned char* action_id = NULL; 
    
    char* ip = NULL;
    char* src_ip = NULL;
    char* dst_ip = NULL;
    char* ip_version = NULL;
    char* ip_sdtype = NULL;
    unsigned int hex_ip_block[XML_MAX_IP_NUM + 1];
    unsigned short hex_ipv6_block[XML_MAX_IP_NUM + 1][8];
    unsigned int ip_num = 0;

    char* port = NULL;
    int port_count = 0, port_count_s = 0;
    uint16_t hex_port_black[XML_MAX_PORT_NUM + 1];
    uint16_t port_num = 0;
    char* port_type = NULL;     
    char* port_sdtype = NULL;

    char* key = NULL;
    char* key_type = NULL;
    char* key_order = NULL;
    short key_id = 0;
    char* key_hex = NULL;    
    char* key_offset = NULL;
    short k_off = 0 ;
    
    char* key_len = NULL;
    char* key_end = NULL;    
    short k_end = 0 ;
    char* k_repre = NULL;

    char* pkt_proto = NULL;
    char* pkt_type = NULL;
    char* pkt_dir = NULL;
    char* pkt_seq = 0;

    unsigned int key_num_index = 0;
    unsigned int info_key_num = 0;
    unsigned int info_ip_num = 0;   
    unsigned int info_port_num = 0;

    g_xml_rule_info[g_rule_info_num].ip_num = 0;
    g_xml_rule_info[g_rule_info_num].port_num= 0;
    g_xml_rule_info[g_rule_info_num].key_num= 0;

    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
        if (!xmlStrcmp(cur->name, (const xmlChar *)"level")) {
            level = (unsigned char*)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1); 
            if (NULL != level) {
                g_xml_rule_info[g_rule_info_num].level = atoi((char *)level);     
            }
            if (NULL != level) {
                xmlFree((xmlChar*)level);
            }
        }

        if (!xmlStrcmp(cur->name, (const xmlChar *)"action_id")) {
             action_id = (unsigned char*)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);            
             g_xml_rule_info[g_rule_info_num].action_id = *(unsigned char*)action_id;     

             if (NULL != action_id) {
                xmlFree((xmlChar*)action_id);
             }
        }
        
        if (!xmlStrcmp(cur->name, (const xmlChar *)"key")) {    
            key = (char*)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            key_type =(char*)xmlGetProp(cur, (const xmlChar *)"type");
            key_order =(char*)xmlGetProp(cur, (const xmlChar *)"order");
            key_len =(char*)xmlGetProp(cur, (const xmlChar *)"len");
            k_repre = (char*)xmlGetProp(cur, (const xmlChar *)"k_repre");
            key_hex = (char*)xmlGetProp(cur, (const xmlChar *)"hex");
            key_offset = (char*)xmlGetProp(cur, (const xmlChar *)"offset");
            key_end = (char*)xmlGetProp(cur, (const xmlChar *)"end");             
            //printf("keyword: %s type: %s\n", key,key_type);

            if (NULL != key_offset){
                k_off = atoi(key_offset);
            }
            if (NULL != key) {
                if (0 != g_key_rule_num) {
                    for (key_num_index = 0; key_num_index < g_key_rule_num; key_num_index++) {
                        if (0 == strcmp(key, (char*)(g_xml_key_rule[key_num_index]))) {
                            break; 
                        }              
                    }
                }
                //if (k_off < 0) {
                //    printf("key offset:%d, key:%s, k_len:%d\n", k_off, key, strlen(key));
                //}
                ///printf("keyword1: %s, g_xml_key_rule: %s g_key_rule_num:%d i:%d, key_host_index:%d\n", key, g_xml_key_rule[key_num_index-1], g_key_rule_num, key_num_index, key_host_index);
                if (!((0 == strcmp((char *)g_xml_rule_info[g_rule_info_num].rule_name, "im_qq")) && (-1 == k_off))) {
                    if (g_key_rule_num == key_num_index) {
                        key_id = key_index_num + g_key_rule_num + 1;
                        //if (NULL == key_hex || NULL == key_offset) {  
                        if (NULL == key_hex ) {  
                            fprintf(fp_exp, "%s\n",key);                          
                        } else {
                            if (1 == atoi(key_hex)) {
                                keystr_rule(fp_exp, key, k_off);
                            } else {
                                fprintf(fp_exp, "{@0}\\d{%s}\n",key);
                            }              
                        }
                        strcpy((char*)(g_xml_key_rule[g_key_rule_num]), key);
                        g_key_rule_num++;
                    } else {
                        key_id = key_num_index + key_index_num + 1;
                    }
                }
                g_xml_rule_info[g_rule_info_num].key[info_key_num].key_id =  htons(key_id);
                g_xml_rule_info[g_rule_info_num].key_num = info_key_num + 1;
                if (NULL != key_type) {
                    g_xml_rule_info[g_rule_info_num].key[info_key_num].k_type = atoi(key_type);
                } else {
                    g_xml_rule_info[g_rule_info_num].key[info_key_num].k_type = 0;
                }
                if (NULL != key_order) {
                    g_xml_rule_info[g_rule_info_num].key[info_key_num].k_order = atoi(key_order);
                } else {
                    g_xml_rule_info[g_rule_info_num].key[info_key_num].k_order = 0;
                }
                if (NULL != key_len) {
                    g_xml_rule_info[g_rule_info_num].key[info_key_num].k_len = atoi(key_len);
                } else {
                    g_xml_rule_info[g_rule_info_num].key[info_key_num].k_len = 0;
                }
                if (NULL != key_offset) {
                    //if (k_off != 1) {
                        g_xml_rule_info[g_rule_info_num].key[info_key_num].s_off = htons(k_off);
                    //} else {                        
                    //    g_xml_rule_info[g_rule_info_num].key[info_key_num].s_off = 0;          
                    //}         
                }
                if (NULL != key_end) {
                    k_end = atoi(key_end);
                    //if (k_off != 1) {
                        g_xml_rule_info[g_rule_info_num].key[info_key_num].e_off = htons(k_end);
                    //} else {
                    //    g_xml_rule_info[g_rule_info_num].key[info_key_num].e_off = 0;
                    //}
                }
                info_key_num++;
            } 
            if (NULL != key) {             
                xmlFree((xmlChar*)key);
            }
            if (NULL != key_type) {             
                xmlFree((xmlChar*)key_type);
            }
            if (NULL != key_order) {             
                xmlFree((xmlChar*)key_order);
            }
            if (NULL != k_repre) {             
                xmlFree((xmlChar*)k_repre);
            }
            if (NULL != key_hex) {             
                xmlFree((xmlChar*)key_hex);
            }             
            if (NULL != key_offset) {             
                xmlFree((xmlChar*)key_offset);
            }             
            if (NULL != key_end) {             
                xmlFree((xmlChar*)key_end);
            } 
            if (NULL != key_len) {             
                xmlFree((xmlChar*)key_len);
            } 
            ///if (NULL != action_id) {             
            ///    xmlFree((xmlChar*)action_id);
            ///}
            ///if (NULL != level) {             
            ///    xmlFree((xmlChar*)level);
            ///}
        }

        if(!xmlStrcmp(cur->name, (const xmlChar *)"packet")) { 
            pkt_proto = (char*)xmlGetProp(cur, (const xmlChar *)"proto");           
            pkt_type = (char*)xmlGetProp(cur, (const xmlChar *)"type");
            pkt_dir = (char*)xmlGetProp(cur, (const xmlChar *)"dir");
            pkt_seq = (char*)xmlGetProp(cur, (const xmlChar *)"seq");
            g_xml_rule_info[g_rule_info_num].pkt_flag = 1;
            if (NULL != pkt_proto) {
                if (0 == strcmp("TCP", pkt_proto)) {
                    g_xml_rule_info[g_rule_info_num].pkt.pkt_proto = 6;
                } else if (0 == strcmp("UDP",pkt_proto)) {
                    g_xml_rule_info[g_rule_info_num].pkt.pkt_proto = 17;
                } else {
                    g_xml_rule_info[g_rule_info_num].pkt.pkt_proto = 0;
                }
            }
            if (NULL != pkt_type) {
                g_xml_rule_info[g_rule_info_num].pkt.pkt_type = atoi(pkt_type);
            }
            if (NULL != pkt_dir) {
                if (0 == strcmp("UP", pkt_dir)) {
                    g_xml_rule_info[g_rule_info_num].pkt.pkt_dir = 1;
                } else if (0 == strcmp("DOWN",pkt_dir)) {
                    g_xml_rule_info[g_rule_info_num].pkt.pkt_dir = 2;
                } else {
                    g_xml_rule_info[g_rule_info_num].pkt.pkt_dir = 0;
                }
            }
            if (NULL != pkt_seq) {
                g_xml_rule_info[g_rule_info_num].pkt.pkt_seq = htonl(atoi(pkt_seq));
            }
            
            if (NULL != pkt_proto) {
                xmlFree((xmlChar*)pkt_proto);
            } 
            if (NULL != pkt_type) {
                xmlFree((xmlChar*)pkt_type);
            } 
            if (NULL != pkt_dir) {
                xmlFree((xmlChar*)pkt_dir);
            } 
            if (NULL != pkt_seq) {
                xmlFree((xmlChar*)pkt_seq);
            } 
        }
        if(!xmlStrcmp(cur->name, (const xmlChar *)"port")) { 
            port = (char*)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            port_type = (char*)xmlGetProp(cur, (const xmlChar *)"type");           
            port_sdtype = (char*)xmlGetProp(cur, (const xmlChar *)"sdtype");
            memset(hex_port_black, 0x00, sizeof(hex_port_black));
            if (NULL != port) {
                if(-1 == portstr_process(port, hex_port_black, &port_num)) {
                    return -1;    
                }
            }
            while(0 != hex_port_black[info_port_num]) {
                if (NULL != port_type) {
                    if (0 == strcmp("TCP", port_type))
                    {
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].l4_proto = 6;
                    } else if (0 == strcmp("UDP",port_type)) {
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].l4_proto = 17;
                    } else {
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].l4_proto = 0;
                    }
                } else {
                    g_xml_rule_info[g_rule_info_num].port[info_port_num].l4_proto = 0;
                }
                if (NULL != port_sdtype) 
                {
                    if (0 == strcmp("dport",port_sdtype)) {
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].sdtype = 2;                       
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].sdport.sdport.dport= htons(hex_port_black[info_port_num]);  
                    } else if (0 == strcmp("sport",port_sdtype)) {
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].sdtype = 1;                        
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].sdport.sdport.sport = htons(hex_port_black[info_port_num]); 
                    } else if (0 == strcmp("sdport",port_sdtype)) {
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].sdtype = 3;                     
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].sdport.sdport.sport = htons(hex_port_black[0]);             
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].sdport.sdport.dport= htons(hex_port_black[1]); 
                        g_xml_rule_info[g_rule_info_num].port_num = info_port_num + 1;
                        break;
                    } else {
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].sdtype = 0;                      
                        g_xml_rule_info[g_rule_info_num].port[info_port_num].sdport.port = htons(hex_port_black[info_port_num]);  
                    }
                } else {                    
                    g_xml_rule_info[g_rule_info_num].port[info_port_num].sdport.port = htons(hex_port_black[info_port_num]);  
                    g_xml_rule_info[g_rule_info_num].port[info_port_num].sdtype = 0;
                }
                g_xml_rule_info[g_rule_info_num].port_num = info_port_num + 1;
                info_port_num++;
            }
            //info_port_num = 0;
            port_count = 0;
            port_count_s = 0;
            if (NULL != port_type) {
                xmlFree((xmlChar*)port_type);
            } 
            if (NULL != port_sdtype) {
                xmlFree((xmlChar*)port_sdtype);
            } 
            if (NULL != port) {                
                port = NULL;
                //xmlFree((xmlChar*)port);
            }
        }
        if(!xmlStrcmp(cur->name, (const xmlChar *)"ip")) { 
            ip = (char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            ip_version = (char*)xmlGetProp(cur, (const xmlChar *)"ver");           
            ip_sdtype= (char*)xmlGetProp(cur, (const xmlChar *)"sdtype"); 
            if (NULL != ip) {
                if (NULL != ip_version && 0 == strcmp("v6", ip_version)) {
                    memset(hex_ip_block, 0x00 , sizeof(hex_ip_block));
                    memset(hex_ipv6_block, 0x00 , sizeof(hex_ipv6_block));                    
                    if (-1 == ipv6str_process(ip, hex_ipv6_block, &ip_num)) {
                        return -1;
                    }
                } else {
                    memset(hex_ip_block, 0x00 , sizeof(hex_ip_block));
                    memset(hex_ipv6_block, 0x00 , sizeof(hex_ipv6_block));
                    if (-1 == ipv4str_process(ip, hex_ip_block, &ip_num)) {
                        return -1;
                    }
                }
            }
            while(0 != hex_ip_block[info_ip_num] || 0 != hex_ipv6_block[info_ip_num][7]) {
                if (NULL != ip_version) {
                    if (0 == strcmp("v4", ip_version)) {
                        g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].ip_ver = 4;                      
                        if (NULL != ip_sdtype) {
                            if (0 == strcmp("dip",ip_sdtype)) {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 2;
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.dip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                            } else if (0 == strcmp("sip",ip_sdtype)) {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 1;
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.sip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                            } else if (0 == strcmp("sdip",ip_sdtype)) {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 3;
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.sip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.dip.ipv4 = htonl(hex_ip_block[info_ip_num + 1]);                                
                                //g_xml_rule_info[g_rule_info_num].ip_num = info_ip_num + 1;                               
                                g_xml_rule_info[g_rule_info_num].ip_num = g_xml_rule_info[g_rule_info_num].ip_num + 1;
                                break;
                            } else {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 0;
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.ip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                            }
                        } else {
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 0;
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.ip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                        } 
                    } else if (0 == strcmp("v6",ip_version)) {
                        g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].ip_ver = 6;
                        hex_ipv6_block[info_ip_num][0] = htons(hex_ipv6_block[info_ip_num][0]);
                        hex_ipv6_block[info_ip_num][1] = htons(hex_ipv6_block[info_ip_num][1]);
                        hex_ipv6_block[info_ip_num][2] = htons(hex_ipv6_block[info_ip_num][2]);
                        hex_ipv6_block[info_ip_num][3] = htons(hex_ipv6_block[info_ip_num][3]);
                        hex_ipv6_block[info_ip_num][4] = htons(hex_ipv6_block[info_ip_num][4]);
                        hex_ipv6_block[info_ip_num][5] = htons(hex_ipv6_block[info_ip_num][5]);
                        hex_ipv6_block[info_ip_num][6] = htons(hex_ipv6_block[info_ip_num][6]);
                        hex_ipv6_block[info_ip_num][7] = htons(hex_ipv6_block[info_ip_num][7]);
                        if (NULL != ip_sdtype) {
                            if (0 == strcmp("dip",ip_sdtype)) {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 2;
                                memcpy(g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.dip.ipv6, hex_ipv6_block[info_ip_num], 16);
                            } else if (0 == strcmp("sip",ip_sdtype)) {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 1;
                                memcpy(g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.sip.ipv6, hex_ipv6_block[info_ip_num], 16);
                            } else {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 0;
                                memcpy(g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.ip.ipv6, hex_ipv6_block[info_ip_num], 16);
                            }
                        } else {
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 0;
                            memcpy(g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.ip.ipv6, hex_ipv6_block[info_ip_num], 16);
                        }     
                    } else {
                        g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].ip_ver = 4;
                        if (NULL != ip_sdtype) {
                            if (0 == strcmp("dip",ip_sdtype)) {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 2;
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.dip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                            } else if (0 == strcmp("sip",ip_sdtype)) {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 1;
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.sip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                            } else if (0 == strcmp("sdip",ip_sdtype)) {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 3;
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.sip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.dip.ipv4 = htonl(hex_ip_block[info_ip_num+1]);                              
                                //g_xml_rule_info[g_rule_info_num].ip_num = info_ip_num + 1;                                
                                g_xml_rule_info[g_rule_info_num].ip_num = g_xml_rule_info[g_rule_info_num].ip_num + 1;
                            } else {
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 0;
                                g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.ip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                            }
                        } else {
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 0;
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.ip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                        }                          
                    }
                } else {
                    g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].ip_ver = 4;
                    if (NULL != ip_sdtype) {
                        if (0 == strcmp("dip",ip_sdtype)) {
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 2;
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.dip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                        } else if (0 == strcmp("sip",ip_sdtype)) {
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 1;
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.sip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                        } else if (0 == strcmp("sdip",ip_sdtype)) {
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 3;
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.sip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.sdip.dip.ipv4 = htonl(hex_ip_block[info_ip_num+1]);                               
                            //g_xml_rule_info[g_rule_info_num].ip_num = info_ip_num + 1;                         
                            g_xml_rule_info[g_rule_info_num].ip_num = g_xml_rule_info[g_rule_info_num].ip_num + 1;
                            break;
                        } else {
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 0;
                            g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.ip.ipv4= htonl(hex_ip_block[info_ip_num]);
                        }
                    } else {
                        g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdtype = 0;
                        g_xml_rule_info[g_rule_info_num].ip[g_xml_rule_info[g_rule_info_num].ip_num].sdip.ip.ipv4 = htonl(hex_ip_block[info_ip_num]);
                    }  
                }                
                //printf("g_xml_rule_info[g_rule_info_num].ip_num: %d, g_xml_rule_info[g_rule_info_num].ip:%x\n", g_xml_rule_info[g_rule_info_num].ip_num, g_xml_rule_info[g_rule_info_num].ip[info_ip_num].sdip.ip.ipv4);
                //g_xml_rule_info[g_rule_info_num].ip_num = info_ip_num + 1;
                g_xml_rule_info[g_rule_info_num].ip_num = g_xml_rule_info[g_rule_info_num].ip_num + 1;
                info_ip_num++;
            }
            if (NULL != ip)
            {
                xmlFree((xmlChar*)ip);                
            }
            if (NULL != ip_sdtype)
            {               
                xmlFree((xmlChar*)ip_sdtype);                
            }
            if (NULL != ip_version)
            {               
                xmlFree((xmlChar*)ip_version);                
            }
            if (NULL != src_ip)
            {
                src_ip = NULL;
            }
            if (NULL != dst_ip)
            {
                dst_ip = NULL;
            }
            memset(hex_ip_block, 0x00 , sizeof(hex_ip_block));          
            memset(hex_ipv6_block, 0x00 , sizeof(hex_ipv6_block));
            info_ip_num = 0;
        }        
        cur = cur->next;
    }   
    return 0;
}

int parseRule(FILE *fp_exp, xmlDocPtr doc, xmlNodePtr cur, unsigned int rule_id, char *rule_name, unsigned short key_index_num)
{  
    /*xmlChar* info;*/     
    cur = cur->xmlChildrenNode;  
    int ret = 0;
    while(cur != NULL){ 
         g_xml_rule_info[g_rule_info_num].rule_id = htonl(rule_id);
         strcpy((char *)g_xml_rule_info[g_rule_info_num].rule_name, rule_name);  
        if(!xmlStrcmp(cur->name, (const xmlChar *)"info")) {
            ret = parseInfo(fp_exp, doc, cur, key_index_num);
            if (-1 == ret) {
                printf("parseInfo error\n");
                return ret;
            }  
            g_rule_info_num++;
        }
        cur = cur->next;
    }
    return ret;  
}  

static int parseDoc(FILE *fp_exp, char *docname, unsigned short key_index_num, unsigned short rule_index_num)
{  
    xmlDocPtr doc;  
    xmlNodePtr cur; 
    int ret = 0;
    char rule_name[32] = {0};
    char *rule_name_ptr = NULL;
    unsigned int rule_id = 0;
    xmlChar *rule_id_ptr = NULL;
    doc = xmlParseFile(docname);      
    ///printf("loadxml file: %s\n", docname); 
    if(doc == NULL){  
        fprintf(stderr, "Document not parse successfully. \n");  
        return -1;  
    }  

    cur = xmlDocGetRootElement(doc);  
    if(cur == NULL){  
        fprintf(stderr, "empty document\n");  
        xmlFreeDoc(doc);  
        return -1;
    }  

    if(xmlStrcmp(cur->name, (const xmlChar *)"aclk")){  
        fprintf(stderr, "document of the wrong type, root node != story");  
        xmlFreeDoc(doc);  
        return -1;  
    }  

    cur = cur->xmlChildrenNode;  
    while(cur != NULL){  
        if(!xmlStrcmp(cur->name, (const xmlChar *)"id")){
            rule_id_ptr = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            /*rule_id = *(unsigned int*)(char*)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);*/  
            //printf("id: %s\n", rule_id_ptr);
            rule_id = (unsigned int)atoi((char *)rule_id_ptr);
            rule_name_index[rule_index_num].rule_id = rule_id;
            xmlFree((xmlChar*)rule_id_ptr);
        }
        if(!xmlStrcmp(cur->name, (const xmlChar *)"name")){
            rule_name_ptr = (char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            strcpy((char *)rule_name, rule_name_ptr);
            strcpy((char *)rule_name_index[rule_index_num].rule_name, (char *)rule_name);
            xmlFree((xmlChar*)rule_name_ptr);
        } 
        if(!xmlStrcmp(cur->name, (const xmlChar *)"rule")){  
            ret = parseRule(fp_exp, doc, cur, rule_id, rule_name, key_index_num); 
            if (-1 == ret) {
                return ret;   
            }
        }
        cur = cur->next;
    } 
    xmlFreeDoc(doc); 

    return ret;  
}  

int load_rule_init(void)
{
    int i;

    g_xml_key_rule = (char **)malloc(sizeof(char *) * XML_RULE_MAX_EXP_NUM);
    if (NULL == g_xml_key_rule) {
        printf("%s[%d]:malloc for rule exp error\n", __func__, __LINE__);
        return -1;
    }
    memset(g_xml_key_rule, 0x00, sizeof(char *) * XML_RULE_MAX_EXP_NUM);
    for (i = 0; i < XML_RULE_MAX_EXP_NUM; i++) {
        g_xml_key_rule[i] = (char *)malloc(XML_RULE_MAX_KEY_LEN);
        if (NULL == g_xml_key_rule[i]) {
            printf("%s[%d]:malloc for rule exp error\n", __func__, __LINE__);
            goto rule_init_error;
        }
        memset(g_xml_key_rule[i], 0x00, XML_RULE_MAX_KEY_LEN);
    }

    g_xml_rule_info = (rule_t *)malloc(sizeof(rule_t) * RULE_NUM_MAX);
    if (NULL == g_xml_rule_info) {
        printf("%s[%d]:malloc for rule info error\n", __func__, __LINE__);
        goto rule_init_error;
    }
    memset(g_xml_rule_info, 0x00, sizeof(rule_t) * RULE_NUM_MAX);
    g_rule_info_num = 0;
    g_key_rule_num = 0;

    return 0;
rule_init_error:
    if (g_xml_rule_info) {
        free(g_xml_rule_info);
        g_xml_rule_info = NULL;
    }
    if (g_xml_key_rule) {
        for (i = 0; i < XML_RULE_MAX_EXP_NUM; i++) {
            if (g_xml_key_rule[i]) {
                free(g_xml_key_rule[i]);
                g_xml_key_rule[i] = NULL;
            }
        }
        free(g_xml_key_rule);
        g_xml_key_rule = NULL;
    }

    return -1;
}

void load_rule_fini(void)
{
    int i;

    if (g_xml_rule_info) {
        free(g_xml_rule_info);
        g_xml_rule_info = NULL;
    }
    if (g_xml_key_rule) {
        for (i = 0; i < XML_RULE_MAX_EXP_NUM; i++) {
            if (g_xml_key_rule[i]) {
                free(g_xml_key_rule[i]);
                g_xml_key_rule[i] = NULL;
            }
        }
        free(g_xml_key_rule);
        g_xml_key_rule = NULL;
    }

    return;
}

int rule_name_str_transfer(char *rule_name, unsigned int rule_id)
{
    unsigned int i = 0 ;
    for (i = 0; i < name_index_num; i++) {
        if (rule_name_index[i].rule_id == rule_id) {
            strcpy(rule_name, (char*)rule_name_index[i].rule_name);
            break;
        }
    }
    return 0;
}

int load_rule(int sock, int octeon_id)
{
    char docname[30000][128];
    char docname_tmp[128] = {0};
    char head_str[32] = {0};
    DIR *dir; 
    struct dirent *ptr; 
    unsigned short key_index_num = 0; 
    unsigned short xml_name_num = 0, i = 0;
    FILE *fp_exp;
    FILE *fp_head;
    char md5_exp[33], md5_tmp[33];
    FILE *fp;


    if ((fp_head = fopen (XML_RULE_DEFAULT_XML_PATH XML_RULE_SPECIAL_KEY, "r")) == NULL) {
        printf ("File head open error!\r\n");
        return -1;
    }
    if (NULL == (fp_exp = fopen(XML_RULE_TEMP_PATH XML_RULE_EXP_FILE,"w+"))) {
        printf("open exp file error\r\n");
        return -1;
    }
    while ((fgets(head_str, 32, fp_head)) != NULL) {
        fprintf(fp_exp, head_str); 
        memset(head_str, 0, sizeof(head_str));
        key_index_num++ ;
    }
    fclose(fp_head);

    if (load_rule_init()) {
        return -1;
    }

    if ((dir=opendir(XML_RULE_DEFAULT_XML_PATH)) == NULL) { 
        printf("Open dir error\r\n"); 
        load_rule_fini();
        fclose(fp_exp);

        return -1;
    } 
    // readdir() return next enter point of directory dir
    while ((ptr=readdir(dir)) != NULL) { 
        if((0 == strcmp(ptr->d_name,".")) || (0 == strcmp(ptr->d_name,"..")) || (0 == strcmp(ptr->d_name, XML_RULE_SPECIAL_KEY)) || (0 == strcmp(ptr->d_name, XML_RULE_APPNAME_ID))) { //current dir OR parrent dir
            continue;
        }

        memset(docname[xml_name_num], 0, sizeof(docname[xml_name_num])); 
        sprintf(docname[xml_name_num], XML_RULE_DEFAULT_XML_PATH "%s", ptr->d_name);
        for (i = 1; i < xml_name_num + 1; i++) {
            if (strcmp(docname[xml_name_num - i + 1], docname[xml_name_num - i]) < 0 ) {
            //sw  
                strcpy(docname_tmp, docname[xml_name_num - i]);
                strcpy(docname[xml_name_num - i], docname[xml_name_num - i + 1]);
                strcpy(docname[xml_name_num - i + 1], docname_tmp);
            }
        }
        xml_name_num++;
    }
    name_index_num = xml_name_num - 1;
    memset(rule_name_index, 0, sizeof(rule_name_index)); 
    for (i = 0 ; i < xml_name_num; i++) { 
        if (-1 == parseDoc(fp_exp, docname[i], key_index_num, i)) {
            closedir(dir);
            load_rule_fini();
            fclose(fp_exp);
            return -1;
        }    
    }
    /*
       for (i = 0 ; i < g_rule_info_num ; i++) {      
       printf("rule_name[%d]: %s,rule_id:%d\n", i,g_xml_rule_info[i].rule_name, g_xml_rule_info[i].rule_id);
       }
    */
    closedir(dir);
    fclose(fp_exp);

    memset(md5_exp, 0x00, 33);
    memset(md5_tmp, 0x00, 33);
    fp = popen("md5sum "XML_RULE_DEFAULT_PATH XML_RULE_EXP_FILE, "r");
    if (NULL == fp) {
        printf("popen command md5sum"XML_RULE_DEFAULT_PATH XML_RULE_EXP_FILE"error\r\n");
        load_rule_fini();
        return -1;
    }
    fread(md5_exp, 33, 1, fp);
    md5_exp[32] = '\0';
    pclose(fp);
    fp = popen("md5sum "XML_RULE_TEMP_PATH XML_RULE_EXP_FILE, "r");
    if (NULL == fp) {
        printf("popen command md5sum"XML_RULE_TEMP_PATH XML_RULE_EXP_FILE"error\r\n");
        load_rule_fini();
        return -1;
    }
    fread(md5_tmp, 33, 1, fp);
    md5_tmp[32] = '\0';
    pclose(fp);
    
    if (0 != strcmp(md5_exp, md5_tmp)) {
        printf("rule xml and graph not match\n");
        load_rule_fini();
        return -1;
    }

    if (0 != g_key_rule_num) {
        ///system("./bin/hfac --input=bin/exp");
        if (aclk_load_rule_init(sock, octeon_id, key_index_num)) {
            printf("rule graph init error\r\n");
            load_rule_fini();
            return -1;
        }
    } else {
        if (aclk_load_rule_init(sock, octeon_id, 0)) {
            printf("rule graph init error\r\n");
            load_rule_fini();
            return -1;
        }
    }    

    if (aclk_load_rule_info(sock, octeon_id, g_rule_info_num)) {
        printf("rule graph upload process error\r\n");
        load_rule_fini();
        return -1;
    }

    if (0 != g_key_rule_num) {
        if (aclk_load_rule_graph(sock, octeon_id)) {
            printf("hfa add graph process error\r\n");
            load_rule_fini();
            return -1;
        }
    } 

    if (aclk_load_config_info(sock, octeon_id)) {
        printf("rule config error\r\n");
        load_rule_fini();
        return -1;
    }
    
    load_rule_fini();

    return 0;
}
