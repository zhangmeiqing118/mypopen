/*
 * @Filename: comm.h
 * @Brief   :
 * @Author  : zl 
 * @Version : 1.0.0
 * @Date    : 08/08/2016 03:51:22 PM
 */
#ifndef __Loadxml_H__
#define __Loadxml_H__


#include <libxml/parser.h>
#include <ctype.h>
#include <dirent.h> 
#include <arpa/inet.h>

#ifdef  __cpulsplus
extern "C" {
#endif

#define XML_RULE_DEFAULT_PATH       "/etc/aclk/rule/"
#define XML_RULE_DEFAULT_XML_PATH   XML_RULE_DEFAULT_PATH"xml/"
#define XML_RULE_SPECIAL_KEY        "special_key"
#define XML_RULE_APPNAME_ID         "appname.dat"
#define XML_RULE_GRAPH_FILE         "hfa.out"
#define XML_RULE_EXP_FILE           "exp"
#define XML_RULE_TEMP_PATH          "/tmp/"
#define	SYS_FLASH_PATH			        "/mnt/disk3/flash/"
#define	START_NP0_CONF_FILE_NAME_TMP	"start-np0-config.conf.tmp"
#define	START_NP0_CONF_FILE_NAME	    "start-np0-config.conf"
#define START_CONFIG_FILE_DEL_NP0      	"rm -f /mnt/disk3/flash/start-np0-config.conf"
#define START_CONFIG_FILE_BACK_NP0      "cp /mnt/disk3/flash/start-np0-config.conf.tmp /mnt/disk3/flash/start-np0-config.conf"
#define	START_NP1_CONF_FILE_NAME_TMP	"start-np1-config.conf.tmp"
#define	START_NP1_CONF_FILE_NAME	    "start-np1-config.conf"
#define START_CONFIG_FILE_DEL_NP1      	"rm -f /mnt/disk3/flash/start-np1-config.conf"
#define START_CONFIG_FILE_BACK_NP1      "cp /mnt/disk3/flash/start-np1-config.conf.tmp /mnt/disk3/flash/start-np1-config.conf"

#define XML_RULE_MAX_EXP_NUM    0x10000
#define XML_RULE_MAX_KEY_LEN    256

#define XML_FILE_MAX_LEN 2048
#define XML_MAX_PORT_NUM 200
#define XML_MAX_IP_NUM 50

int load_rule(int sock, int octeon_id);
int rule_name_str_transfer(char *rule_name, unsigned int rule_id);

#ifdef  __cpulsplus
}
#endif
#endif
