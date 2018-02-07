/*
    test.c -- Unit test program for GoAhead

    Usage: goahead-test [options] [documents] [endpoints...]
        Options:
        --auth authFile        # User and role configuration
        --home directory       # Change to directory to run
        --log logFile:level    # Log to file file at verbosity level
        --route routeFile      # Route configuration file
        --verbose              # Same as --log stderr:2
        --version              # Output version information

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/sysinfo.h>


#include "js.h"
#include "me.h"
#include "goahead.h"

#include "web.h"

#define HOME_DIR_EN "/opt/www/www_ocml_en"
#define HOME_DIR_CH "/opt/www/www_ocml_ch"

#if 0
#include <sys/ioctl.h>
#include <signal.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <pthread.h>

#include "sys_cfg.h"
#include "acl_cfg.h"
#include "if_cfg.h"
#include "vos.h"
#include "vos_libc.h"
#include "hdl_api.h"
#include "acl_api.h"
#include "sw_if_api.h"
#include "cli_acl_cfg.h"
#include "cli_sys_cfg.h"
#include "cli_command.h"
#include "log_pub.h"
#include "sw_dev.h"
#include "feature.h"
#include "debug.h"
#include "goahead.h"
#include "lbApi.h"
#include "web_main.h"

#include "js.h"
#endif

#include "cJSON.h"
#if 0
#include "ospGlobalConfig.h"
#include "ospEthProcess.h"
#include "ospOlpProcess.h"
#include "ospEdfaProcess.h"
#include "ospGenApi.h"
#endif
/********************************* Defines ************************************/

#ifndef LANG_ENGLISH
#define LANG_ENGLISH                1
#define LANG_CHINESE                0
#endif
#define LANG_DEFAULT                LANG_ENGLISH

#ifndef USER_STR_LEN_MAX
#define USER_STR_LEN_MAX            168
#endif

static int finished = 0;
int g_lang_en;

extern char websClientIp[64];
#if 0
extern user_info_t g_user_db[SYS_USER_MAX];
static VOS_THREAD_t g_pWebThreadId = 0;
extern hdl_tm_t sysStartupTime;
extern uchar acllist[MAX_SUPPORT_FILTER_NUM+1];
char netgateway[18]={"192.168.1.1"};
char netgateway1[18]={"172.16.1.1"};
char netgateway2[18]={"192.168.1.1"};
extern struct host host;
extern ulong server_ip;
extern uint g_ntp_mode;
extern sys_poll_info_t g_sys_poll_info;

int ntpsetok=0;
#define  WEB_STR_OK                  "ok"
#define  WEB_STR_ERROR               "fail"
#define  WEB_STR_FAIL                "fail"
#define	 LOG_DIR_PATH	             "/mnt/flash/log"
#define  LOG_DOWNLOAD_GZ_FILE_NAME   "log.tar.gz"
#define  LOG_DOWNLOAD_GZ_FILE_PATH   "/mnt/application/www/www_ch/log.tar.gz"
#define  LOG_FILE_MAX_NUM            20

#define  LOG_NAME_MAX_LEN       20

static void initPlatform();
static void logHeader();
static void usage();
static bool testHandler(Webs *wp);
#if ME_GOAHEAD_JAVASCRIPT
static int aspTest(int eid, Webs *wp, int argc, char **argv);
static int bigTest(int eid, Webs *wp, int argc, char **argv);
#endif
static void actionTest(Webs *wp, char *path, char *query);
static void sessionTest(Webs *wp, char *path, char *query);
static void showTest(Webs *wp, char *path, char *query);
#if ME_GOAHEAD_UPLOAD
static void uploadTest(Webs *wp, char *path, char *query);
#endif
#if ME_GOAHEAD_LEGACY
static int legacyTest(Webs *wp, char *prefix, char *dir, int flags);
#endif
#if ME_UNIX_LIKE
static void sigHandler(int signo);
#endif

extern int is_ocml_module(void);

static void loginServiceProc(Webs *wp);
static void logoutServiceProc(Webs *wp);

static bool websLoginUser1(Webs *wp, char *username, char *password);

static char *getVar(Webs *wp,char *name)
{
	char *special,*content;
	//printf("info:%s\r\n",wp->input.buf);

	if(strlen(wp->input.buf)<strlen(name)+2)
	{
		return NULL;
	}
	special = strchr(wp->input.buf,61);/*61  '='*/
	if(NULL == special)
	{
		return NULL;
	}
	if(0 == sncmp(wp->input.buf,name,strlen(name)))
	{
		content = special+1;
//		printf("content:%s,%d\r\n",content,content);
		return content;
	}
//	printf("buf:%s,name:%s\r\n",wp->input.buf,name);
	return NULL;
}

unsigned char  isAllDigit (char str[])
{
    int len, i;

    if(str == NULL || *str == 0)
    {
        return 0;
    }

    len = strlen(str);

    for (i = 0; (i < len) ; i ++)
    {
        if ( (str[i] < '0') || (str[i] > '9') )
        {
	     return 0;
        }
    }

    return 1;
}

int str2int(char str[])
{
    int len, i, sum;

    if(str == NULL || *str == 0)
    {
        return 0;
    }

    if( !isAllDigit(str))
   {
       return -1;
   }

    len = strlen(str);
    if(len < 1)
        return 0;

    sum = 0;

    for (i = 0; i < len; i ++)
    {
        sum=sum*10+(str[i]-'0');
    }

    return sum;
}

static bool web_param_valid_check(char * input)
{
	char *p=NULL;
	p=strchr(input,'%');
	if(p!=NULL)
	{
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG," p:%s\r\n",p);
		return FALSE;
	}
	p=strchr(input,'+');
	if(p!=NULL)
	{
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG," p:%s\r\n",p);
		return FALSE;
	}
	p=strstr(input,"and");
	if(p!=NULL)
	{
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG," p:%s\r\n",p);
		return FALSE;
	}
	p=strstr(input,"or");
	if(p!=NULL)
	{
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG," p:%s\r\n",p);
		return FALSE;
	}

	return TRUE;
}

static bool web_param_valid_check_no_or(char * input)
{
	char *p=NULL;
	p=strchr(input,'%');
	if(p!=NULL)
	{
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG," p:%s\r\n",p);
		return FALSE;
	}
	p=strchr(input,'+');
	if(p!=NULL)
	{
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG," p:%s\r\n",p);
		return FALSE;
	}
	p=strstr(input,"and");
	if(p!=NULL)
	{
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG," p:%s\r\n",p);
		return FALSE;
	}

	return TRUE;
}

void llgtostr(unsigned long long pkt,char *pdata,int *len)
{
	char *data=NULL;
	char buff[32]={0};
	int i=0;

	data=vosAlloc(500);
	if(data==NULL)
	{
		return;
	}
	memset(data,0,500);
	sprintf(buff,"%lld",pkt);
	strcpy(data,buff);
	i=strlen(buff);

	sprintf(&data[i++],"-");
	*len=i;
	memcpy(pdata,data,*len);
	free(data);
	data=NULL;
}

static void web_getAllFlowInfos(Webs *wp, char *path, char *query)
{
	char *data=NULL, bdata[40]={0};
	int i=0,card=1,port=0,len=0;
	st_port_statistics port_stats;
    int ret = OK;
    uint phy_port;
	int unit = 0;
    uint separated;
	unsigned long long rxpkt,txpkt,rxbyte,txbyte,rxrate,txrate,rxerror,txerror;

	data=vosAlloc(50000);
	if(data==NULL)
	{
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	memset(data,0,50000);
	memset(&port_stats,0,sizeof(st_port_statistics));

	for(port=1;port<=PORT_TOTOL_MAX_NUM;port++)//PORT_TOTOL_MAX_NUM
	{

        if (check_port_is_invalid_state(port))
        {
            memset(&port_stats,0,sizeof(st_port_statistics));
        }
        else
        {
           //logic_to_Physic(port, &phy_port, &unit);
            //printf("wwwww phy port:%d!!\r\n", phy_port);
           adp_port_statistics_get(port, &port_stats);
        }

		txpkt=port_stats.tx.Pkts;
		llgtostr(txpkt,&bdata,&len);
		memcpy(&data[i],bdata,len);
		i+=len;

		rxpkt=port_stats.rx.Pkts;
		llgtostr(rxpkt,&bdata,&len);
		memcpy(&data[i],bdata,len);
		i+=len;

		txbyte=port_stats.tx.Bytes;
		llgtostr(txbyte,&bdata,&len);
		memcpy(&data[i],bdata,len);
		i+=len;

		rxbyte=port_stats.rx.Bytes;
		llgtostr(rxbyte,&bdata,&len);
		memcpy(&data[i],bdata,len);
		i+=len;

		txerror=port_stats.tx.Errors;
		llgtostr(txerror,&bdata,&len);
		memcpy(&data[i],bdata,len);
		i+=len;

		rxerror=port_stats.rx.Errors;
		llgtostr(rxerror,&bdata,&len);
		memcpy(&data[i],bdata,len);
		i+=len;

		rxerror=port_stats.rx.Discards;
		llgtostr(rxerror,&bdata,&len);
		memcpy(&data[i],bdata,len);
		i+=len;

		txerror=port_stats.tx.Discards;
		llgtostr(txerror,&bdata,&len);
		memcpy(&data[i],bdata,len);
		i+=len;

		txrate=port_stats.tx.bps;
		llgtostr(txrate,&bdata,&len);
		memcpy(&data[i],bdata,len);
		i+=len;

		rxrate=port_stats.rx.bps;
		llgtostr(rxrate,&bdata,&len);
		memcpy(&data[i],bdata,len);
		i+=len-1;/*端口下最后一个数据的"-"  要被";" 替代*/
		sprintf(&data[i++],";");
	}

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "data: %s \r\n",data);

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;
}

static void web_getFlowRate(Webs *wp, char *path, char *query)
{
	char *data=NULL, bdata[40]={0};
	int i=0,card=1,port=0,len=0;
	st_port_statistics port_stats;
	unsigned long long rxrate,txrate;
	char	*key,*value;

	key = stok(wp->input.buf, "=", &value);
	if(key==NULL || strlen(key)<1 || value==NULL || strlen(key)<1)
	{
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

	if (strcmp(key, "port")==0)
	{
		port = atoi(value);

	}

	data=vosAlloc(500);
	if(data==NULL)
	{
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,"error");
		websDone(wp);
		return;
	}
	memset(data,0,500);
	memset(&port_stats,0,sizeof(st_port_statistics));

	adp_port_statistics_get(port, &port_stats);

	txrate=port_stats.tx.bps;
	llgtostr(txrate,&bdata,&len);
	memcpy(&data[i],bdata,len);
	i+=len;

	rxrate=port_stats.rx.bps;
	llgtostr(rxrate,&bdata,&len);
	memcpy(&data[i],bdata,len);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "data: %s \r\n",data);

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;
}

#if 0
static void web_getPortStatus(Webs *wp, char *path, char *query)
{
	int ret,i=0;
	unsigned int port,mngStatus,speed,separa;
	unsigned char negotiation,duplex,linkStatus;
	t_Sw_EthIf if_info;
	char buff[50]={0};
	char *data=NULL;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	data=vosAlloc(1000);
	if(data==NULL)
	{
		printf("memery malloc error!!\r\n");
		websWrite(wp,"error");
		websDone(wp);
		return;
	}
	memset(data,0,1000);
	for(port=1;port<=PORT_TOTOL_MAX_NUM;port++)
	{

		ret=get_port_info(1,1,port,&if_info);
		if(ret!=OK)
		{
			mngStatus=4;
			speed=0;
			separa=0;
		}
		else
		{
			DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "test: %d %d %d %d %d\r\n",if_info.mngStatus,if_info.linkStatus,if_info.negotiation,if_info.duplex,if_info.speed);
			negotiation=if_info.negotiation;
			duplex=if_info.duplex;

			mngStatus=(if_info.mngStatus!=PL_ETHPORT_STATE_UP) ? 0 : ((if_info.linkStatus==PL_ETHPORT_STATE_UP) ?  2 : 1);
			speed=(negotiation==PL_ETHPORT_AUTO_NEG_ENABLE) ? 0 : ((duplex==PL_ETHPORT_DUPLEX_HALF) ? 1 :( (if_info.speed==PL_ETHPORT_SPEED_10G) ? 3 : 2));
			separa=if_info.beenSeparated;
		}

		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "port:%d admin: %d speed: %d separa: %d\r\n",port,mngStatus,speed,separa);

		if(port/10==0)
		{
			vosIntToStr(buff, port, 10, 0, 0);
			sprintf(&data[i++],"%s",buff);
		}
		else
		{
			vosIntToStr(buff, port/10, 10, 0, 0);
			sprintf(&data[i++],"%s",buff);
			vosIntToStr(buff, port%10, 10, 0, 0);
			sprintf(&data[i++],"%s",buff);
		}

		sprintf(&data[i++],",");
		vosIntToStr(buff, mngStatus, 10, 0, 0);
		sprintf(&data[i++],"%s",buff);
		sprintf(&data[i++],"*");
		vosIntToStr(buff, speed, 10, 0, 0);
		sprintf(&data[i++],"%s",buff);
		sprintf(&data[i++],"*");
		vosIntToStr(buff, separa, 10, 0, 0);
		sprintf(&data[i++],"%s",buff);
		sprintf(&data[i++]," ");
	}
	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "data=%s \n",data);
	websWrite(wp,data);
	websDone(wp);
	free(data);
	data=NULL;
}
#else
//extern hdl_optical_data g_optical_info;
//extern sys_poll_info_t g_sys_poll_info;
static void web_getPortStatus(Webs *wp, char *path, char *query)
{
	int ret,i=0;
	unsigned int port=0,sub_port=0,logic_num=0,mngStatus=0,speed=0;
    unsigned char negotiation,duplex,linkStatus,loopbackMode;
	t_Sw_EthIf if_info;
	char buff[32]={0};
	char  speed_str[5][16]= {"1G", "10G", "40G", "100G","auto"};
	char  moudle_type[5][16]= {"SFP", "SFP+", "QSFP+", "QSFP28","unkown"};
	int   moudle_id = 0;
	char *data=NULL;
    int single_flag=0;
    cJSON *data_json = NULL;
    cJSON *port_json = NULL;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	data_json = cJSON_CreateArray();
	if(data_json == NULL)
	{
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

	for(port=1;port<=PORT_USER_MAX_NUM;port++)
	{
	//	for (sub_port = 0; sub_port <= PORT_SUB_PORT_MAX_NUM; sub_port++)
		{
			logic_num = port;//get_port_logic_num(1, 1, port, sub_port);
			if (check_port_is_invalid_state(logic_num))
			{
			    continue;
			}
			else
			{
			   // ret=get_port_info(logic_num, &if_info);
				ret=get_port_info(1,1,port,&if_info);
				if(ret!=OK)
				{
					mngStatus  = 4;
					linkStatus = 4;
					speed      = 0;
					loopbackMode = 0;
				}
				else
				{
					DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "test: %d %d %d %d %d\r\n",if_info.mngStatus,if_info.linkStatus,if_info.negotiation,if_info.duplex,if_info.speed);
					negotiation=if_info.negotiation;
					duplex=if_info.duplex;

		            // 0 端口禁止  1 端口使能，link down  2 端口使能link up
					//mngStatus=(if_info.mngStatus!=PL_ETHPORT_STATE_UP) ? 0 : ((if_info.linkStatus==PL_ETHPORT_STATE_UP) ?  2 : 1);
					mngStatus    = (if_info.mngStatus!=PL_ETHPORT_STATE_UP) ? 0 : 1;
			             linkStatus   = (if_info.linkStatus!=PL_ETHPORT_STATE_UP) ? 0 : 1;
					loopbackMode = (if_info.loopbackMode==NONE_LOOPBACK) ? 0 : 1;
					//speed,  0 自协商   2 10G  3  100G
					// 自协商  NA
					if (negotiation==PL_ETHPORT_AUTO_NEG_ENABLE)
					{
						speed = 4;
						moudle_id = 4;
					}
					else
					{
						if (if_info.speed==PL_ETHPORT_SPEED_10G)
						{
							speed = 1;
							moudle_id = 1;

						}
						else if (if_info.speed==PL_ETHPORT_SPEED_1000)
						{
							speed = 0;
							moudle_id = 0;
						}
						else if(if_info.speed==PL_ETHPORT_SPEED_40G)
						{
							speed = 2;
							moudle_id = 2;
						}
						else if(if_info.speed==PL_ETHPORT_SPEED_100G)
						{
							speed = 3;
							moudle_id = 3;
						}
						else
						{
							speed = 1;
							moudle_id = 1;
						}
					}
					//speed=(negotiation==PL_ETHPORT_AUTO_NEG_ENABLE) ? 0 : ((duplex==PL_ETHPORT_DUPLEX_HALF) ? 1 :( (if_info.speed==PL_ETHPORT_SPEED_10G) ? 3 : 2));
					//separa=if_info.beenSeparated;
				}
			}

			adp_port_linkdown_trans_get(port, &single_flag);
	        DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "port:%d admin: %d speed: %d loopbackmode: %d,single_flag:%d\r\n",
	            port,mngStatus,speed,loopbackMode,single_flag);

			//web_port_id_trans_str(buff, port,sub_port);
	        	cJSON_AddItemToArray(data_json, port_json=cJSON_CreateObject());
			cJSON_AddNumberToObject(port_json, "portid", port);          //端口号
			cJSON_AddNumberToObject(port_json, "subport", sub_port);          //端口号
			cJSON_AddNumberToObject(port_json, "status", linkStatus);    //端口状态
			cJSON_AddStringToObject(port_json, "speed",  speed_str[speed]);         //端口速率
			cJSON_AddBoolToObject(port_json, "enable", mngStatus);       //端口使能
			cJSON_AddBoolToObject(port_json, "siglsend", single_flag);          //单纤发送
			cJSON_AddBoolToObject(port_json, "circl", loopbackMode);     //端口环回
			cJSON_AddStringToObject(port_json, "moduletype", moudle_type[moudle_id]);     //光模块类型
			cJSON_AddNumberToObject(port_json, "rxpower", (float)g_sys_poll_info.sys_optical_info.sfpRxPower[port-1]/100.0);     //光功率
			cJSON_AddNumberToObject(port_json, "txpower", (float)g_sys_poll_info.sys_optical_info.sfpTxPower[port-1]/100.0);     //光功率
			DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"sfp %d TX POWER	: %.2f (Power dbm)", port,(float)g_sys_poll_info.sys_optical_info.sfpTxPower[port-1]/100.0);
			DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"sfp %d RX POWER	: %.2f (Power dbm)", port,(float)g_sys_poll_info.sys_optical_info.sfpRxPower[port-1]/100.0);
		}
	}

	data = cJSON_Print(data_json);
	if(data == NULL)
	{
		cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	cJSON_Delete(data_json);

	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "data=%s \n",data);
	websWrite(wp,data);
	websDone(wp);
	free(data);
	data=NULL;
}
#endif
#endif

/*********************************** Code *************************************/
static void logHeader()
{
    char    home[ME_GOAHEAD_LIMIT_STRING];

    getcwd(home, sizeof(home));
    logmsg(2, "Configuration for %s", ME_TITLE);
    logmsg(2, "---------------------------------------------");
    logmsg(2, "Version:            %s", ME_VERSION);
    logmsg(2, "BuildType:          %s", ME_DEBUG ? "Debug" : "Release");
    logmsg(2, "CPU:                %s", ME_CPU);
    logmsg(2, "OS:                 %s", ME_OS);
    logmsg(2, "Host:               %s", websGetServer());
    logmsg(2, "Directory:          %s", home);
    logmsg(2, "Documents:          %s", websGetDocuments());
    logmsg(2, "Configure:          %s", ME_CONFIG_CMD);
    logmsg(2, "---------------------------------------------");
}

#if ME_UNIX_LIKE
static void sigHandler(int signo)
{
    finished = 1;
}
#endif


void initPlatform()
{
#if ME_UNIX_LIKE
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);
    signal(SIGKILL, sigHandler);
    #ifdef SIGPIPE
        signal(SIGPIPE, SIG_IGN);
    #endif
#elif ME_WIN_LIKE
    _fmode=_O_BINARY;
#endif
}



/*
    Simple handler and route test
    Note: Accesses to "/" are normally remapped automatically to /index.html
 */
static bool testHandler(Webs *wp)
{
    if (smatch(wp->path, "/")) {
        websRewriteRequest(wp, "/home.html");
        /* Fall through */
    }
    return 0;
}


#if ME_GOAHEAD_JAVASCRIPT
/*
    Parse the form variables: name, address and echo back
 */
static int aspTest(int eid, Webs *wp, int argc, char **argv)
{
	char	*name, *address;

	if (jsArgs(argc, argv, "%s %s", &name, &address) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	return (int) websWrite(wp, "Name: %s, Address %s", name, address);
}


/*
    Generate a large response
 */
static int bigTest(int eid, Webs *wp, int argc, char **argv)
{
    int     i;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "<html>\n");
    for (i = 0; i < 800; i++) {
        websWrite(wp, " Line: %05d %s", i, "aaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbccccccccccccccccccddddddd<br/>\r\n");
    }
    websWrite(wp, "</html>\n");
    websDone(wp);
    return 0;
}
#endif


/*
    Implement /action/actionTest. Parse the form variables: name, address and echo back.
 */
static void actionTest(Webs *wp, char *path, char *query)
{
	char	*name, *address;

	name = websGetVar(wp, "name", NULL);
	address = websGetVar(wp, "address", NULL);
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
	websWrite(wp, "<html><body><h2>name: %s, address: %s</h2></body></html>\n", name, address);
	websDone(wp);
}


static void sessionTest(Webs *wp, char *path, char *query)
{
	char	*number;

    if (scaselessmatch(wp->method, "POST")) {
        number = websGetVar(wp, "number", 0);
        websSetSessionVar(wp, "number", number);
    } else {
        number = websGetSessionVar(wp, "number", 0);
    }
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "<html><body><p>Number %s</p></body></html>\n", number);
    websDone(wp);
}


static void showTest(Webs *wp, char *path, char *query)
{
    WebsKey     *s;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "<html><body><pre>\n");
    for (s = hashFirst(wp->vars); s; s = hashNext(wp->vars, s)) {
        websWrite(wp, "%s=%s\n", s->name.value.string, s->content.value.string);
    }
    websWrite(wp, "</pre></body></html>\n");
    websDone(wp);
}


#if ME_GOAHEAD_UPLOAD
/*
    Dump the file upload details. Don't actually do anything with the uploaded file.
 */
static void uploadTest(Webs *wp, char *path, char *query)
{
    WebsKey         *s;
    WebsUpload  *up;
    char            *upfile;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteHeader(wp, "Content-Type", "text/plain");
    websWriteEndHeaders(wp);
    if (scaselessmatch(wp->method, "POST")) {
        for (s = hashFirst(wp->files); s; s = hashNext(wp->files, s)) {
            up = s->content.value.symbol;
            websWrite(wp, "FILE: %s\r\n", s->name.value.string);
            websWrite(wp, "FILENAME=%s\r\n", up->filename);
            websWrite(wp, "CLIENT=%s\r\n", up->clientFilename);
            websWrite(wp, "TYPE=%s\r\n", up->contentType);
            websWrite(wp, "SIZE=%d\r\n", up->size);
            upfile = sfmt("%s/tmp/%s", websGetDocuments(), up->clientFilename);
            rename(up->filename, upfile);
            wfree(upfile);
        }
        websWrite(wp, "\r\nVARS:\r\n");
        for (s = hashFirst(wp->vars); s; s = hashNext(wp->vars, s)) {
            websWrite(wp, "%s=%s\r\n", s->name.value.string, s->content.value.string);
        }
    }
    websDone(wp);
}
#endif


#if ME_GOAHEAD_LEGACY
/*
    Legacy handler with old parameter sequence
 */
static int legacyTest(Webs *wp, char *prefix, char *dir, int flags)
{
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteHeader(wp, "Content-Type", "text/plain");
    websWriteEndHeaders(wp);
    websWrite(wp, "Hello Legacy World\n");
    websDone(wp);
    return 1;
}

#endif

static bool websLoginUser1(Webs *wp, char *username, char *password)
{
#ifndef __WEB_DEBUG__
	int ret,pri=0;
#endif

	assert(wp);
	assert(wp->route);
	assert(username);
	assert(password);

	if (!wp->route) {
	    return 0;
	}
	if(wp->username)
	{
		wfree(wp->username);
	}
	wp->username = sclone(username);
	if(wp->password)
	{
		wfree(wp->password);
	}
	wp->password = sclone(password);

	if(wp->ipaddr==NULL || strnlen(wp->ipaddr,15)<1)/*for client ip not correct at this time @huangzhun*/
	{
#ifndef __WEB_DEBUG__
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"websClientIp=%s \r\n",websClientIp);
#endif
		strncpy(wp->ipaddr, websClientIp, min(sizeof(wp->ipaddr) - 1, strlen(websClientIp)));
	}
#ifndef __WEB_DEBUG__
	ret=sys_user_login(username,password,&pri);
	//printf("websLoginUser1 ret:%d \r\n",ret);
	if(ret!=USER_LOGIN_SUCCESS)/*login failed*/
	{
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] login failed.\r\n", username);
        trace(2, "Password does not match");
        if(Check_User_Block(inet_addr(wp->ipaddr),1)== 2)/*locked*/
		{
			 return 2;
		}else{
			 loginSetBlock(inet_addr(wp->ipaddr),0,1);
	        	 return 0;
		}
	}
#endif
#ifndef __WEB_DEBUG__
	if(Check_User_Block(inet_addr(wp->ipaddr),1)== 2)/*locked*/
	{
		 return 2;
	}else{
		loginSetBlock(inet_addr(wp->ipaddr),1,1);
	    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] login successful.\r\n", username);
		trace(2, "Login successful for %s", username);
#endif
		websSetSessionVar(wp, WEBS_SESSION_USERNAME, wp->username);
		return 1;
#ifndef __WEB_DEBUG__
	}
#endif
}


static void loginServiceProc(Webs *wp)
{
	WebsRoute   *route;
	char	*userName,*password, *lang;
	int flag =0,lang_flag=0;

	assert(wp);
	route = wp->route;
	assert(route);

	userName = websGetVar(wp, "username", NULL);
	password = websGetVar(wp, "password", NULL);
	lang = websGetVar(wp, "language", NULL);

	if(userName==NULL || password==NULL  ||
		strlen(userName)<1 ||strlen(password)<1 ||
		strlen(userName) >USER_STR_LEN_MAX||strlen(password) >USER_STR_LEN_MAX)
	{
		websSetStatus(wp, HTTP_CODE_OK);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);

        	websWrite(wp,WEB_RET_USER_LOGIN_PASSWORD_ERR);
		websDone(wp);
		return;
	}

    printf("lang:%s\n", lang);
    flag=websLoginUser1(wp, userName, password);
    if(flag==1)
    {
        if(lang!=NULL)
        {
            lang_flag=atoi(lang);
            if(lang_flag==1)
            {
                g_lang_en=LANG_ENGLISH;
                websSetDocuments(HOME_DIR_EN);
            }
            else 
            {
                g_lang_en=LANG_CHINESE;
                websSetDocuments(HOME_DIR_CH);
            }
        }
        websSetStatus(wp, HTTP_CODE_OK);
        websWriteHeaders(wp, -1, 0);
        websWriteEndHeaders(wp);
        websWrite(wp,WEB_RET_CONFIG_OK);
        websSetSessionVar(wp, "loginStatus", "ok");
        websDone(wp);
    }else if(flag==2){
        if (route->askLogin) {
            (route->askLogin)(wp);
        }
        websSetStatus(wp, HTTP_CODE_OK);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_USER_LOGIN_LOCK);
		websDone(wp);
	}
	else
	{
		if (route->askLogin) {
		    (route->askLogin)(wp);
		}
		websSetStatus(wp, HTTP_CODE_OK);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_USER_LOGIN_PASSWORD_ERR);
		websDone(wp);
	}
}

static void logoutServiceProc(Webs *wp)
{
    assert(wp);
    websRemoveSessionVar(wp, WEBS_SESSION_USERNAME);
    if (smatch(wp->authType, "basic") || smatch(wp->authType, "digest")) {
        websError(wp, HTTP_CODE_UNAUTHORIZED, "Logged out.");
        return;
    }
    websRedirectByStatus(wp, HTTP_CODE_OK);
}

#if 0
static void web_add_user(Webs *wp, char *path, char *query)
{
	char	*userName,*password, *repassword;
	int firstPos,i,ret;

	userName = websGetVar(wp, "userName", NULL);
	password = websGetVar(wp, "password", NULL);
	repassword = websGetVar(wp, "repassword", NULL);

	if(userName==NULL || password==NULL || repassword==NULL ||
		strlen(userName)<1 ||strlen(password)<1  || strlen(repassword)<1 ||
		strlen(userName) >USER_STR_LEN_MAX||strlen(password) >USER_STR_LEN_MAX  || strlen(repassword) >USER_STR_LEN_MAX  ||
		!web_param_valid_check(userName) ||!web_param_valid_check(password) ||!web_param_valid_check(repassword))
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] add user error.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_PARMA_ERR);
		websDone(wp);
		return;
	}
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	if (!isalpha((int) *userName))
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] add user error.\r\n", wp->username);
		websWrite(wp, WEB_RET_USER_NAME_ERR);
		websDone(wp);
		return ;
	}

	if(strcmp(password, repassword) != 0)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] add user error.\r\n", wp->username);
		websWrite(wp, WEB_RET_USER_PASSWORD_MODIFY_ERR);
		websDone(wp);
		return;
	}

	ret=sys_add_user(userName,password,CMD_PRI_ADMIN);
	if(ret==USER_EXITS)
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] add user error.\r\n", wp->username);
		websWrite(wp, WEB_RET_USER_IS_EXIST);
		websDone(wp);
		return;
	}
	else if(ret==USER_OVER_MAX)
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] add user error.\r\n", wp->username);
		websWrite(wp, WEB_RET_MAX_NUMBER_ERR);
		websDone(wp);
		return;
	}
	else
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] add user error.\r\n", wp->username);
		websWrite(wp, WEB_RET_CONFIG_OK);
		websDone(wp);
	}
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] add user sucessful.\r\n", wp->username);
	return;
}

static void web_modify_user(Webs *wp, char *path, char *query)
{
	char	*userName,*oldpassword,*newpassword, *repassword;
	int firstPos,i,ret,active_flag=0;

	userName = wp->username;
	oldpassword = websGetVar(wp, "oldpassword", NULL);
	newpassword = websGetVar(wp, "newpassword", NULL);
	repassword = websGetVar(wp, "confirmpassword", NULL);

	if(userName==NULL || oldpassword==NULL || newpassword==NULL || repassword==NULL ||
		strlen(oldpassword)<1 ||strlen(newpassword)<1  || strlen(repassword)<1 ||
		strlen(oldpassword) >USER_STR_LEN_MAX ||strlen(newpassword)>USER_STR_LEN_MAX  || strlen(repassword) >USER_STR_LEN_MAX  ||
		!web_param_valid_check(oldpassword) ||!web_param_valid_check(newpassword) ||!web_param_valid_check(repassword))
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] modify user fail.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_PARMA_ERR);
		websDone(wp);
		return;
	}

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"userName=%s oldpassword=%s newpassword=%s repassword=%s \n",userName,oldpassword,newpassword,repassword);
	if(strcmp(newpassword, repassword) != 0)
	{
	     LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] modify user fail.\r\n", wp->username);
		websWrite(wp, WEB_RET_USER_PASSWORD_MODIFY_ERR);
		websDone(wp);
		return;
	}

    if(CheckPwdComplexity(userName,newpassword,0)!=0)
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] modify user fail.\r\n", wp->username);
		websWrite(wp, WEB_RET_USER_LOGIN_PASSWORD_SIMPLE);
		websDone(wp);
		return ;
	}

	ret=sys_user_modify_password(userName,oldpassword,newpassword);
	if(ret==SUCCESS)
	{
		websWrite(wp, WEB_RET_CONFIG_OK);
		websDone(wp);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] modify user success.\r\n", wp->username);
	}
	else if(ret==PASSWD_INVALID)
	{
	    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] modify user fail.\r\n", wp->username);
		websWrite(wp, WEB_RET_USER_PASSWORD_MODIFY_ERR2);
		websDone(wp);
	}
	else
	{
	    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] modify user fail.\r\n", wp->username);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
	}

	return;
}

static void web_del_user(Webs *wp, char *path, char *query)
{
	char	*option, *key, *value, *index,*userinfo,*p;
	char userName[5][16]={0},password[16]={0};
	int i=0,ret=OK,num=0,active=0;

	userinfo = websGetVar(wp, "edtDelUser", NULL);
	if((userinfo==NULL || strlen(userinfo)<1)  || strlen(userinfo) >USER_STR_LEN_MAX || !web_param_valid_check(userinfo))
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] delete user fail.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_PARMA_ERR);
		websDone(wp);
		return;
	}
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"11userName=%s len=%d\r\n",userinfo, strlen(userinfo));
	p=strtok(userinfo,";");
	while(p)
	{
		strcpy(&userName[num],p);
		printf("user(%d):%s \n",i+1,userName[num]);
		num++;
		p = strtok(NULL,";");
	}

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	for(i=0;i<num;i++)
	{
		if(vosStrCmp(&userName[i],"admin")==0)
		{
			LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] delete user fail.\r\n", wp->username);
			websWrite(wp,WEB_RET_USER_DONOT_DEL_ADMIN);
			websDone(wp);
			return;
		}
		ret|=sys_del_user(&userName[i],NULL,0,&active);
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"user=%s ret=%d\r\n",&userName[i],ret);
	}

	if(ret==OK)
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] delete user successful.\r\n", wp->username);
		websWrite(wp,WEB_RET_CONFIG_OK);
	}
	else
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] delete user fail.\r\n", wp->username);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
	}

	websDone(wp);
	return;
}

static int UserInfShow(Webs *wp, char *path, char *query)
{
	int i,rc,j,Len=0;
	char *data=NULL;

    cJSON *data_json = NULL;
    cJSON *user_json = NULL;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    data_json = cJSON_CreateArray();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }

	for(i=0;i<SYS_USER_MAX;i++)
	{
	    if ((vosStrCmp(g_user_db[i].name,"root")==0) ||(vosStrCmp(g_user_db[i].name,"bcm")==0))
		{
			continue;
		}
		if(g_user_db[i].flag)
		{
		    cJSON_AddItemToArray(data_json, user_json=cJSON_CreateObject());
            cJSON_AddStringToObject(user_json, "user", g_user_db[i].name);
            cJSON_AddStringToObject(user_json, "authority", i==0 ? "Administrator" : "User");
            cJSON_AddStringToObject(user_json, "status", g_user_db[i].active==USER_STATE_ACTIVE? "active":"inactive");
		}
	}
    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"user data=%s len=%d\r\n",data,Len);

	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;

	return 0;
}
#endif

static int showUsername(int eid, Webs *wp, int argc, char **argv)
{
	websWrite(wp, wp->username);

	return 0;
}

#if 0
static void web_clear_flow(Webs *wp, char *path, char *query)
{
	char	*key,*value,buf[5]={0};
	int 	port,i;
	key = stok(wp->input.buf, "=", &value);


	if(key==NULL || strlen(key)<1 ||value==NULL || strlen(value)<1)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] clear  flow  fail.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	if (strcmp(key, "flow_port")==0)
	{
		if(0==vosStrCmp(value,"all"))/*value="all"*/
		{
			port=-1;
		}
		else/*value="XE1"~"XE68"*/
		{
			strcpy(buf,value);
			DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG,"buf:%s ,value:%s \r\n",buf,value);
			if(strlen(value)>3)/*XE10~XE68*/
			{
				port = vosAscToHex(buf[2])*10+vosAscToHex(buf[3]);
			}else{/*XE1~XE9*/
				port = vosAscToHex(buf[2]);
			}
		}
	}

	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG,"port=%d\r\n",port);
	if(port == -1)
	{
		for(i=1;i<=PORT_TOTOL_MAX_NUM;i++)
		{
			adp_port_statistics_clear(i);
		}
	}
	else
	{
		adp_port_statistics_clear(port);
	}
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] clear  flow  successful.\r\n", wp->username);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	websDone(wp);

}

void web_get_single_trans(uint port,uint *flag)
{

	adp_port_linkdown_trans_get(port, flag);
}
static void web_set_port_admin_state(Webs *wp, char *path, char *query)
{
	int card=1,port=0,ifIndex,shutdown,i=0;
	int AdminState=0,err_flag=0;
	char	*PortAdmin,*p=NULL;
	char Admin[4]={0};

	PortAdmin = websGetVar(wp, "edtEnablePort", NULL);

	if(PortAdmin==NULL || !strlen(PortAdmin) ||(!web_param_valid_check(PortAdmin)))
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port shutdown fail,input error.\r\n", wp->username);
		 err_flag=1 ;
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, " web_set_port_attribute PortAdmin %s \r\n",PortAdmin);
	p=strtok(PortAdmin,",");
	while(p)
	{
		Admin[i]=atoi(p);
		i++;
		p = strtok(NULL,",");
	}
	port=Admin[0];
	AdminState=Admin[1];

	shutdown=(AdminState==1) ? PL_ETHPORT_STATE_UP : PL_ETHPORT_STATE_DOWN;

   	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "port:%d admin:%s",port,(shutdown==PL_ETHPORT_STATE_UP) ? "up" : "down");

	if(port<=PORT_TOTOL_MAX_NUM)
	{
		if (check_port_is_invalid_state(port))
       	{
       	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d shudown fail, invalid port\r\n", wp->username, port);
			err_flag=1 ;
		}
		ifIndex=get_port_logic_num(card,port);
		if(ifIndex==ERROR)
		{
			printf("  %%Failed to get interface logical index.");
			LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d shudown fail, port id error\r\n", wp->username, port);
			err_flag=1 ;
		}
		if(adp_port_mng_state_set(ifIndex,shutdown)!=OK)
		{
			printf("  %%Failed to no shutdown interface.%s");
			LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d shudown fail.\r\n", wp->username, port);
			err_flag=1 ;
		}
	}
	websSetStatus(wp, 200);
out:

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if(err_flag==1)
	{
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		if (PL_ETHPORT_STATE_UP == shutdown)
		{
			LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] undo port %d shudown fail.\r\n", wp->username, port);
		}
		else
		{
			LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d shudown fail.\r\n", wp->username, port);
		}
	}
	else
	{
	    if (PL_ETHPORT_STATE_UP == shutdown)
		{
			LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] undo port %d shudown successful.\r\n", wp->username, port);
		}
		else
		{
			LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d shudown successful.\r\n", wp->username, port);
		}
		websWrite(wp, WEB_RET_CONFIG_OK);
	}
	websDone(wp);
}

static void web_set_port_SingleTrans(Webs *wp, char *path, char *query)
{
	int card=1,port=0,ifIndex,i=0;
	char	*SingleTrans,*p=NULL;
	char single[4]={0};
	int SingleState=0,separated=0,ret=0,err_flag=0;

	SingleTrans = websGetVar(wp, "edtSingSend", NULL);

	if(SingleTrans==NULL || !strlen(SingleTrans) ||(!web_param_valid_check(SingleTrans)))
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port set single transmit fail.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, " SingleTrans=%s len= %d \r\n",SingleTrans,strlen(SingleTrans));
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	p=strtok(SingleTrans,",");
	while(p)
	{
		single[i]=atoi(p);
		printf("test:%d \n",single[i]);
		i++;
		p = strtok(NULL,",");
	}
	if(i==0)
	{
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
	}
	port=single[0];
	SingleState=single[1];

	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"port=%d  state=%d\r\n",port,SingleState);

	if (!check_port_is_invalid_state(port))
    {
		ret=adp_port_linkdown_trans_set(port, SingleState);
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"ret=%d\r\n",ret);
		if(ret==OK)
		{
			if (SingleState)
			{
		    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d single transmit successful.\r\n", wp->username, port);
			}
			else
			{
				LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] undo port %d single transmit successful.\r\n", wp->username, port);
			}
			websWrite(wp, WEB_RET_CONFIG_OK);
			websDone(wp);
			return;
		}
    }
	if (SingleState)
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d single transmit fail.\r\n", wp->username, port);
	}
	else
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] undo port %d single transmit fail.\r\n", wp->username, port);
	}

	websWrite(wp, WEB_RET_CONFIG_FAIL);
	websDone(wp);

}

static void web_set_port_loopback(Webs *wp, char *path, char *query)
{
	int card=1,port=0,ifIndex,separated=0,i=0;
	char	*loopback,*p=NULL;
	int32 loopbackMode=PHY_LOOPBACK;
	char loopbk[4]={0};

	loopback = websGetVar(wp, "edtCircle", NULL);

	if(loopback==NULL|| !strlen(loopback) ||(!web_param_valid_check(loopback)))
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port set loopback mode fail.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		 return ;
	}

	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "loopback %s len %d \r\n",loopback,strlen(loopback));
	p=strtok(loopback,",");
	while(p)
	{
		loopbk[i]=atoi(p);
	//	printf("test:%d \n",loopbk[i]);
		i++;
		p = strtok(NULL,",");
	}
	port=loopbk[0];
	loopbackMode=(loopbk[1]==1) ? PHY_LOOPBACK : NONE_LOOPBACK;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	if (check_port_is_invalid_state(port))
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set loopback mode fail.\r\n", wp->username, port);
		websWrite(wp, WEB_RET_PORT_INVAIT_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	ifIndex=get_port_logic_num(card,port);
	adp_port_loop_back_set(ifIndex,loopbackMode);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"port= %d loopbackMode=%d \r\n",port,loopbackMode);
	if (NONE_LOOPBACK == loopbackMode)
	{
    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] undo port %d loopback mode successful.\r\n", wp->username, port);
	}
	else
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set loopback mode successful.\r\n", wp->username, port);
	}
	websWrite(wp, WEB_RET_CONFIG_OK);
	websDone(wp);
}

static void web_set_port_speed(Webs *wp, char *path, char *query)
{
	int card=1,port=0,negation,ifIndex,speed,duplex;
	char	*PortSpeed,*p=NULL;
	int Atrribute=0,separated=0,i=0;
	char portAtt[4]={0};

	PortSpeed = websGetVar(wp, "edtSpeed", NULL);

	if(PortSpeed==NULL || !strlen(PortSpeed) ||(!web_param_valid_check(PortSpeed)))
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port set speed  fail.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		 return ;
	}

	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, " PortSpeed=%s len = %d \r\n",PortSpeed,strlen(PortSpeed));
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	p=strtok(PortSpeed,",");
	while(p)
	{
		portAtt[i]=atoi(p);
	//	printf("test:%d \n",portAtt[i]);
		i++;
		p = strtok(NULL,",");
	}
	port=portAtt[0];
	Atrribute=portAtt[1];

   	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port-id:%d,PortAtt:%d\r\n",port,Atrribute);

	negation=(Atrribute==0) ? PL_ETHPORT_AUTO_NEG_ENABLE : PL_ETHPORT_AUTO_NEG_DISABLE;
	if(Atrribute==1)/*千兆半双工*/
	{
		speed=PL_ETHPORT_SPEED_1000;
		duplex=PL_ETHPORT_DUPLEX_HALF;
	}
	else if(Atrribute==2)/*千兆 双工*/
	{
		speed=PL_ETHPORT_SPEED_1000;
		duplex=PL_ETHPORT_DUPLEX_FULL;
	}
	else if(Atrribute==3)/*10GE  双工*/
	{
		speed=PL_ETHPORT_SPEED_10G;
		duplex=PL_ETHPORT_DUPLEX_FULL;
	}
#if 0
	if(port>PORT_10GE_NUM && port<=(PORT_10GE_NUM+PORT_40GE_NUM))//40G口只能配置为40G全双工
	{
		speed=PL_ETHPORT_SPEED_40G;
		duplex=PL_ETHPORT_DUPLEX_FULL;
	}
#endif
   	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "negation:%d,",negation);
   	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "speed:%s,",(speed==PL_ETHPORT_SPEED_1000) ? "1000" : "10G");
   	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "duplex:%s \r\n",(duplex==PL_ETHPORT_DUPLEX_FULL) ? "full" : "half");

	if (check_port_is_invalid_state(port))
       {
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set speed  fail.\r\n", wp->username,port);
		websWrite(wp,WEB_RET_PORT_INVAIT_CONFIG_FAIL);
		websDone(wp);
		return;
	}

	ifIndex=get_port_logic_num(card,port);
	if(ifIndex==ERROR)
	{
	    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set speed %s  fail.\r\n", wp->username, port, (speed==PL_ETHPORT_SPEED_1000) ? "1000" : "10G");
		websWrite(wp,WEB_RET_PORT_INVAIT_CONFIG_FAIL);
		websDone(wp);
		return ;
	}

	if(PL_ETHPORT_AUTO_NEG_ENABLE==negation)
	{
		if(adp_port_negation_set(ifIndex,negation)!=OK)
		{
		     	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set negation fail.\r\n", wp->username, port);
			websWrite(wp,WEB_RET_CONFIG_FAIL);
			websDone(wp);
			return ;
		}
	}
	else
	{
		if(adp_port_negation_set(ifIndex,negation)!=OK)
		{
		    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set negation  fail.\r\n", wp->username, port);
			websWrite(wp,WEB_RET_CONFIG_FAIL);
			websDone(wp);
			return ;
		}
		if(adp_port_speed_set(ifIndex,speed)!=OK)
		{
  	          	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set speed %s fail.\r\n", wp->username, port, (speed==PL_ETHPORT_SPEED_1000) ? "1000" : "10G");
			websWrite(wp,WEB_RET_CONFIG_FAIL);
			websDone(wp);
			return ;
		}
		if(adp_port_duplex_set(ifIndex,duplex)!=OK)
		{
		    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set duplex  fail.\r\n", wp->username, port);
			websWrite(wp,WEB_RET_CONFIG_FAIL);
			websDone(wp);
			return ;
		}
	}
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set speed %s successful.\r\n", wp->username, port,(speed==PL_ETHPORT_SPEED_1000) ? "1000" : "10G");
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	websWrite(wp,WEB_RET_CONFIG_OK);
	websDone(wp);
}

static void web_get_port_attribute(Webs *wp, char *path, char *query)
{
	int ret,i=0;
	unsigned int port,mngStatus,speed,separa,loopback,singletrans,single_temp;
	unsigned char negotiation,duplex,linkStatus;
	t_Sw_EthIf if_info;
	char buff[50]={0};
	char *data=NULL;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	data=vosAlloc(1000);
	if(data==NULL)
	{
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	memset(data,0,1000);
	for(port=1; port<=PORT_TOTOL_MAX_NUM; port++)
	{
		ret=get_port_info(1,1,port,&if_info);
		if(ret!=OK)
		{
			mngStatus=4;
			speed=0;
			separa=0;
			singletrans=0;
			loopback=0;
		}
		else
		{
			DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "test: %d %d %d %d %d\r\n",if_info.mngStatus,if_info.linkStatus,if_info.negotiation,if_info.duplex,if_info.speed);
			negotiation=if_info.negotiation;
			duplex=if_info.duplex;

			mngStatus=(if_info.mngStatus!=PL_ETHPORT_STATE_UP) ? 0 : ((if_info.linkStatus==PL_ETHPORT_STATE_UP) ?  2 : 1);
			speed=(negotiation==PL_ETHPORT_AUTO_NEG_ENABLE) ? 0 : ((duplex==PL_ETHPORT_DUPLEX_HALF) ? 1 :( (if_info.speed==PL_ETHPORT_SPEED_10G) ? 3 : 2));
			separa=if_info.beenSeparated;
			web_get_single_trans(port,&single_temp);
			singletrans = single_temp==ENABLE ? 1 : 0;
			loopback=if_info.loopbackMode!=NONE_LOOPBACK ? 1:0;
		}

		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "port:%d admin: %d speed: %d separa: %d\r\n",port,mngStatus,speed,separa);

		if(port/10==0)
		{
			vosIntToStr(buff, port, 10, 0, 0);
			sprintf(&data[i++],"%s",buff);
		}
		else
		{
			vosIntToStr(buff, port/10, 10, 0, 0);
			sprintf(&data[i++],"%s",buff);
			vosIntToStr(buff, port%10, 10, 0, 0);
			sprintf(&data[i++],"%s",buff);
		}

		sprintf(&data[i++],",");
		vosIntToStr(buff, mngStatus, 10, 0, 0);
		sprintf(&data[i++],"%s",buff);
		sprintf(&data[i++],"*");
		vosIntToStr(buff, speed, 10, 0, 0);
		sprintf(&data[i++],"%s",buff);
		sprintf(&data[i++],"*");
		vosIntToStr(buff, separa, 10, 0, 0);
		sprintf(&data[i++],"%s",buff);
		sprintf(&data[i++],"*");

		vosIntToStr(buff, singletrans, 10, 0, 0);
		sprintf(&data[i++],"%s",buff);
		sprintf(&data[i++],"*");

		vosIntToStr(buff, loopback, 10, 0, 0);
		sprintf(&data[i++],"%s",buff);
		sprintf(&data[i++]," ");

	}
	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "data=%s \n",data);
	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;
}

static void web_set_port_separate(Webs *wp, char *path, char *query)
{
	int card=1,port=0,ifIndex,separated=0,i=0;
	char	*sport=NULL,*separate=NULL;
	char loopbk[4]={0};
	char port_mode[4][8]={0},mode[8]={0};

	sport = websGetVar(wp, "portSplit", NULL);
	separate=websGetVar(wp, "splitState", NULL);
	if(sport==NULL|| !strlen(sport) ||separate==NULL ||
		!strlen(separate) || strlen(sport)>5 || strlen(separate)>5 ||
		(!web_param_valid_check(separate)) || (!web_param_valid_check(sport)))
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		 return ;
	}
	if((separate ==NULL) || strlen(separate)<1 || strlen(separate)>5)
	{
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
	    	return ;
	}
	port=atoi(sport);
	if(atoi(separate))
	{
		strcpy(mode,"4X10G");
	}
	else
	{
		strcpy(mode,"1X40G");
	}

	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d separate %s mode=%s\r\n",port,separate,mode);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	if ((port > 52)||(port)< 49)
	{
		websWrite(wp, WEB_RET_PORT_INVAIT_CONFIG_FAIL);
		websDone(wp);
	    	return ;
	}

	get_40gport_separated_state(1,port, &separated);

	if(separated==atoi(separate))
	{
		websWrite(wp, WEB_RET_PORT_INVAIT_CONFIG_FAIL);
		websDone(wp);
	    	return ;
	}

	(void)ReadFile(PORT_MODE_FILE, port_mode, sizeof(port_mode));
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"port_mode:%s, %s,%s,%s\r\n",port_mode[0],port_mode[1],port_mode[2],port_mode[3]);
	memcpy(&port_mode[port-49][0], mode, 5);
	(void)WriteFile(PORT_MODE_FILE, port_mode, sizeof(port_mode));

	websWrite(wp, WEB_RET_CONFIG_OK);
	websDone(wp);
}

#define  INTTOSTR(buffer, value)  do{vosIntToStr(buffer, value, 10, 0, 0);}while(0)
#define  HEXTOSTR(buffer, value)  do{vosIntToStr(buffer, value, 16, 0, 0);}while(0)

void web_acl_get_mac(char *Data, int *Pos, char * Mac)
{
    char mac[6]={0};
    int i=0;
	memcpy(mac,Mac,MAC_ADDR_LEN);
        *Pos += vosSprintf((Data+(*Pos)), "%02x-%02x-%02x-%02x-%02x-%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    return;
}

void web_acl_get_ip(char *Data, int *Pos, int ipaddr)
{
    int i=0;
    *Pos += vosSprintf((Data+(*Pos)), "%s", vosIpIntToStr(ipaddr));
    return;
}

void web_acl_get_ipv6(char *Data, int *Pos, int ipaddr)
{
    int i=0;
    *Pos += vosSprintf((Data+(*Pos)), "%s", vosIpv6IntToStr(ipaddr));
    return;
}

void web_acl_get_int_str(char *Data, int *Pos, int value)
{
    char buffer[32]={0};
    INTTOSTR(buffer, value);
    *Pos += vosSprintf((Data+(*Pos)), "%s", buffer);
    return;
}
void web_acl_get_hex_str(char *Data, int *Pos, int value)
{
    char buffer[32]={0};
    HEXTOSTR(buffer, value);
    *Pos += vosSprintf((Data+(*Pos)), "%s", buffer);
    return;
}

void web_acl_get_portlist(char *Data, int *Pos, int value)
{
    char buffer[8]={0};
    char * portlist = (char *)value;
    int i,num=0;
    for(i = 0; i < PORT_TOTOL_MAX_NUM; i++)
    {
        if (portlist[i] == 1)
        {
        	if(num>0)
        	{
	            *Pos += vosSprintf((Data+(*Pos)), ",");
		}
            *Pos += vosSprintf((Data+(*Pos)), "%d", i+1);
		num++;
        }
    }
    return;
}

void web_get_dmac(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_mac(Data, Pos, condition->con.exact.dmac);
    *Pos+=vosSprintf((Data+(*Pos)), " dmac_mask:");
    web_acl_get_mac(Data, Pos, condition->con.exact.dmac_mask);
    return;
}
void web_get_smac(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_mac(Data, Pos, condition->con.exact.smac);
    *Pos+=vosSprintf((Data+(*Pos)), " smac_mask:");
    web_acl_get_mac(Data, Pos, condition->con.exact.smac_mask);
    return;
}
void web_get_dip(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_ip(Data, Pos, condition->con.exact.dip.addr.ipv4);
    *Pos += vosSprintf((Data+(*Pos)), " dip_mask:");
    web_acl_get_ip(Data, Pos, condition->con.exact.dip_mask.ipv4);
     return;
}
void web_get_sip(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_ip(Data, Pos, condition->con.exact.sip.addr.ipv4);
    *Pos+=vosSprintf((Data+(*Pos)), " sip_mask:");
    web_acl_get_ip(Data, Pos, condition->con.exact.sip_mask.ipv4);
     return;
}

void web_get_dipv6(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_ipv6(Data, Pos, condition->con.exact.dip.addr.ipv6);
    *Pos += vosSprintf((Data+(*Pos)), " dipv6_mask:");
    web_acl_get_ipv6(Data, Pos, condition->con.exact.dip_mask.ipv6);
     return;
}
void web_get_sipv6(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_ipv6(Data, Pos, condition->con.exact.sip.addr.ipv6);
    *Pos+=vosSprintf((Data+(*Pos)), " sipv6_mask:");
    web_acl_get_ipv6(Data, Pos, condition->con.exact.sip_mask.ipv6);
     return;
}

void web_get_sport(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_int_str(Data, Pos, condition->con.exact.sport);
}

void web_get_dport(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_int_str(Data, Pos, condition->con.exact.dport);
     return;
}

void web_get_cvlan(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_int_str(Data, Pos, condition->con.exact.cvlan[0].vlan);
     return;
}

void web_get_svlan(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_int_str(Data, Pos, condition->con.exact.svlan[0].vlan);
     return;
}

void web_get_ethtype(char *Data, int *Pos, acl_condition_s * condition)
{
#if 0
    web_acl_get_hex_str(Data, Pos, condition->con.exact.eth_type);
#else

	ushort type=0,str_len=0,i,len=0;
	char buf[6]={0},data[8]={0};

	type=condition->con.exact.eth_type;
	str_len=vosSprintf(buf,"%02x",type);
	len=vosSprintf(data+len,"0x");
	for(i=0;i<4-str_len;i++)
	{
		len+=vosSprintf(data+len,"0");
	}
	len+=vosSprintf(data+len,"%s",buf);
       *Pos += vosSprintf((Data+(*Pos)), "%s", data);


#endif
}

void web_get_proto(char *Data, int *Pos, acl_condition_s * condition)
{
	uint8 protocol=0;
	acl_protocol_type_pro protocol_type[]=
	{
		{IP_ICMP_TYPE_VALUE,		"icmp"},
		{IP_IGMP_TYPE_VALUE,		"igmp"},
		{IP_TCP_TYPE_VALUE,		"tcp"},
		{IP_UDP_TYPE_VALUE,		"udp"},
		{IP_GRE_TYPE_VALUE,		"gre"},

	};

	for (protocol = 0; protocol < (sizeof(protocol_type)/sizeof(acl_protocol_type_pro)); protocol++)
	{
		if(protocol_type[protocol].protocol == condition->con.exact.l3_proto)
		{
		    *Pos += vosSprintf((Data+(*Pos)), "%s", protocol_type[protocol].string);
			return;
		}
	}
    *Pos += vosSprintf((Data+(*Pos)), "customer_%d",condition->con.exact.l3_proto);
     return;
}

void web_get_tcp_flag(char *Data, int *Pos, acl_condition_s * condition)
{
	int flag=0;
	acl_tcp_flag_pro tcp_flag[]=
	{
		{TCP_FLAG_ACK_VALUE,		"ack"},
		{TCP_FLAG_FIN_VALUE,			"fin"},
		{TCP_FLAG_PSH_VALUE,		"psh"},
		{TCP_FLAG_RST_VALUE,		"rst"},
		{TCP_FLAG_SYN_VALUE,		"syn"},
		{TCP_FLAG_URG_VALUE,		"urg"},
	};

	for(flag=0;flag<TCP_FLAG_MAX;flag++)
	{
		if(condition->con.exact.tcp_flag & (tcp_flag[flag].flag))
		{
	            *Pos += vosSprintf((Data+(*Pos)),"%s,", tcp_flag[flag].string);
		}
	}
	return;
}

void web_get_source_port(char *Data, int *Pos, acl_condition_s * condition)
{
    web_acl_get_portlist(Data, Pos, condition->con.exact.src_ports);
    return;
}

void web_acl_get_action_mirror(char *Data, int *Pos, acl_action_s * action)
{
	web_acl_get_int_str(Data, Pos, action->mirror_port);
}
void web_acl_get_action_redirect(char *Data, int *Pos, acl_action_s * action)
{
	web_acl_get_portlist(Data, Pos, action->redict.portlist);
    return;
}
void web_acl_get_action_redirect_trunk(char *Data, int *Pos, acl_action_s * action)
{
	web_acl_get_int_str(Data, Pos, action->redict.trunkid);
}
void web_acl_get_action_vlan(char *Data, int *Pos, acl_action_s * action)
{
    if ((ACL_VLAN_ACTION_INSERT_IN_TAG_VALUE == action->tag_action.act)
        || (ACL_VLAN_ACTION_REPLACE_IN_TAG_VALUE== action->tag_action.act)
         || (ACL_VLAN_ACTION_DELETE_IN_TAG_VALUE== action->tag_action.act))
    {
        web_acl_get_int_str(Data, Pos, action->tag_action.in_tag.vlan);
    }
    if ((ACL_VLAN_ACTION_INSERT_OUT_TAG_VALUE == action->tag_action.act)
        || (ACL_VLAN_ACTION_REPLACE_OUT_TAG_VALUE== action->tag_action.act)
         || (ACL_VLAN_ACTION_DELETE_OUT_TAG_VALUE== action->tag_action.act))
    {
        web_acl_get_int_str(Data, Pos, action->tag_action.out_tag.vlan);
    }
}
void web_acl_get_action_egress_add(char *Data, int *Pos, acl_action_s * action)
{
	web_acl_get_portlist(Data, Pos, action->egrportlist);
}
void web_acl_get_action_dst_mac_new(char *Data, int *Pos, acl_action_s * action)
{
	web_acl_get_mac(Data, Pos, action->dstMacNew);
}

int  web_check_input_key(Webs *wp, uint * frame)
{
	char  *key;
	char *value;
	char *index=NULL;

	key = stok(wp->input.buf, "=", &value);
	if(key==NULL || strlen(key)<1)
	{
		return RET_FAIL;
	}
	if (strcmp(key, "rule_index")==0)
	{
		index = value;
	}
	*frame =atoi(index);
	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG,"input-index:%d \r\n",*frame);
	if(((*frame)<=0) && ((*frame)>240))
	{

		return RET_FAIL;
	}
	return RET_SUCCESS;
}

void web_fill_rule_page_num(uchar  *data, uint * Lenth, uint rulenum, uint frame)
{
    uint frm=0,Len = *Lenth;

	frm=(rulenum/20)+((rulenum%20)==0 ? 0:1);

	web_acl_get_int_str(data, &Len, frm);
	Len += vosSprintf((data+Len), "&");
/*
	if((rulenum/21 +1< frame ) && ((rulenum+1)/21 +1== frame ))
	{
		frame-=1;
	}
*/
	web_acl_get_int_str(data, &Len, frame);
	Len += vosSprintf((data+Len), "&");
	*Lenth = Len;
	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "len=%d data%s\r\n", Len,data);
	return;
}

void web_fill_rule_ingress_port(uchar  *data, uint * Len, acl_msg_rule_s * conditon, uint con_num)
{
	int loop = 0;

	for(loop = 0; loop < con_num; loop++)
	{
		DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG,"loop:%d mask:%d \r\n",loop,conditon[loop].condition.con.exact.mask);
		if (!(ACL_CONDITION_TYPE_EXACT_VALUE & conditon[loop].condition.flag))
		{
			continue;
		}
    	if(conditon[loop].condition.con.exact.mask & ACL_CONDITION_MASK_SRC_PORT_VALUE)
    	{
    	   (*Len) += vosSprintf((data+(*Len)), "%s","ingress_port:");
    		web_get_source_port(data, Len, &conditon[loop].condition);
		}
	}

	return;
}

void web_fill_rule_exact_condition(uchar  *data, uint * Lenth, acl_msg_rule_s * conditon, uint con_num)
{
	int loop = 0;
	int i = 0;
	char buff[32];
	uint Len = *Lenth;

	acl_trans_str_s exact_condition[]={
		{ACL_CONDITION_MASK_SMAC_VALUE,	"smac:",    web_get_smac},
		{ACL_CONDITION_MASK_DMAC_VALUE,	"dmac:",  web_get_dmac},
		{ACL_CONDITION_MASK_SIP_VALUE,  "sipaddr:",    web_get_sip},
		{ACL_CONDITION_MASK_DIP_VALUE,	"dipaddr:",	     web_get_dip},
		{ACL_CONDITION_MASK_SIPV6_VALUE,"sipv6:",	 web_get_sipv6},
		{ACL_CONDITION_MASK_DIPV6_VALUE,"dipv6:",	 web_get_dipv6},
		{ACL_CONDITION_MASK_SPORT_VALUE,"sport:",	 web_get_sport},
		{ACL_CONDITION_MASK_DPORT_VALUE,"dport:",	  web_get_dport},
		{ACL_CONDITION_MASK_SVLAN_VALUE,"svlan:",       web_get_svlan},
		{ACL_CONDITION_MASK_CVLAN_VALUE,"cvlan:",       web_get_cvlan},
		{ACL_CONDITION_MASK_ETH_TYPE_VALUE,"ethtype:",		 web_get_ethtype},
		{ACL_CONDITION_MASK_L3_PROTOCOL_VALUE,"protocol:",   web_get_proto},
		{ACL_CONDITION_MASK_TCP_FLAG_VALUE,	"tcp_flag:",	    web_get_tcp_flag},
	};


	for(loop = 0; loop < con_num; loop++)
	{
	    if (ACL_CONDITION_TYPE_EXACT_VALUE & conditon[loop].condition.flag)
    	{
    		for (i = 0; i < sizeof(exact_condition)/sizeof(acl_trans_str_s); i++)
			{
	    		if (conditon[loop].condition.con.exact.mask & exact_condition[i].mask)
    			{
		    		Len += vosSprintf((data+Len), "%s", exact_condition[i].attr_name);
		            if (exact_condition[i].trans_str)
		            {
		                exact_condition[i].trans_str(data, &Len,  &conditon[loop].condition);
		                Len += vosSprintf((data+Len), " ");
		            }
    			}
			}
    	}
	}
	*Lenth = Len;
	return;
}
void web_fill_rule_udf_condition(uchar  *data, uint * Lenth, acl_msg_rule_s * conditon, uint con_num)
{
    int loop = 0;
	int i = 0;
	char buff[32];
	uint Len = *Lenth;

	char  udf_conditon[ACL_UDF_BASE_TYPE_MAX][16]= {"eth#","l3in#","l3out#","l4in#", "l4out#"};
	for(loop = 0; loop < con_num; loop++)
	{
	    if (ACL_CONDITION_TYPE_UDF_VALUE & conditon[loop].condition.flag)
    	{
    	    Len += vosSprintf((data+Len), "UDFlist:%d-winPos#", loop);
			if (conditon[loop].condition.con.udf.base_addr < ACL_UDF_BASE_TYPE_MAX)
			{
				Len += vosSprintf((data+Len), "%s", udf_conditon[conditon[loop].condition.con.udf.base_addr]);
			}
			memset(buff, 0, sizeof(buff));
	        INTTOSTR(buff, conditon[loop].condition.con.udf.offset);
	        Len += vosSprintf((data+Len), "offset#%s",buff);
	        memset(buff, 0, sizeof(buff));
	        vosByteToHexStr(conditon[loop].condition.con.udf.value, buff, conditon[loop].condition.con.udf.lenth);
	        Len += vosSprintf((data+Len), "udf_code#%s",buff);
	        memset(buff, 0, sizeof(buff));
	        vosByteToHexStr(conditon[loop].condition.con.udf.mask, buff, conditon[loop].condition.con.udf.lenth);
	        Len += vosSprintf((data+Len), "udf_mask#%s* ",buff);
    	}

	}
	*Lenth = Len;
	return;
}

void web_fill_rule_action(uchar  *data, uint * Lenth, acl_action_s * action)
{
	int i = 0;
	int Len = *Lenth;
	acl_action_str_s action_proc[]=
	{
		{ACL_ACTION_DROP_VALUE,		"drop",		    NULL},
		{ACL_ACTION_MIRROR_ING_VALUE,	"mirror_port:",	web_acl_get_action_mirror},
		{ACL_ACTION_TO_CPU_VALUE,	"tocpu",		NULL},
		{ACL_ACTION_REDIRECT_PBMP_VALUE, "redirect_port:",	web_acl_get_action_redirect},
		{ACL_ACTION_REDIRECT_TRUNK_VALUE, "redirect_trunk:"	,	web_acl_get_action_redirect_trunk},
		//{ACL_ACTION_MIRROR_EGR_VALUE,"mirror_egr:",		cli_acl_show_action_mirror},
		{ACL_ACTION_EGR_PORTS_ADD_VALUE,"egress_port:",	web_acl_get_action_egress_add},
		{ACL_ACTION_DST_MAC_NEW_VALUE,"dmac_replace:",	web_acl_get_action_dst_mac_new},
		{ACL_ACTION_ADD_TAG_VALUE,	 "add_vlan:",	web_acl_get_action_vlan},
		{ACL_ACTION_DEL_TAG_VALUE,	 "del_vlan:",	web_acl_get_action_vlan},
		{ACL_ACTION_MODIFY_TAG_VALUE,	"modify_vlan:",		web_acl_get_action_vlan},
	};

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "mask:%d\r\n",action->mask);
	for(i = 0; i < (sizeof(action_proc)/sizeof(acl_action_pro)); i++)
	{
	    if(action->mask & action_proc[i].mask)
	    {
	        Len += vosSprintf((data+Len), "%s", action_proc[i].string);
	        if (action_proc[i].TransStrFun)
	        {
	            action_proc[i].TransStrFun(data, &Len,  action);
	            Len += vosSprintf((data+Len), " ");
	        }
	        else
	        {
	            Len += vosSprintf((data+Len), " ");
	        }
	    }
	}
	*Lenth = Len;
	return;
}

static void web_get_rule(Webs *wp, char *path, char *query)
{
	uint ret,tnum=0,rulenum=0, k, Len = 0,frame=0;
	char *data=NULL;
	acl_template_s ptemplate;
	acl_template_s *gptemplates;
	acl_msg_rule_s acl_rules[MAX_SUPPORT_RULE_NUM];
	acl_action_s       action;

	data=vosAlloc(8000);
	if(data==NULL)
	{
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

	gptemplates = vosAlloc(MAX_SUPPORT_FILTER_NUM * sizeof(acl_template_s));
	if (!gptemplates)
	{
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_OK);
		websDone(wp);
	    	free(data);
		data = NULL;
	    	return;
	}

	memset(data,0,8000);
	memset(&action,0,sizeof(action));
    memset(&ptemplate,0,sizeof(ptemplate));
	memset(acl_rules,0,MAX_SUPPORT_RULE_NUM*sizeof(acl_msg_rule_s));
	memset(gptemplates,0,MAX_SUPPORT_FILTER_NUM*sizeof(acl_template_s));

    ret =  web_check_input_key(wp, &frame);
	if(ret != RET_SUCCESS)
	{
		goto out;
	}

	(void)acl_get_template(gptemplates,&rulenum);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"rulenum:%d \r\n",rulenum);

	if(((rulenum < 0) || (rulenum > MAX_SUPPORT_FILTER_NUM) ))
	{
		goto out;
	}

    web_fill_rule_page_num(data, &Len, rulenum, frame);

	for(k= (20 * (frame-1)); k < ((20 * frame)>rulenum? rulenum:(20 * frame)); k++)
	{
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"gptemplates[k].id:%d k:%d\r\n",gptemplates[k].id,k);
		ptemplate.id= gptemplates[k].id;
		ptemplate.succ=gptemplates[k].succ;
		tnum=0;

        memset(acl_rules,0,MAX_SUPPORT_RULE_NUM*sizeof(acl_msg_rule_s));
		ret = acl_get_condition_by_template(&ptemplate, acl_rules, &tnum);
		if((ret!=OK) || (tnum <=0))
		{
			DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "acl_get_condition_by_template error!! ret=%d\r\n",ret);
		}

        /*填充ACL ID*/
		web_acl_get_int_str(data, &Len, (int)ptemplate.id);
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "len=%d data%s\r\n", Len,data);
		Len += vosSprintf((data+Len), "$");
        DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG,"data:%s Len:%d \r\n",data,Len);

		/*填充 ingress port */
		web_fill_rule_ingress_port(data, &Len, acl_rules, tnum);
		Len += vosSprintf((data+Len), "$");
        DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG,"data:%s Len:%d \r\n",data,Len);

		/*填充condition */
		web_fill_rule_exact_condition(data, &Len, acl_rules, tnum);
		DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG,"data:%s Len:%d \r\n",data,Len);

		web_fill_rule_udf_condition(data, &Len, acl_rules, tnum);
		Len += vosSprintf((data+Len), "$");
		DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG,"data:%s Len:%d \r\n",data,Len);

		/*填充action */
        (void)acl_get_action_by_template(&ptemplate, &action);
		web_fill_rule_action(data, &Len, &action);
		Len += vosSprintf((data+Len), "$");

		/*填充用户和规则状态*/
		Len += vosSprintf((data+Len), "admin");//暂时都只填admin用户
		Len += vosSprintf((data+Len), "$");
		Len += vosSprintf((data+Len), "%s",(ptemplate.succ == 1)? "Active" : "Inactive");
		Len += vosSprintf((data+Len), ";");
	}

out:
	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "%s\r\n", data);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data = NULL;
	free(gptemplates);
	gptemplates = NULL;
}

static void web_del_rule(Webs *wp, char *path, char *query)
{
	char	*option, *key, *value, *index=NULL;
	int num=1;
	int ret,i=0;
	acl_template_s ptemplate;

	memset(&ptemplate,0,sizeof(ptemplate));

	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"web_del_rule start \r\n");
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	key = stok(wp->input.buf, "=", &value);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"value:%s,key:%s\r\n",value,key);
	if(key==NULL || strlen(key)<1)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl delete rule fail.\r\n", wp->username);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

	if (strcmp(key, "ruleID")==0)
	{
		index = value;
	}
	else
	{
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	if((index ==NULL) || strlen(index)<1 || strlen(index)>5 ||(!web_param_valid_check(index)))
	{
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	ptemplate.id = atoi(index);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"ptemplate.id:%d\r\n",ptemplate.id);


	if(ptemplate.id<=0 ||ptemplate.id >4096)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl delete rule fail, id %d is out of range.\r\n", wp->username,ptemplate.id);
		websWrite(wp, WEB_RET_CONFIG_OK);
		websDone(wp);
		return;
	}

	if(acllist[ptemplate.id]==0)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl delete rule fail, acl %d is not exist.\r\n", wp->username,ptemplate.id);
		websWrite(wp, WEB_RET_ACL_RULE_ID_NOT_EXIST);
		websDone(wp);
		return;
	}
	acllist[ptemplate.id]=0;

	ret=acl_del_template(&ptemplate);
	if(ret==OK)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl %d delete successful.\r\n", wp->username,ptemplate.id);
		websWrite(wp, WEB_RET_CONFIG_OK);
	}
	else
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl %d delete fail.\r\n", wp->username,ptemplate.id);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
	}

	websDone(wp);
}

static void web_del_rule_multi(Webs *wp, char *path, char *query)
{
	char	*option, *key, *value, *index=NULL,*p;
	int num=1,acl_id=0,ret=0,ret1=0;
	acl_template_s ptemplate;

	memset(&ptemplate,0,sizeof(ptemplate));

	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"web_del_rule start \r\n");
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl batch delete start.\r\n", wp->username);

	key = stok(wp->input.buf, "=", &value);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"value:%s,key:%s\r\n",value,key);
	if(key==NULL || strlen(key)<1)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl batch delete fail.\r\n", wp->username,ptemplate.id);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	if (strcmp(key, "ruleID")==0)
	{
		index = value;
	}else{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl batch delete fail.\r\n", wp->username,ptemplate.id);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

	p=strtok(index,",");
	while(p)
	{
		ptemplate.id = atoi(p);
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"ptemplate.id:%d\r\n",ptemplate.id);
		if(ptemplate.id<=0 ||ptemplate.id >4096)
		{
		    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl batch delete fail,acl %d out of range.\r\n", wp->username,ptemplate.id);
			websWrite(wp, WEB_RET_ACL_RULE_ID_MAX_ERR);
			websDone(wp);
			return;
		}
		if(acllist[ptemplate.id]==0)
		{
		    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl batch delete fail,acl %d is not range.\r\n", wp->username,ptemplate.id);
			websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"规则不存在!\")\r\n</script>");
			websDone(wp);
			return;
		}
		acllist[ptemplate.id]=0;
		ret |=acl_del_template(&ptemplate);
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"ptemplate.id:%d ret=%d\r\n",ptemplate.id,ret);
		p = strtok(NULL,",");
	}

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	if(ret==OK)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl batch delete successful.\r\n", wp->username);
		websWrite(wp, WEB_RET_CONFIG_OK);
	}
	else
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl batch delete fail.\r\n", wp->username);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
	}

	websDone(wp);
}

uchar web_port_list_calc(uchar *s,uchar *port,uint *num)
{
	char i=0;
	char *p=NULL;
	uchar port_list[PORT_TOTOL_MAX_NUM]={0};

	p=strtok(s,",");
	while(p)
	{
		if(vosMemCmp(p,"CE",2)==0)
		{
			port_list[i]=atoi(p+2)+BASE_10G_PORT_NUM;/*CE1-->41*/
		}
		else if(vosMemCmp(p,"XE",2)==0)
		{
			port_list[i]=atoi(p+2);/*XE1-->1*/
		}
		else
		{
			return ERROR;
		}
		i++;
		*num=i;
		p = strtok(NULL,",");
	}
	memcpy(port,port_list,*num);

	return OK;
}
int web_get_portlist(char *inPortNum,acl_condition_s *pacl_condition_s)
{
	int i=0,j=0,port,port_num=0,ret=0;
	char separated,port_list[PORT_TOTOL_MAX_NUM]={0};

	if(inPortNum==NULL || strlen(inPortNum)<=0)
	{
		return RET_FAIL;
	}
	ret=web_port_list_calc(inPortNum,port_list,&port_num);
	if(ret!=OK)/*huangzhun add for app-scan*/
	{
		return RET_FAIL;
	}
	for(i=0;i<=port_num;i++)
	{
		for(port=1;port<=PORT_TOTOL_MAX_NUM;port++)
		{
			if(port_list[i]==port)
			{
				if (check_port_is_invalid_state(port))
		             {
		                    return RET_FAIL;
		             }
				pacl_condition_s->con.exact.src_ports[port-1]=1;
				DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"pacl_condition_s.con.exact.src_ports[i]= %d  , id=%d\r\n",pacl_condition_s->con.exact.src_ports[port-1],port);
			}
		}
	}
	pacl_condition_s->con.exact.mask|=ACL_CONDITION_MASK_SRC_PORT_VALUE;
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"pacl_condition_s->con.exact.mask=%d\r\n",pacl_condition_s->con.exact.mask);
    return RET_SUCCESS;
}


int web_get_rule_param(Webs *wp,web_acl_rule_t *para)
{
	web_acl_rule_t rule_param;
	char *p=NULL;

	para->acl_id = websGetVar(wp, "RuleID", NULL);
	para->smac = websGetVar(wp, "SrcMac", NULL);
	para->smask=websGetVar(wp, "SrcMacMask", NULL);
	para->dmac = websGetVar(wp, "DstMac", NULL);
	para->dmask=websGetVar(wp, "DstMacMask", NULL);
	para->eth_type = websGetVar(wp, "EthType", NULL);
	para->inPortNum = websGetVar(wp, "edtInPorts", NULL);
	para->sip = websGetVar(wp, "SrcIP", NULL);
	para->sip_mask = websGetVar(wp, "SrcMaskSip", NULL);
	para->dip = websGetVar(wp, "DstIP", NULL);
	para->dip_mask = websGetVar(wp, "DstMaskDip", NULL);
	para->pr = websGetVar(wp, "IpProtol", NULL);
	para->udfpr=websGetVar(wp, "cusPro", NULL);
	para->sport = websGetVar(wp, "SrcPort", NULL);
	para->dport = websGetVar(wp, "DstPort", NULL);
	para->length = websGetVar(wp, "DatasLen", NULL);
	para->portFlag = websGetVar(wp, "portFlag", NULL);
	para->udfoffset = websGetVar(wp, "OffsetValue", NULL);
	para->action = websGetVar(wp, "TransAction", NULL);
	para->portmirr = websGetVar(wp, "MirrorPort", NULL);
	para->iptype = websGetVar(wp, "IPType", NULL);
	para->udfbase = websGetVar(wp, "WinPos", NULL);
	para->udfstr = websGetVar(wp, "Udf", NULL);
	para->udfmask = websGetVar(wp, "UdfMask", NULL);
	para->vlan_inner=websGetVar(wp, "VlanInner", NULL);
	para->vlan_outer=websGetVar(wp, "VlanOuter", NULL);
	para->Export=websGetVar(wp, "edtPorts", NULL);
	para->replacemac=websGetVar(wp, "replaceMac", NULL);
	para->tcpflag=websGetVar(wp, "check_boxTcp", NULL);
	para->trunkid=websGetVar(wp, "TrunkGroup", NULL);
	para->UDFList=websGetVar(wp, "UDFList", NULL);
	para->L2Vlan=websGetVar(wp, "L2Vlan", NULL);
	para->L2Vlaninner=websGetVar(wp, "L2VlanInner", NULL);
	para->AddVlanOter=websGetVar(wp, "VlanAddOuter", NULL);
	para->AddVlanOterFlag=websGetVar(wp, "addVlan", NULL);

	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"smac:%s,smask:%s,dmac:%s,dmask:%s,eth_type:%s,inPortNum:%s,trunkid=%s\r\n",para->smac,para->smask,para->dmac,para->dmask,para->eth_type,para->inPortNum,para->trunkid);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"sip:%s,sip_mask:%s,dip:%s,dip_mask:%s,pr:%s,sop:%s,dop:%s\r\n",para->sip,para->sip_mask,para->dip,para->dip_mask,para->pr,para->sport,para->dport);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"length:%s,udfbase:%s,udfoff:%s,udf:%s,udfmask:%s,action:%s,L2Vlan=%s\r\n",para->length,para->udfbase,para->udfoffset,para->udfstr,para->udfmask,para->action,para->L2Vlan);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"portFlag:%s,iptype:%s,vlan_inner:%s,vlan_outer:%s,Export:%s,replacemac:%s,tcpflag:%s,strunkid:%s\r\n",para->portFlag,para->iptype,para->vlan_inner,para->vlan_outer,para->Export,para->replacemac,para->tcpflag,para->trunkid);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"len-inPortNum=%d\r\n",strlen(para->inPortNum));
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"UDFList:%s AddVlanOter=%s AddVlanOterFlag=%s\r\n",para->UDFList,para->AddVlanOter,para->AddVlanOterFlag);
	if(para->acl_id==NULL)
	{
		return ERROR;
	}
	if((para->smac!=NULL) && (!web_param_valid_check(para->smac)) && (!vosValidMacCheck(para->smac)))
	{
		return ERROR;
	}
	if((para->smask!=NULL) && (!web_param_valid_check(para->smask))  && (!vosValidMacCheck(para->smask)))
	{
		return ERROR;
	}
	if((para->dmac!=NULL) && (!web_param_valid_check(para->dmac)) && (!vosValidMacCheck(para->dmac)))
	{
		return ERROR;
	}
	if((para->dmask!=NULL) && (!web_param_valid_check(para->dmask)) && (!vosValidMacCheck(para->dmask)))
	{
		return ERROR;
	}
	if((para->sip!=NULL) && (!web_param_valid_check(para->sip)) && (vosValidIpCheck(para->sip)))
	{
		return ERROR;
	}
	if((para->replacemac!=NULL) && (!web_param_valid_check(para->replacemac)) && (!vosValidMacCheck(para->replacemac)))
	{
		return ERROR;
	}
	if((para->sip_mask!=NULL) && (!web_param_valid_check(para->sip_mask)) && (vosValidIpCheck(para->sip_mask)))
	{
		return ERROR;
	}
	if((para->dip!=NULL) && (!web_param_valid_check(para->dip)) && (vosValidIpCheck(para->dip)))
	{
		return ERROR;
	}
	if(para->inPortNum !=NULL && (!web_param_valid_check(para->inPortNum)))
	{
		return ERROR;
	}
	if(para->eth_type !=NULL && (!web_param_valid_check(para->eth_type)))
	{
		return ERROR;
	}
	if(para->portFlag !=NULL && (!web_param_valid_check(para->portFlag)))
	{
		return ERROR;
	}
	if(para->action !=NULL && (!web_param_valid_check(para->action)))
	{
		return ERROR;
	}
	if(para->portmirr !=NULL && (!web_param_valid_check(para->portmirr)))
	{
		return ERROR;
	}
	if(para->iptype !=NULL && (!web_param_valid_check(para->iptype)))
	{
		return ERROR;
	}
	if(para->trunkid!=NULL && (!web_param_valid_check(para->trunkid)))
	{
		return ERROR;
	}
	if(para->UDFList !=NULL && (!web_param_valid_check(para->UDFList)))
	{
		printf(" UDFList error\r\n");
		return ERROR;
	}
	if(para->udfbase !=NULL && (!web_param_valid_check(para->udfbase)))
	{
		return ERROR;
	}
	if(para->udfstr !=NULL && (!web_param_valid_check(para->udfstr)))
	{
		return ERROR;
	}
	if(para->udfmask !=NULL && (!web_param_valid_check(para->udfmask)))
	{
		return ERROR;
	}
	if(para->udfoffset !=NULL && (!web_param_valid_check(para->udfoffset)))
	{
		return ERROR;
	}
	if((para->dip_mask!=NULL) && (!web_param_valid_check(para->dip_mask)) && (vosValidIpCheck(para->dip_mask)))
	{
		return ERROR;
	}
	if((para->Export!=NULL) && (!web_param_valid_check(para->Export)))
	{
		return ERROR;
	}
	if((para->tcpflag!=NULL) && (!web_param_valid_check(para->tcpflag)))
	{
		return ERROR;
	}
	if((para->pr!=NULL) && (!web_param_valid_check(para->pr)) && (strlen(para->pr)>6 || strlen(para->pr) <3))
	{
		return ERROR;
	}
	if((para->sport!=NULL) && (!web_param_valid_check(para->sport)) && strnlen(para->sport,2) > 1 &&((atoi(para->sport) <=0 )||(atoi(para->sport)>65536)))
	{
		return ERROR;
	}
	if((para->dport!=NULL) && (!web_param_valid_check(para->dport)) && strnlen(para->dport,2) > 1 && ((atoi(para->dport) <=0) ||(atoi(para->dport)>65536)))
	{
		return ERROR;
	}
	if((para->vlan_inner!=NULL) && (!web_param_valid_check(para->vlan_inner)) && strnlen(para->vlan_inner,2) > 1 && ((atoi(para->vlan_inner) <=0) ||(atoi(para->vlan_inner)>4096)))
	{
		return ERROR;
	}
	if((para->vlan_outer!=NULL) && (!web_param_valid_check(para->vlan_outer))  && strnlen(para->vlan_outer,2) > 1 && ((atoi(para->vlan_outer) <=0) ||(atoi(para->vlan_outer)>4096)))
	{
		return ERROR;
	}
	if((para->L2Vlan!=NULL) && (!web_param_valid_check(para->L2Vlan)) && strnlen(para->L2Vlan,2) > 1 && ((atoi(para->L2Vlan) <=0) ||(atoi(para->L2Vlan)>4096)))
	{
		return ERROR;
	}
	if((para->L2Vlaninner!=NULL) && (!web_param_valid_check(para->L2Vlaninner)) && strnlen(para->L2Vlaninner,2) > 1 && ((atoi(para->L2Vlaninner) <=0) ||(atoi(para->L2Vlaninner)>4096)))
	{
		return ERROR;
	}
	if((para->AddVlanOter!=NULL) && (!web_param_valid_check(para->AddVlanOter)) && strnlen(para->AddVlanOter,2) > 1 && ((atoi(para->AddVlanOter) <=0) ||(atoi(para->AddVlanOter)>4096)))
	{
		return ERROR;
	}
	if((para->AddVlanOterFlag!=NULL) && (!web_param_valid_check(para->AddVlanOterFlag)) && (strcmp(para->AddVlanOterFlag,"on")!=0))
	{
		return ERROR;
	}
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"param input check ok ,for debug\r\n");

	return OK;
}

int web_add_condition(web_acl_rule_t *inputparm,acl_template_s *ptemplate)
{
	int ret=OK;
	acl_msg_rule_s acl_rules;
	memset(&acl_rules,0,sizeof(acl_msg_rule_s));

	ret = web_get_portlist(inputparm->inPortNum,&acl_rules.condition);
	if (RET_SUCCESS != ret)
	{
		return ACL_RET_SRC_PORT_ERR;
	}
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"acl_rules.condition.con.exact.mask=%d\r\n",acl_rules.condition.con.exact.mask);

	acl_condition_set_protocol(inputparm->pr,strlen(inputparm->pr),&acl_rules.condition,inputparm->udfpr);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"l3_proto=%x con-mask=%d\r\n",
		acl_rules.condition.con.exact.l3_proto,acl_rules.condition.con.exact.mask);

	acl_condition_set_eth_type(inputparm->eth_type,&acl_rules.condition);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"eth_type=%x con-mask=%d\r\n",
		acl_rules.condition.con.exact.eth_type,acl_rules.condition.con.exact.mask);

	acl_condition_set_l4port(inputparm->sport,inputparm->dport,&acl_rules.condition);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"sport=%x con-mask=%d\r\n",
		acl_rules.condition.con.exact.sport,acl_rules.condition.con.exact.mask);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"dport=%x con-mask=%d\r\n",
		acl_rules.condition.con.exact.dport,acl_rules.condition.con.exact.mask);

	acl_condition_set_tcp_flag(inputparm->tcpflag,strlen(inputparm->tcpflag),&acl_rules.condition);

	ret=acl_condition_set_mac(inputparm->smac,inputparm->smask,inputparm->dmac,inputparm->dmask,&acl_rules.condition);
	if(ret!=RET_SUCCESS)
	{
		return ret;
	}
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"smac[%02x %02x %02x %02x %02x %02x ] con-mask=%d\r\n",
		acl_rules.condition.con.exact.smac[0],acl_rules.condition.con.exact.smac[1],
		acl_rules.condition.con.exact.smac[2],acl_rules.condition.con.exact.smac[3],
		acl_rules.condition.con.exact.smac[4],acl_rules.condition.con.exact.smac[5]);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"dmac[%02x %02x %02x %02x %02x %02x ] con-mask=%d\r\n",
		acl_rules.condition.con.exact.dmac[0],acl_rules.condition.con.exact.dmac[1],
		acl_rules.condition.con.exact.dmac[2],acl_rules.condition.con.exact.dmac[3],
		acl_rules.condition.con.exact.dmac[4],acl_rules.condition.con.exact.dmac[5]);

	acl_condition_set_ipv4(inputparm->sip,inputparm->sip_mask,inputparm->dip,inputparm->dip_mask,inputparm->iptype,&acl_rules.condition);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"sip %s \n",vosIpIntToStr(acl_rules.condition.con.exact.sip.addr.ipv4));
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"dip %s \n",vosIpIntToStr(acl_rules.condition.con.exact.dip.addr.ipv4));
	ret=acl_condition_set_ipv6(inputparm->sip,inputparm->sip_mask,inputparm->dip,inputparm->dip_mask,inputparm->iptype,&acl_rules.condition);
	if(ret!=RET_SUCCESS)
	{
		return ACL_RET_IPV6_ERR;
	}
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"sipv6 %s \n",vosIpv6IntToStr(acl_rules.condition.con.exact.sip.addr.ipv6));
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"dipv6 %s \n",vosIpv6IntToStr(acl_rules.condition.con.exact.dip.addr.ipv6));

    /*外层vlan 作条件*/
	acl_condition_set_vlan(NULL,inputparm->L2Vlan,&acl_rules.condition);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"in-vlan=%x out-vlan=%x \n",
		acl_rules.condition.con.exact.cvlan[0].vlan,acl_rules.condition.con.exact.svlan[0].vlan);
    /*内层vlan 作条件*/
	acl_condition_set_vlan(inputparm->L2Vlaninner,NULL,&acl_rules.condition);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"in-vlan=%x out-vlan=%x \n",
		acl_rules.condition.con.exact.cvlan[0].vlan,acl_rules.condition.con.exact.svlan[0].vlan);

	acl_rules.acl_id=ptemplate->id;
	acl_rules.condition.flag |= ACL_CONDITION_TYPE_EXACT_VALUE;

	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"test eth_type=%x con-mask=%d\r\n",
		acl_rules.condition.con.exact.eth_type,acl_rules.condition.con.exact.mask);
	if(acl_rules.condition.con.exact.mask)
	{
		ret=acl_add_conditon_to_template(ptemplate,&acl_rules);
	}
	if (OK != ret)
	{
		return ACL_RET_CONFIG_ERR;
	}
	return ret;
}
int web_add_action(web_acl_rule_t *rule_param,acl_msg_rule_s *acl_rules)
{
	/*action*/
    int ret1;
	int ret = RET_SUCCESS;

	acl_action_set_baseact(rule_param->action,&acl_rules->action);
	if(rule_param->AddVlanOterFlag!=NULL && (vosStrCmp(rule_param->AddVlanOterFlag,"on")==0))
	{
		acl_action_set_vlan("insert-vid","out",rule_param->AddVlanOter,&acl_rules->action);
	}
	else
	{
		acl_action_set_vlan("repalce-vid","out",rule_param->vlan_outer,&acl_rules->action);
	}
	acl_action_replace_mac(rule_param->replacemac,&acl_rules->action);
	ret1 = acl_action_mirror(rule_param->portmirr,rule_param->action, &acl_rules->action);
	if (RET_SUCCESS != ret1)
	{
		ret |= ACL_RET_ACTION_MIRROR_ERR;
	}
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"acl_action->mirror_port=%d,mask=%d\r\n",acl_rules->action.mirror_port,acl_rules->action.mask );
	acl_action_redirect_port(rule_param->trunkid,rule_param->action,&acl_rules->action);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"acl_action->redict.trunkid=%d,mask=%d\r\n",acl_rules->action.redict.trunkid,acl_rules->action.mask );
	ret1 = acl_action_redirect_portlist(rule_param->Export,rule_param->action, &acl_rules->action);
	if (RET_SUCCESS != ret1)
	{
		ret |= ACL_RET_ACTION_REDICT_ERR;

	}
	ret1 = acl_action_egress_portlist(rule_param->Export,rule_param->action,&acl_rules->action);
	if (RET_SUCCESS != ret1)
	{
		ret |= ACL_RET_ACTION_EGR_PORT_ERR;
	}
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG," action-mask=%d \r\n",acl_rules->action.mask);
	return ret;
}

static void web_add_rule(Webs *wp, char *path, char *query)
{
	int i,j,port,port_num=0,ret=0,aclid=0;
	char *data=NULL;
	acl_template_s ptemplate;
	acl_msg_rule_s acl_rules;
	int ret1=OK,ret2=OK,ret3=OK;
	web_acl_rule_t rule_param;
	WEB_ACL_RET_TYPE ret_proc[] = {
		{ACL_RET_ID_OUT_OF_RANGE,  ACL_OUT_OF_RANGE_STR},
		{ACL_RET_ID_IS_EXIST,      ACL_IS_EXIST_STR},
		{ACL_RET_ID_ERR,           ACL_IS_ERR_STR},
		{ACL_RET_SRC_PORT_ERR,     ACL_SRC_PORT_STR},
		{ACL_RET_SRC_MAC_ERR,      ACL_SRC_MAC_STR},
		{ACL_RET_DST_MAC_ERR,      ACL_DST_MAC_STR},
		{ACL_RET_SRC_IP_ERR,       ACL_SRC_IP_STR},
		{ACL_RET_DST_IP_ERR,       ACL_DST_IP_STR},
		{ACL_RET_SRC_L4PORT_ERR,   ACL_SRC_L4PORT_STR},
		{ACL_RET_DST_L4PORT_ERR,   ACL_DST_L4PORT_STR},
		{ACL_RET_SVLAN_ERR,        ACL_SVLAN_STR},
		{ACL_RET_CVLAN_ERR,        ACL_CVLAN_STR},
		{ACL_RET_ETH_TYPE_ERR,     ACL_ETH_TYPE_STR},
		{ACL_RET_L3_PROCTOC_TYPE_ERR, ACL_L3_PROTOC_STR},
		{ACL_RET_TCP_FLAG_ERR,       ACL_TCP_FLAG_STR},
		{ACL_RET_UDF_ERR,            ACL_UDF_STR},
		{ACL_RET_ACTION_MIRROR_ERR,  ACL_MIRROR_STR},
		{ACL_RET_ACTION_REDICT_ERR,  ACL_REDICT_STR},
		{ACL_RET_ACTION_EGR_PORT_ERR,  ACL_EGRESS_PORT_STR},
		{ACL_RET_CONFIG_ERR,           ACL_CONFIG_STR},
		{ACL_RET_IPV6_ERR,             ACL_IPV6_STR},

	};

	memset(&rule_param,0,sizeof(web_acl_rule_t));
	ret=web_get_rule_param(wp,&rule_param);
	if(ret!=OK ||(rule_param.acl_id ==NULL) || strlen(rule_param.acl_id)<1 || strlen(rule_param.acl_id)>5)
	{
		printf("set rule parameter error!!\r\n");
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, "error");
		websDone(wp);
		return;
	}

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	data=vosAlloc(1000);
	if(data==NULL)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl add rule, malloc fail.\r\n", wp->username);
		printf("memery malloc error!!\r\n");
		websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"配置失败!\")\r\n</script>");
		websDone(wp);
	}
	memset(data,0,1000);
	memset(&ptemplate,0,sizeof(ptemplate));
	memset(&acl_rules,0,sizeof(acl_msg_rule_s));

	ptemplate.id=atoi(rule_param.acl_id);
	if(ptemplate.id<=0 ||ptemplate.id >4096)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl add rule fail, acl %d out of range.\r\n", wp->username,ptemplate.id);
		ret |= ACL_RET_ID_OUT_OF_RANGE;
		goto out;
	}
	if(acllist[ptemplate.id]==1)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl add rule fail, acl %d is exist.\r\n", wp->username,ptemplate.id);
		ret |= ACL_RET_ID_IS_EXIST;
		goto out;
	}

	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG," ptemplate.id=%d \r\n",ptemplate.id);
	ret1=acl_set_template(&ptemplate);
	if(ret1 != OK)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl %d create rule fail.\r", wp->username,ptemplate.id);
		ret |= ACL_RET_CONFIG_ERR;
		goto out;
	}

	ret1 = web_add_condition(&rule_param,&ptemplate);
	if(OK != ret1)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl %d add conditon fail.\r\n", wp->username,ptemplate.id);
		ret |= ret1;
		goto out;
	}

    /*UDF*/
	ret2=acl_condition_set_udf2(rule_param.UDFList,ptemplate.id);
	if(ret2!=OK)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl %d add udf conditon fail.\r\n", wp->username,ptemplate.id);
        ret |= ACL_RET_UDF_ERR;
		goto out;
	}

	/*action */
  	memset(&acl_rules,0,sizeof(acl_msg_rule_s));
	acl_rules.acl_id=ptemplate.id;
    ret3 = web_add_action(&rule_param, &acl_rules);
	if (ret3 != OK)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl %d add action fail.\r\n", wp->username,ptemplate.id);
		ret |= ret3;
		goto out;
	}

	ret3 = acl_add_action_to_template(&ptemplate,&acl_rules);
    if(ret3 != OK)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl %d add action fail.\r\n", wp->username,ptemplate.id);
		ret |= ACL_RET_CONFIG_ERR;
		goto out;
	}
	ret3=acl_install_template(&ptemplate);
	 if(ret3 != OK)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl %d commit fail.\r\n", wp->username,ptemplate.id);
		ret |= ACL_RET_CONFIG_ERR;
	}
	else
	{
		acllist[ptemplate.id]=1;
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] acl %d create successful.\r\n", wp->username,ptemplate.id);
		websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"配置成功!\")\r\n</script>");
		goto success;
	}

out:
	for (i = 0; i < (sizeof(ret_proc)/sizeof(WEB_ACL_RET_TYPE)); i++)
	{
		if (ret & ret_proc[i].type)
		{
			websWrite(wp, ret_proc[i].string);
		}
	}
	acl_del_template(&ptemplate);

success:

	free(data);
	data=NULL;
	websDone(wp);
	return;
}
static void web_save_cfg(Webs *wp, char *path, char *query)
{
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	sys_save_config();//ruleCompress(RULE_DB_PATH,RULE_DB_SAVE_PATH);
	//websWrite(wp, "ok");
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] save config successful.\r\n", wp->username);
	websWrite(wp, WEB_RET_CONFIG_OK_AND_REBOOT);
	websDone(wp);
}

static void web_restore_cfg(Webs *wp, char *path, char *query)
{
	int ret=OK;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	ret=sys_config_restore();
	//websWrite(wp, "ok");
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] restore config successful.\r\n", wp->username);
	if(ret==OK)
	{
		websWrite(wp, WEB_RET_RESTORE_OK);
	}
	else
	{
		websWrite(wp, WEB_RET_RESTORE_FAIL);
	}
	websDone(wp);
}

static void web_sys_reboot(Webs *wp, char *path, char *query)
{
	WebsKey         *s;
	WebsUpload  *up;
	char            *upfile;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] reboot successful.\r\n", wp->username);
	websWrite(wp, WEB_RET_SYS_REBOOT);
	websDone(wp);

	vosReboot();
}
#endif

static int showSysTime(int eid, Webs *wp, int argc, char **argv)
{
	char data[48];
#ifndef __WEB_DEBUG__
	int year, month, day, hour, minute, second;
#endif

#ifdef __WEB_DEBUG__
    time_t tm_val;
    struct tm *val;

    tm_val = time(NULL);
    val = localtime(&tm_val);
    strftime(data, 48,  "%F %T", val);
#else
/*	sprintf(data,"%d/%d/%d  %d:%.2d:%.2d",
		(2000+time.tm_year),time.tm_mon,time.tm_mday,time.tm_hour,time.tm_min,time.tm_sec);*/
	snprintf(data,50,"%04d-%02d-%02d %d:%.2d:%.2d",g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_year+2000,g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_mon,
	    g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_mday,g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_hour,g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_min,
	    g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_sec);
#endif

	websWrite(wp, data);
    return 0;
}

static int showSoftVer(int eid, Webs *wp, int argc, char **argv)
{
#ifdef __WEB_DEBUG__
	websWrite(wp, "0.1.1");
#else
	websWrite(wp, sys_get_sw_version());
#endif
	return 0;
}

#if 0
static void web_set_time(Webs *wp, char *path, char *query)
{
	char	*settime;
	char    *ptr;
	hdl_tm_t time;
	int length,ret=0;
	int year,month,day,hour,minute,second;
	char IntStr[5];

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "web_SetTime (%s %d)\r\n",__FUNCTION__,__LINE__);

	settime = websGetVar(wp, "setTime", NULL);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "input:%s len=%d\r\n",settime,strlen(settime));
	if(settime==NULL || strlen(settime)<10 || strlen(settime)>20 )
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] system set time fail.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	if(vosMemCmp((settime+4),"-",1)!=0 || vosMemCmp((settime+7),"-",1)!=0)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

 	ptr = strchr(settime,'-');
	length =(int)(ptr-settime);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "length:%d\r\n",length);
	if( length>0 && length<=5)
	{
		memcpy(IntStr , settime ,length);
		IntStr[length]='\0';
		time.tm_year= atoi(IntStr)-2000;
		settime = ptr+1;

	 	ptr = strchr(settime,'-');
		length =(int)(ptr-settime);
		memcpy(IntStr , settime ,length);
		IntStr[length]='\0';
		time.tm_mon= atoi(IntStr);
		settime = ptr+1;

	 	ptr = strchr(settime,' ');
		length =(int)(ptr-settime);
			memcpy(IntStr , settime ,length);
		IntStr[length]='\0';
		time.tm_mday= atoi(IntStr);
		settime = ptr+1;

	 	ptr = strchr(settime,':');
		length =(int)(ptr-settime);
			memcpy(IntStr , settime ,length);
		IntStr[length]='\0';
		time.tm_hour= atoi(IntStr);
		settime = ptr+1;

	 	ptr = strchr(settime,':');
		length =(int)(ptr-settime);
			memcpy(IntStr , settime ,length);
		IntStr[length]='\0';
		time.tm_min= atoi(IntStr);
		settime = ptr+1;

		time.tm_sec= atoi(settime);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] system set time successful.\r\n", wp->username);
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "%d/%d/%d, %d:%.2d:%.2d\n",year,month,day,hour,minute,second);
	    /*48GE TAP*/
		//ret=adp_system_time_set(&time);
		memset(&g_sys_poll_info.sys_rtc_time.rtc_time_set, 0, sizeof(hdl_tm_t));
		memcpy(&g_sys_poll_info.sys_rtc_time.rtc_time_set, &time, sizeof(hdl_tm_t));
		adp_sys_info_set(SYS_RTC_TYPE, &g_sys_poll_info);
	}
	if(ret==OK)
	{
		websWrite(wp, WEB_RET_CONFIG_OK);
	}
	else
	{
		websWrite(wp, WEB_RET_CONFIG_FAIL);
	}
	websDone(wp);
}

web_get_cpu_mem()
{
	int useage=0;
	cpu_occupy_t occupy_first,occupy_sec;
	mem_occupy_t mem_info;

	memset(&occupy_first,0,sizeof(cpu_occupy_t));
	memset(&occupy_sec,0,sizeof(cpu_occupy_t));
	memset(&mem_info,0,sizeof(mem_occupy_t));

	sys_get_cpuoccupy(&occupy_first);
	usleep(800000);
	sys_get_cpuoccupy(&occupy_sec);
	useage=sys_cal_cpuoccupy(&occupy_first,&occupy_sec);

	sys_get_memoccupy(&mem_info);
}

extern int cpu_usage;
extern sys_gloable_t g_system_gloable_info;
static void web_get_sys_status(Webs *wp, char *path, char *query)
{
	//char data[100],tem_str[10]={0};
	char tem_str[10]={0};
	int tempture=0;
	//int fan_level=5,cpu_used=30,mem_per=0;
	int cpu_used=30,mem_per=0;
	unsigned long mem_used=0;
	cpu_occupy_t occupy_first,occupy_sec;
	mem_occupy_t mem_info;
    int fan_level[4] = {0};
    int i = 0;
    cJSON *data_json = NULL;
    char *data = NULL;

	memset(&occupy_first,0,sizeof(cpu_occupy_t));
	memset(&occupy_sec,0,sizeof(cpu_occupy_t));
	memset(&mem_info,0,sizeof(mem_occupy_t));

	/*获取CPU利用率*/
#if 0
	sys_get_cpuoccupy(&occupy_first);
	usleep(800000);
	sys_get_cpuoccupy(&occupy_sec);
	cpu_used=sys_cal_cpuoccupy(&occupy_first,&occupy_sec);
#else
	cpu_used=g_system_gloable_info.cpu_usage;
#endif
	/*获取MEMERY 利用率*/
	sys_get_memoccupy(&mem_info);
	mem_used=mem_info.total-mem_info.free;
	mem_per=((mem_used*1000)/mem_info.total);

//	printf("[%s %d] totl=%ld free=%ld mem_used=%ld mem_per=%d\r\n",__FUNCTION__,__LINE__,mem_info.total,mem_info.free,mem_used,mem_per);
	/*获取设备平均温度*/
	tempture = sys_get_temperature();
//	printf("tempture:%d\r\n", tempture);
    vosSprintf(tem_str, "%.2f",(float)tempture/100.0);

	/*获取风扇转速等级*/
	//fan_level=adp_fan_level_get();
	for(i = 0; i < FAN_MAX_NUM; i++)
	    fan_level[i] = g_sys_poll_info.sys_fan_module.fan_level_get[i];

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

    // 组成json结构
    data_json = cJSON_CreateObject();
    if(data_json == NULL)
    {
        websWrite(wp,WEB_RET_CONFIG_FAIL);
    	websDone(wp);
        return;
    }
    cJSON_AddStringToObject(data_json, "temp", tem_str);
    //cJSON_AddNumberToObject(data_json, "fan", fan_level);
    cJSON_AddNumberToObject(data_json, "fan1", fan_level[0]);
    cJSON_AddNumberToObject(data_json, "fan2", fan_level[1]);
    cJSON_AddNumberToObject(data_json, "fan3", fan_level[2]);
    cJSON_AddNumberToObject(data_json, "fan4", fan_level[3]);
    cJSON_AddNumberToObject(data_json, "ramRate", (float)mem_per/10.0);
    cJSON_AddNumberToObject(data_json, "cpuRate", cpu_used);

    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
    	websWrite(wp,WEB_RET_CONFIG_FAIL);
    	websDone(wp);
        return;
    }
    cJSON_Delete(data_json);

	//sprintf(data,"%s-%d-%d.%d-%d",tem_str,fan_level,(mem_per/10),(mem_per%10),cpu_used);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "data_send:\n%s\n",data);
	websWrite(wp, data);
	websDone(wp);
    free(data);

}

static void web_set_ntp(Webs *wp, char *path, char *query)
{
	char	*ip, *ntp;
	char ifname[10]={0};
	int ret=OK,ntp_flag=0;

	ip = websGetVar(wp, "NtpdIp", NULL);
	ntp = websGetVar(wp, "NtpdValue", NULL);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "ip:%s,ntp:%s\n",ip,ntp);
	if(ntp==NULL || strlen(ntp)<1 || strlen(ntp)>4 ||(!vosValidIpCheck(ip)))
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return ;
	}

	ntp_flag=atoi(ntp);
	if(ntp_flag)
	{
		if(ip!=NULL && strlen(ip)>0 && strlen(ip) < 20)
		{
			server_ip = inet_addr(ip);
			ret=ntp_time_get();
		}
	}
	else
	{
		g_ntp_mode = 0;
        server_ip = inet_addr("1.2.3.4");
        ret=ntp_server_set();
	}
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	if(ret==OK)
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] system set syn ntp successful.\r\n", wp->username);
		if(ntp_flag)
		{
			g_ntp_mode = 1;
			ntpsetok=1;
			websWrite(wp,WEB_RET_NTP_OPEN);
		}
		else
		{
			g_ntp_mode = 0;
			ntpsetok=0;
			websWrite(wp,WEB_RET_NTP_CLOSE);
		}
	}
	else
	{
		g_ntp_mode = 0;
		ntpsetok=0;
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] system set syn ntp fail.\r\n", wp->username);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
	}
	websDone(wp);
}

static void web_get_ntp(Webs *wp, char *path, char *query)
{
	//char data[100]={100};
	int len=0;
    struct in_addr srv_addr;

    char *data = NULL;
    char systime_str[50] = {0};
    cJSON *data_json = NULL;
    cJSON *rule_json = NULL;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }

    snprintf(systime_str,50,"%04d-%02d-%02d %d:%.2d:%.2d",g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_year+2000,g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_mon,
        g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_mday,g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_hour,g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_min,
        g_sys_poll_info.sys_rtc_time.rtc_time_get.tm_sec);

    cJSON_AddBoolToObject(data_json, "ntpdswitch", ntpsetok);
    cJSON_AddStringToObject(data_json, "systime", systime_str);
    srv_addr.s_addr = server_ip;
    cJSON_AddStringToObject(data_json, "serverip", inet_ntoa(srv_addr));

    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);

    DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "data_send:%s\n",data);
	websWrite(wp, data);
	websDone(wp);
    free(data);
}
extern int g_lang_en;

static void web_language_change(Webs *wp, char *path, char *query)
{
	char	*lang;
	int ret=OK,lang_flag=0;

	lang = websGetVar(wp, "language", NULL);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "lang:%s\n",lang);
	if(lang==NULL || strlen(lang)<1 || strlen(lang)>2 || !web_param_valid_check(lang))
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return ;
	}
	lang_flag=atoi(lang);
	if(lang_flag==1)
	{
		g_lang_en=1;
	}
	else
	{
		g_lang_en=0;
	}
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	websWrite(wp,WEB_RET_CONFIG_OK);
	websDone(wp);

}

//extern hdl_optical_data g_optical_info;

static void web_get_optical_mode_status(Webs *wp, char *path, char *query)
{
	int ret=ERROR,len=0,port=0,optical=0;
	char data[100]={0};
	char	*key,*value;
	int optical_tx,optical_rx;
	hdl_optical_data_t optical_info;
	uint64 temp;

	key = stok(wp->input.buf, ":", &value);
	if(key==NULL || strlen(key)<1)
	{
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

	if (strcmp(key, "portnum")==0)
	{
		port = atoi(value);
	}
	//adp_ioagent_optical_poll(&optical_info);
	memcpy(&optical_info,&g_sys_poll_info.sys_optical_info,sizeof(hdl_optical_data_t));
	temp = (uint64)optical_info.sfpABS[0] | ((uint64)optical_info.sfpABS[1]<<16) | ((uint64)optical_info.sfpABS[2]<<32);

	//printf("!!! 1111 szp test port=%d sfp tx=%d\n",port,optical_info.sfpTxPower[port-1]);
	//printf("!!! szp test \ntemp=%#llx\n",temp);
	if(port>BASE_10G_PORT_NUM)
	{
		if(!((optical_info.cfpABS>>(port-BASE_10G_PORT_NUM-1))&0x1))
		{
			len+=vosSprintf(data+len,"CFP4");
			len+=vosSprintf(data+len,";");
			len+=vosSprintf(data+len,"Tx:NA(dbm),Rx:NA(dbm)");
		}
		else
		{
			len+=vosSprintf(data+len,"CFP4");
			len+=vosSprintf(data+len,";");
			optical_tx=optical_info.cfpTxPower[port-BASE_10G_PORT_NUM-1];
			optical_rx=optical_info.cfpRxPower[port-BASE_10G_PORT_NUM-1];
			len+=vosSprintf(data+len,"Tx:%.2f(dbm),Rx:%.2f(dbm)",(float)optical_tx/100.0, (float)optical_rx/100.0);
		}
	}
	else if(port>0)
	{
		if((temp>>(port-1))&0x1)
		{
			len+=vosSprintf(data+len,"SFP+");
			len+=vosSprintf(data+len,";");
			len+=vosSprintf(data+len,"Tx:NA(dbm),Rx:NA(dbm)");
		}
		else
		{
			len+=vosSprintf(data+len,"SFP+");
			len+=vosSprintf(data+len,";");
			optical_tx=optical_info.sfpTxPower[port-1];
			optical_rx=optical_info.sfpRxPower[port-1];
			//len+=vosSprintf(data+len,"Tx:%d(dB),Rx:%d(dB)",optical_tx,optical_rx);
			len+=vosSprintf(data+len,"Tx:%.2f(dbm),Rx:%.2f(dbm)",(float)optical_tx/100.0, (float)optical_rx/100.0);
		}
	}
	//printf("!!!! 2222 szp test\n");
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "data_send:%s\n",data);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	websWrite(wp,"%s",data);
	websDone(wp);
}

static void web_get_running_config(Webs *wp, char *path, char *query)
{

	char *file_data=NULL;
	int ret,max_len=10000;
	unsigned char *config_file=RUNNING_CONFIG_FILE;

	file_data=vosAlloc(max_len);
	if(file_data==NULL)
	{
		printf("Memery malloc error !! \r\n");
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	memset(file_data,0,max_len);
	sys_get_runing_config();
	ret=get_file_to_buffer(config_file,file_data,max_len);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "ret=%d  file_data=%s\r\n",ret,file_data);
	if(ret==SUCCESS)
	{
		websWrite(wp, file_data);
	}
	free(file_data);
	file_data=NULL;
	websDone(wp);
}

static void web_get_log(Webs *wp, char *path, char *query)
{
    int lognum = 0, i = 0;
    char filename_str[30][256] = {0};
    char  *data = NULL;
    cJSON *data_json = NULL;
    cJSON *log_json = NULL;

    DIR *dir = NULL;
    struct dirent *ptr = NULL;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	data_json = cJSON_CreateArray();
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }

    dir = opendir(LOG_DIR_PATH);
    if(dir)
    {
        while((ptr = readdir(dir)) != NULL)
        {
           // printf("[%s %d] ptr->d_name:%s \r\n",__FUNCTION__,__LINE__,ptr->d_name);
            if(vosStrCmp(ptr->d_name, ".") == 0 || vosStrCmp(ptr->d_name, "..") == 0 || strlen(ptr->d_name) > 13)
                continue;
            else
            {
                strcpy(filename_str[lognum], ptr->d_name);
                lognum++;
		   if(lognum>=30)
		   {
			break;
		   }
            }
        }
        closedir(dir);
    }

    for(i = 0; i < lognum; i++)
    {
        cJSON_AddItemToArray(data_json, log_json=cJSON_CreateObject());
        cJSON_AddStringToObject(log_json, "filename", filename_str[i]);
    }
    /* 3. json数据转换发送 */
    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
    	websWrite(wp, WEB_RET_CONFIG_FAIL);
    	websDone(wp);
        return;
    }
    cJSON_Delete(data_json);

    DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "log file data=%s\r\n", data);
    websWrite(wp, data);
    websDone(wp);
    free(data);

}

static void web_load_logs(Webs *wp, char *path, char *query)
{
    uint   lognum = 0, i = 0;
    char  filename[LOG_FILE_MAX_NUM][LOG_NAME_MAX_LEN] = {0};
    char  file_dir[150] = {0};
    char  filename_str[100] = {0};
    char  cmd_buf[200] = {0};
    char  *file_data = NULL;
    int   file_len = 0, len = 0;
    struct stat  s;
    char  log_url[80] = {0};
    char  *ipaddr = NULL;
    char  rcv[1500] = {0};
    cJSON *rcv_json = NULL;
    cJSON *log_json = NULL;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);

    strcpy(rcv, websGetVar(wp, "logfilename", ""));
    DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "load_logs: %s\n",rcv);
    if(rcv == NULL || !strlen(rcv))
    {
        websWriteEndHeaders(wp);
        websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }

    rcv_json = cJSON_Parse(rcv);
	if(!rcv_json)
    {
        DEBUG_TRACE(WEB_MODULE, LEVEL_ERROR, "Error before: [%s]\n",cJSON_GetErrorPtr());
        websWriteEndHeaders(wp);
        websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }

    lognum = cJSON_GetArraySize(rcv_json);
    //DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "load log num=%d\r\n", lognum);
    if(lognum == 0)
    {
        cJSON_Delete(rcv_json);
        websWriteEndHeaders(wp);
        websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }

    for(i = 0; i < lognum; i++)
    {
        log_json = cJSON_GetArrayItem(rcv_json, i);
        if(log_json == NULL)
            continue;
        vosSnprintf(filename[i], 20, cJSON_GetObjectItem(log_json, "filename")->valuestring);
        //DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "filename[%d]=%s\r\n",i, filename[i]);
    }
    cJSON_Delete(rcv_json);

    /* 1. 当文件个数为1个时，不用压缩*/
    if(lognum == 1)
    {
        vosSnprintf(file_dir, sizeof(file_dir), "%s/%s", LOG_DIR_PATH, filename[0]);
        (void)stat(file_dir, &s);
        file_len = (int)s.st_size;
        file_data = vosAlloc(file_len);
	    (void)get_file_to_buffer(file_dir, file_data, file_len+1);
        DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "load log file=%s, file_len=%d\r\n", filename[0], file_len);
        websWriteHeader(wp, "Content-Type", "%s", "application/octet-stream");
        websWriteHeader(wp, "Content-Disposition", "%s%s", "attachment; filename=",filename[0]);
        websWriteEndHeaders(wp);
        websWrite(wp, file_data);
        //websWrite(wp, WEB_STR_OK);
        websDone(wp);
        free(file_data);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] load log file=%s.\r\n", wp->username, filename[0]);
    }
    /* 2. 当日志文件大于1时，将日志文件压缩到一个文件中 */
    else
    {
        ipaddr = websGetHost(wp) ? websGetHost(wp) : websGetIpaddr(wp);
        if(ipaddr == NULL)
        {
            websWriteEndHeaders(wp);
            websWrite(wp,WEB_RET_CONFIG_FAIL);
    		websDone(wp);
    		return;
        }

        for(i = 0; i < lognum; i++)
        {
            vosStrCat(filename[i], " ");
            vosStrCat(filename_str, filename[i]);
        }

        vosSnprintf(cmd_buf, sizeof(cmd_buf), "rm -rf %s", LOG_DOWNLOAD_GZ_FILE_PATH);
        system(cmd_buf);
        vosSnprintf(cmd_buf, sizeof(cmd_buf), "cd %s; tar -zcf %s %s", LOG_DIR_PATH, LOG_DOWNLOAD_GZ_FILE_PATH, filename_str);
        system(cmd_buf);

        /*
        压缩生成log.tar.gz文件后，存放到web文件目录下，前端发起文件请求即可下载
        多个文件打包或压缩后，不能通过读取文件到buffer后websWrite方式，会造成文件缺失格式错误
        */
        (void)vosSnprintf(log_url, sizeof(log_url), "%s//%s/%s",
            ME_COM_SSL ? "https:" : "http:", ipaddr, LOG_DOWNLOAD_GZ_FILE_NAME);
        DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "log_url=%s\n", log_url);
        websWriteEndHeaders(wp);
        websWrite(wp, "<script type=\"text/javascript\">\r\nlocation.href = \"%s\";\r\n</script>", log_url);
        websDone(wp);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] load log file=%s.\r\n", wp->username, LOG_DOWNLOAD_GZ_FILE_NAME);
    }

}

static void web_search_log(Webs *wp, char *path, char *query)
{
	char  *startDate = NULL,*endDate   = NULL,*keyword   = NULL;
	char  linefeed[3]="\n";
	//char  tempbuf[1000]={0};
	uchar* output = NULL;
	char* web_data = NULL;
	uint   len = 0;
	char buffer[128]= {0};

	int i = 0;
	char file_name[32]={0};
	char *p=NULL;
	hdl_tm_t tm_start;
	hdl_tm_t tm_end;
	int year,month,day;
	int start_condition = FALSE;
	int end_condition = FALSE;
	int syear,smon,sday,eyear,emon,eday;

	startDate = websGetVar(wp, "startDate", "");
	endDate = websGetVar(wp, "endDate", "");
	keyword = websGetVar(wp, "keyword", "");

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "search log startDate=%s, endDate=%s, keyword=%s\n",startDate, endDate,keyword);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	/* 1.仅按关键字筛选 */
	//if(keyword!="" && startDate=="" && endDate=="")
	if(vosStrCmp(keyword, "")!=0 && vosStrCmp(startDate, "")==0 && vosStrCmp(endDate, "")==0)
	{
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "search log keyword=%s\n",keyword);
		(void)vosSnprintf(buffer,128,"cd %s; find %s -type f -name \"*.log\" | xargs grep \"%s\" > output.txt",
		    LOG_DIR_PATH, LOG_DIR_PATH, keyword);
		system(buffer);
		(void)vosSnprintf(buffer,128,"%s/output.txt", LOG_DIR_PATH);
		len = get_file_size(buffer);
		len = len? len:1000;
		output = vosAlloc(len);
		memset(output, ZERO, sizeof(output));
		if (NULL == output)
		{
		    printf("output malloc error\r\n");
		    websWrite(wp,WEB_RET_CONFIG_FAIL);
			websDone(wp);
			return;
		}
		get_file_to_buffer(buffer, output, &len);//file_read(buffer, output, &len);
		websWrite(wp, output);
		websDone(wp);
		free(output);
		output = NULL;
		(void)vosSnprintf(buffer,128,"rm -rf %s/output.txt", LOG_DIR_PATH);
		system(buffer);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] search log keyword=%s.\r\n", wp->username, keyword);
		return;
	}
	/* 2.仅按日期筛选 */
	//else if(keyword=="" && startDate!="" && endDate!="")
	else if(vosStrCmp(keyword, "")==0 && vosStrCmp(startDate, "")!=0 && vosStrCmp(endDate, "")!=0)
	{
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "search log startDate=%s, endDate=%s\n",startDate, endDate);
		sscanf(startDate,"%d-%d-%d ",&syear,&smon,&sday);
		sscanf(endDate,"%d-%d-%d ",&eyear,&emon,&eday);
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "syear %d,smon %d,sday %d,eyear %d,emon %d,eday %d\n",syear,smon,sday,eyear,emon,eday);

		output = vosAlloc(2*1024*1024);
		memset(output, ZERO, sizeof(output));
		if (NULL == output)
		{
		    printf("output malloc error\r\n");
		    websWrite(wp,WEB_RET_CONFIG_FAIL);
			websDone(wp);
			return;
		}
		web_data = vosAlloc(10*1024*1024);
		memset(web_data, ZERO, sizeof(web_data));
		if (NULL == web_data)
		{
		    printf("web_data malloc error\r\n");
		    free(output);
		    output = NULL;
			websWrite(wp,WEB_RET_CONFIG_FAIL);
			websDone(wp);
			return;
		}

		for (i = 0; i <= 10; i++)
		{
		    memset(output, 0, 2*1024*1024);
			len = 0;
			memset(file_name, 0, sizeof(file_name));
			if (i == 10)
			{
				(void)vosSnprintf(file_name,32,"%s/user.log",LOG_DIR_PATH);
			}
			else
			{
				(void)vosSnprintf(file_name,32,"%s/user.%d.log",LOG_DIR_PATH,(10-i-1));
			}

			DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "file name:%s\r\n", file_name);
			if(file_read(file_name, output, &len) != RET_SUCCESS)
			{
				continue;
			}

			p=strtok(output, linefeed);
			while(p)
			{
		        start_condition = FALSE;
		        end_condition = FALSE;
		        /*处理一行的字符串*/
				sscanf(p, "%d-%d-%d", &year, &month, &day);
		            DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,
						"year %d,month %d,day %d,tm_start.tm_year %d,tm_start.tm_mon %d,tm_start.tm_mday %d\r\n",
						year,month,day,syear,smon,sday);
				if ((year > syear)
					||((year == syear) && (month > smon))
					|| ((year == syear) && (month == smon)&& (day >= sday)))
				{
				    start_condition = TRUE;
				}
		            DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,
						"year %d,month %d,day %d,tm_end.tm_year %d,tm_end.tm_mon %d,tm_end.tm_mday %d\r\n",
						year,month,day,eyear,emon,eday);
				if ((year < eyear)
					||((year == eyear) && (month < emon))
					|| ((year ==eyear) && (month == emon) && (day <= eday)))
				{
				    end_condition = TRUE;
				}
		            DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "start_condition :%d,end_condition:%d\r\n", start_condition,end_condition);

				if (end_condition && start_condition)
				{
					//websWrite(wp, "%s\r\n", p);
					DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "p:%s\r\n", p);
					//(void)vosSnprintf(tempbuf, sizeof(tempbuf), "%s\r\n", p);
					vosStrCat(web_data, p);
				}
				p = strtok(NULL, linefeed);
			}
		}
		websWrite(wp, "%s\r\n", web_data);
		websDone(wp);
		free(output);
		output = NULL;
		free(web_data);
		web_data = NULL;
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] search log date=%s - %s.\r\n", wp->username, startDate, endDate);
		return;
	}
	/* 3.先按关键字后按日期筛选 */
	//else if(keyword!="" && startDate!="" && endDate!="")
	else if(vosStrCmp(keyword, "")!=0 && vosStrCmp(startDate, "")!=0 && vosStrCmp(endDate, "")!=0)
	{
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "search log startDate=%s, endDate=%s, keyword=%s\n",startDate, endDate,keyword);
		sscanf(startDate,"%d-%d-%d",&syear,&smon,&sday);
		sscanf(endDate,"%d-%d-%d",&eyear,&emon,&eday);

		(void)vosSnprintf(buffer,128,"cd %s; find %s -type f -name \"*.log\" | xargs grep \"%s\" > output.txt",
		    LOG_DIR_PATH, LOG_DIR_PATH, keyword);
		system(buffer);
		(void)vosSnprintf(buffer,128,"%s/output.txt", LOG_DIR_PATH);
		len = get_file_size(buffer);
		len = len? len:1000;
		output = vosAlloc(len);
		memset(output, ZERO, sizeof(output));
		if (NULL == output)
		{
		    printf("output malloc error\r\n");
		    websWrite(wp,WEB_RET_CONFIG_FAIL);
			websDone(wp);
			return;
		}
		web_data = vosAlloc(10*1024*1024);
		memset(web_data, ZERO, sizeof(web_data));
		if (NULL == web_data)
		{
		    printf("web_data malloc error\r\n");
		    free(output);
		    output = NULL;
		    websWrite(wp,WEB_RET_CONFIG_FAIL);
			websDone(wp);
			return;
		}
		file_read(buffer, output, &len);

		p=strtok(output, linefeed);
		while(p)
		{
		    start_condition = FALSE;
		    end_condition = FALSE;
		    /*处理一行的字符串*/
		    if((p = strstr(p, ":")) != NULL)
		        p++;
		    else
		        break;
			sscanf(p, "%d-%d-%d", &year, &month, &day);
	            DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,
					"year %d,month %d,day %d,tm_start.tm_year %d,tm_start.tm_mon %d,tm_start.tm_mday %d\r\n",
					year,month,day,syear,smon,sday);
			if ((year > syear)
				||((year == syear) && (month > smon))
				|| ((year == syear) && (month == smon)&& (day >= sday)))
			{
			    start_condition = TRUE;
				//printf("wwwwww start match\r\n");
			}
	            DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,
					"year %d,month %d,day %d,tm_end.tm_year %d,tm_end.tm_mon %d,tm_end.tm_mday %d\r\n",
					year,month,day,eyear,emon,eday);
			if ((year < eyear)
				||((year == eyear) && (month < emon))
				|| ((year == eyear) && (month == emon) && (day <= eday)))
			{
			    end_condition = TRUE;

			}

			if (end_condition && start_condition)
			{
				DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "p:%s\r\n", p);
		        //(void)vosSnprintf(tempbuf, sizeof(tempbuf), "%s\r\n", p);
		        vosStrCat(web_data, p);
			}
			p = strtok(NULL, linefeed);
		}
		websWrite(wp, "%s\r\n", web_data);
		websDone(wp);
		free(output);
		output = NULL;
		free(web_data);
		web_data = NULL;
		(void)vosSnprintf(buffer,128,"rm -rf %s/output.txt", LOG_DIR_PATH);
		system(buffer);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] search log date=%s - %s, keyword=%s.\r\n", wp->username, startDate, endDate, keyword);
		return;
	}
	/* 4.无效参数 */
	else
	{
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "search log para error\n");
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] search log para error.\r\n", wp->username);
		return;
	}
}
static void web_load_search_logs(Webs *wp, char *path, char *query)
{
    char  *rcv = NULL;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	rcv = vosAlloc(10*1024*1024);
	if(rcv == NULL)
	{
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    strcpy(rcv,websGetVar(wp, "strlog", ""));
    if(!strlen(rcv))
    {
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    websWriteHeader(wp, "Content-Type", "%s", "application/octet-stream");
    websWriteHeader(wp, "Content-Disposition", "%s%s", "attachment; filename=","result.txt");
    websWriteEndHeaders(wp);
    websWrite(wp, rcv);
    websDone(wp);
    free(rcv);
    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] load search log file result.txt.\r\n", wp->username);
}

static void web_set_port(Webs *wp, char *path, char *query)
{
    char*  tdCEnumlist = NULL;
    char*  selCEenable = NULL;
    char*  selCEsigl = NULL;
    char*  selCEcirl = NULL;
    char*  selCErate = NULL;
    char*  tdXEnumlist = NULL;
    char*  selXEenable = NULL;
    char*  selXEsigl = NULL;
    char*  selXEcirl = NULL;
    char   selXErate[20] = {0};
    uint8   ceport_list[PORT_TOTOL_MAX_NUM]={0};
    uint8   xeport_list[PORT_TOTOL_MAX_NUM]={0};
    uint8  ceport_num = 0, xeport_num = 0;
    int    i = 0,err_flag = 0;
    int    AdminState=0, SingleState=0, speed = 0;
    int    loopbackMode=PHY_LOOPBACK;
    //char*  operation_enable,operation_sigl,operation_cirl;
    char  operation_enable[25] = {0};
    char  operation_sigl[25] = {0};
    char  operation_cirl[25] = {0};

//return;
    tdCEnumlist = websGetVar(wp, "tdCEnumlist", "");
    selCEenable = websGetVar(wp, "selCEenable", "");
    selCEsigl   = websGetVar(wp, "selCEsigl", "");
    selCEcirl   = websGetVar(wp, "selCEcirl", "");
    //selCErate   = websGetVar(wp, "selCErate", "");

    tdXEnumlist = websGetVar(wp, "tdXEnumlist", "");
    selXEenable = websGetVar(wp, "selXEenable", "");
    selXEsigl   = websGetVar(wp, "selXEsigl", "");
    selXEcirl   = websGetVar(wp, "selXEcirl", "");
    //selXErate   = websGetVar(wp, "selXErate", "");
    strcpy(selXErate, websGetVar(wp, "selXErate", ""));

    DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "tdCEnumlist=%s,selCEenable=%s,selCEsigl=%s,selCEcirl=%s;tdXEnumlist=%s,selXEenable=%s,selXEsigl=%s,selXEcirl=%s;selXErate=%s\r\n",
        tdCEnumlist, selCEenable, selCEsigl, selCEcirl, tdXEnumlist, selXEenable, selXEsigl, selXEcirl, selXErate);
    /* 入口参数过滤，从tdCEnumlist中取出相应的100G未拆分端口号 */
    //port_list_get_port_by_key函数里已执行了check_port_is_invalid_state，port_list[i]中存储的是logicNum
    if(!strlen(selCEenable)||!strlen(selCEsigl)||!strlen(selCEcirl)
        ||!strlen(selXEenable)||!strlen(selXEsigl)||!strlen(selXEcirl)||!strlen(selXErate)
        ||(!strlen(tdCEnumlist) && !strlen(tdXEnumlist)))
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port config fail, input params error.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

    /* 40G端口设置 */
    if(strlen(tdCEnumlist) && !port_list_get_port_by_key(tdCEnumlist,ceport_list,&ceport_num))
    {
        AdminState   = atoi(selCEenable);
        SingleState  = atoi(selCEsigl);
        loopbackMode = atoi(selCEcirl);
        strcpy(operation_enable, (AdminState==1)?"undo shutdown":"shutdown");
        strcpy(operation_sigl, (SingleState==1)?"set single transmit":"undo single transmit");
        strcpy(operation_cirl, (loopbackMode==1)?"set loopback mode":"undo loopback mode");
        for(i = 0; i < ceport_num; i++)
        {
            if(AdminState == 0 || AdminState == 1)  // 0 1 才设置
            {
            	if(adp_port_mng_state_set(ceport_list[i], AdminState) != OK)
            	{
            		DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s fail.\r\n",ceport_list[i], operation_enable);
            		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d %s fail.\r\n",
                        wp->username, ceport_list[i], operation_enable);
                    //websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed !\")\r\n</script>");
            		err_flag=1;
            	}
                DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s success.\r\n",ceport_list[i], operation_enable);
            }
            if(SingleState == 0 || SingleState == 1)
            {
                if(adp_port_linkdown_trans_set(ceport_list[i], SingleState) != OK)
                {
                    DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s fail.\r\n",ceport_list[i], operation_sigl);
            		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d %s fail.\r\n",
                        wp->username, ceport_list[i], operation_sigl);
                    //websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed !\")\r\n</script>");
            		err_flag=1;
                }
                DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s success.\r\n",ceport_list[i], operation_sigl);
            }
            if(loopbackMode == 0 || loopbackMode == 1)
            {
                if(adp_port_loop_back_set(ceport_list[i], loopbackMode) != OK)
                {
                    DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s fail.\r\n",ceport_list[i], operation_cirl);
            		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d %s fail.\r\n",
                        wp->username, ceport_list[i], operation_cirl);
                    //websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed !\")\r\n</script>");
            		err_flag=1;
                }
                DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s success.\r\n",ceport_list[i], operation_cirl);
            }
        }
    }

    /* 10G端口设置 */
    if(strlen(tdXEnumlist) && !port_list_get_port_by_key(tdXEnumlist,xeport_list,&xeport_num))
    {
        AdminState   = atoi(selXEenable);
        SingleState  = atoi(selXEsigl);
        loopbackMode = atoi(selXEcirl);
        //speed        = atoi(selXErate);
        strcpy(operation_enable, (AdminState==1)?"undo shutdown":"shutdown");
        strcpy(operation_sigl, (SingleState==1)?"set single transmit":"undo single transmit");
        strcpy(operation_cirl, (loopbackMode==1)?"set loopback mode":"undo loopback mode");
	// printf("[%s %d] xeport_num:%d \r\n",__FUNCTION__,__LINE__,xeport_num);
        for(i = 0; i < xeport_num; i++)
        {

            if(AdminState == 0 || AdminState == 1)  // 0 1 才设置
            {
            	if(adp_port_mng_state_set(xeport_list[i], AdminState) != OK)
            	{
            		DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s fail.\r\n",xeport_list[i], operation_enable);
            		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d %s fail.\r\n",
                        wp->username, xeport_list[i], operation_enable);
                    //websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed !\")\r\n</script>");
            		err_flag=1;
            	}
                DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s success.\r\n",xeport_list[i], operation_enable);
            }
            if(SingleState == 0 || SingleState == 1)
            {
                if(adp_port_linkdown_trans_set(xeport_list[i], SingleState) != OK)
                {
                    DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s fail.\r\n",xeport_list[i], operation_sigl);
            		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d %s fail.\r\n",
                        wp->username, xeport_list[i], operation_sigl);
                    //websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed !\")\r\n</script>");
            		err_flag=1;
                }
                DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s success.\r\n",xeport_list[i], operation_sigl);
            }
            if(loopbackMode == 0 || loopbackMode == 1)
            {
                if(adp_port_loop_back_set(xeport_list[i], loopbackMode) != OK)
                {
                    DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s fail.\r\n",xeport_list[i], operation_cirl);
            		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d %s fail.\r\n",
                        wp->username, xeport_list[i], operation_cirl);
                    //websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed !\")\r\n</script>");
            		err_flag=1;
                }
                DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d %s success.\r\n",xeport_list[i], operation_cirl);
            }
            // 设速率, negotiation/10GFD/1000MFD  2 不设置
            if(vosStrCmp(selXErate, "negotiation")==0)
            {
                if(adp_port_negation_set(xeport_list[i], PL_ETHPORT_AUTO_NEG_ENABLE) != OK)
                {
                    DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d set speed %s fail.\r\n",xeport_list[i], selXErate);
            		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set speed %s fail.\r\n",
                        wp->username, xeport_list[i], selXErate);
                    //websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed !\")\r\n</script>");
            		err_flag=1;
                }
                DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d set speed %s success.\r\n",xeport_list[i], selXErate);
            }
            else if(vosStrCmp(selXErate, "10GFD")==0 || vosStrCmp(selXErate, "1000MFD")==0)
            {
                speed = (!vosStrCmp(selXErate, "10GFD")) ? PL_ETHPORT_SPEED_10G : PL_ETHPORT_SPEED_1000;
                if(adp_port_negation_set(xeport_list[i], PL_ETHPORT_AUTO_NEG_DISABLE) != OK)
                {
                    DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d set negotiation disable fail.\r\n",xeport_list[i]);
            		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d negotiation disable fail.\r\n",
                        wp->username, xeport_list[i]);
                    //websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed !\")\r\n</script>");
            		err_flag=1;
                }
                if(adp_port_speed_set(xeport_list[i], speed) != OK)
                {
                    DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d set speed %s fail.\r\n",xeport_list[i], selXErate);
            		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %d set speed %s fail.\r\n",
                        wp->username, xeport_list[i], selXErate);
                    //websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed !\")\r\n</script>");
            		err_flag=1;
                }
                DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %d set speed %s success.\r\n",xeport_list[i], selXErate);
            }
            else
            {
                ;
            }


        }
    }
    if(err_flag)
    {
		DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %s %s config fail.\r\n", tdCEnumlist, tdXEnumlist);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %s %s config fail.\r\n",wp->username, tdCEnumlist, tdXEnumlist);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
	DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "port %s %s config success.\r\n", tdCEnumlist, tdXEnumlist);
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port %s %s config success.\r\n",wp->username, tdCEnumlist, tdXEnumlist);
	websWrite(wp, WEB_RET_CONFIG_OK);
	websDone(wp);
	return;

}

static void web_get_log_file(Webs *wp, char *path, char *query)
{

	char *file_data=NULL;
	int ret = 0,file_len = 0;
	uchar file_name[32] = {0};
	int i = 0;
	//unsigned char *log_path=LOG_FILE_PATH;
	struct stat  s;

    for (i = 0; i < 10; i++)
	{
	    memset(file_name, 0, sizeof(file_name));
	    snprintf(file_name,32,"/mnt/flash/log/user.%d.log",(10-i-1));
		ret = stat(file_name, &s);
		if ((ret < 0) || (0 == (int)s.st_size))
		{
			continue;
		}

		file_len += (int) s.st_size;
	}
	(void)stat("/mnt/flash/log/user.log", &s);
	file_len += (int) s.st_size;
	file_data = vosAlloc(file_len);
	if(file_data==NULL)
	{
		printf("Memery malloc error !! \r\n");
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

	file_len = 0;
    for (i = 0; i < 10; i++)
	{
	    memset(file_name, 0, sizeof(file_name));
	    snprintf(file_name,32,"/mnt/flash/log/user.%d.log",(10-i-1));

		ret = stat(file_name, &s);
		if ((ret < 0) || (0 == (int)s.st_size))
		{
			continue;
		}
		(void)get_file_to_buffer(file_name, (file_data + file_len), (int)s.st_size+1);
		file_len += (int) s.st_size;
	}

    (void)stat("/mnt/flash/log/user.log", &s);
	(void)get_file_to_buffer("/mnt/flash/log/user.log", (file_data +file_len), (int)s.st_size+1);
	file_len += (int) s.st_size;
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "file_len:%d %s\r\n",file_len);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteHeader(wp, "Content-Type", "%s", "application/octet-stream");
	//websWriteHeader(wp, "Content-Type", "%s", "text/plain");
	websWriteHeader(wp, "Content-Disposition", "%s%s", "attachment; filename=","log.txt");
	websWriteEndHeaders(wp);
	websWrite(wp, file_data);

	free(file_data);
	file_data=NULL;
	websDone(wp);
}

static void web_option_user(Webs *wp, char *path, char *query)
{
	char	*option,*username_tatol,*p;
	char * username;
	char userName[5][16]={0},password[16]={0};
	int i=0,ret=OK,flag=0;

	username_tatol = websGetVar(wp, "edtUser", NULL);
	option= websGetVar(wp, "edtFlag", NULL);
	if(username_tatol==NULL || strnlen(username_tatol,16)<1 ||option==NULL || strnlen(option,2)<1 ||(!web_param_valid_check(username_tatol)))
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] option user fail, input params error.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_PARMA_ERR);
		websDone(wp);
		return;
	}

	flag=atoi(option);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"11userName=%s len=%d, flag:%d\r\n",username_tatol, strlen(username_tatol), flag);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	if(vosStrCmp(g_user_db[0].name,wp->username)!=0)
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] option user fail, no jurisdiction option.\r\n", wp->username);
		websWrite(wp, WEB_RET_NOT_ALLOW_OPERATION);
		websDone(wp);
		return ;
	}

	username=strtok(username_tatol, ";");
	while(username)
	{
	       if(flag!=3)
		{
			ret|=sys_active_user(username,flag-1);
		}
		else
		{
			ret|=sys_restore_user(username,CMD_PRI_ADMIN);
		}
		username = strtok(NULL,";");
	}
	if(ret==SUCCESS)
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] option user successful.\r\n", wp->username);
		websWrite(wp, WEB_RET_CONFIG_OK);
	}
	else
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] option user fail.\r\n", wp->username);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
	}

	websDone(wp);

	return;
}

static void web_get_sys_alarm(Webs *wp, char *path, char *query)
{

	struct tm tm;
	uchar buf[256] = {0};
	char *data=NULL;
 	st_alarm_info *alarm_info = NULL;
	int  alarm_num = 0,max_len=5000,ret=0,i,len=0;

	/* BEGIN: Added by liqi, 2016/11/3   PN:69542 */
	uchar  time_str[32] = {0};
	cJSON *data_json = NULL;
	cJSON *alarm_json = NULL;

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"[%s %d]\r\n",__FUNCTION__,__LINE__);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	data_json = cJSON_CreateArray();
	if(data_json == NULL)
	{
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"[%s %d]\r\n",__FUNCTION__,__LINE__);

	alarm_info = vosAlloc((sizeof(st_alarm_info))*ALARM_TABLE_MAX_NUM);
	if (alarm_info == NULL)
	{
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"[%s %d]\r\n",__FUNCTION__,__LINE__);
	memset(alarm_info, 0,sizeof(alarm_info));
	ret=adp_ssp_alarm_get(alarm_info, &alarm_num);

	if(ret==OK)
	{
		for (i = 0; i < alarm_num; i++)
		{

		    memcpy(&tm, &alarm_info[i].time, sizeof(struct tm));
			memcpy(buf, alarm_info[i].alarm_str, (sizeof(buf) - sizeof(struct tm)));
		    snprintf(time_str, 32, "%04d-%02d-%02d %02d:%02d:%02d",
              (tm.tm_year +1900), (tm.tm_mon+1), tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec);
		    cJSON_AddItemToArray(data_json, alarm_json=cJSON_CreateObject());
		    cJSON_AddStringToObject(alarm_json, "time", time_str);
            cJSON_AddStringToObject(alarm_json, "warning", buf);
		}
	}
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"[%s %s]\r\n",time_str,buf);

	free(alarm_info);
	alarm_info = NULL;
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"[%s %d]\r\n",__FUNCTION__,__LINE__);

    websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
    	websWrite(wp, WEB_RET_CONFIG_FAIL);
    	websDone(wp);
        return;
    }

    cJSON_Delete(data_json);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "data_send:%s\n",data);
	websWrite(wp, data);
	websDone(wp);
	free(data);
	data=NULL;

	return;
}

static void web_set_network1(Webs *wp, char *path, char *query)
{
	char	*ip, *mask ,*gate, *dns1,*dns2;
	char ifname[10]={0}, ifname_other[10]={0};
	char ip_other[20] = {0}, ip_old[20] = {0};
	int ret=0;

	ip = websGetVar(wp, "oob_ip1", NULL);
	mask = websGetVar(wp, "oob_mask1", NULL);
	gate = websGetVar(wp, "oob_gate1", NULL);

	memcpy(ifname,MANAG_ETH_PORT,5);
	memcpy(ifname_other,MANAG_ETH_PORT_EXT,7);
	vosIPAddrGet(ifname_other,ip_other);
	vosIPAddrGet(ifname, ip_old);
	if(ip!=NULL)
	{
		if(vosValidIpCheck(ip) && web_param_valid_check(ip) && vosStrCmp(ip_other, ip)!=0)
		{
			ret=vosIPAddrSet(ifname,ip);
			//sys_set_ip_rule(ifname, ip, ip_old);
			
			if (0 == ret){
				sccCtrlDip2DportMapSet(ip, 1, 0); 
			}
		}
		else
		{
			ret=-1;
			goto out;
		}
	}
	if(mask!=NULL )
	{
		if(vosValidIpCheck(mask) && web_param_valid_check(mask))
		{
			ret=vosNetMaskSet(ifname,mask);
		}
		else
		{
			ret=-1;
			goto out;
		}
	}
	if(gate!=NULL)
	{
		if(vosValidIpCheck(gate) && web_param_valid_check(gate))
		{
			ret=sys_set_gateway(ifname,gate);
		}
		else
		{
			ret=-1;
			goto out;
		}
	}
out:
	if(ret==OK)
	{
		websSetStatus(wp, HTTP_CODE_OK);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] system set ip :%s mask:%s gateway:%s successful.\r\n",
			                      wp->username, ip, mask, gate);
		websWrite(wp,WEB_RET_CONFIG_OK);
		//websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"配置成功!\")\r\n</script>");
	}
	else
	{
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] system ip :%s mask:%s gateway:%s fail.\r\n",
			                                                                  wp->username, ip, mask, gate);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		//websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"配置失败!\")\r\n</script>");
	}
	websDone(wp);
}


static void web_set_network2(Webs *wp, char *path, char *query)
{
	char	*ip, *mask ,*gate, *dns1,*dns2;
	char ifname[10]={0}, ifname_other[10]={0};
	char ip_other[20] = {0}, ip_old[20] = {0};
	int ret=0;

	ip = websGetVar(wp, "oob_ip2", NULL);
	mask = websGetVar(wp, "oob_mask2", NULL);
	gate = websGetVar(wp, "oob_gate2", NULL);

	memcpy(ifname,MANAG_ETH_PORT_EXT,7);
	memcpy(ifname_other,MANAG_ETH_PORT,5);
	vosIPAddrGet(ifname_other,ip_other);
	vosIPAddrGet(ifname, ip_old);
	if(ip!=NULL)
	{
		if(vosValidIpCheck(ip) && web_param_valid_check(ip) && vosStrCmp(ip_other, ip)!=0)
		{
			ret=vosIPAddrSet(ifname,ip);
			//sys_set_ip_rule(ifname, ip, ip_old);
			if (0 == ret){
				sccCtrlDip2DportMapSet(ip, 2, 1); 
			}
		}
		else
		{
			ret=-1;
			goto out;
		}
	}
	if(mask!=NULL )
	{
		if(vosValidIpCheck(mask) && web_param_valid_check(mask))
		{
			ret=vosNetMaskSet(ifname,mask);
		}
		else
		{
			ret=-1;
			goto out;
		}
	}
	if(gate!=NULL)
	{
		if(vosValidIpCheck(gate) && web_param_valid_check(gate))
		{
			ret=sys_set_gateway(ifname,gate);
		}
		else
		{
			ret=-1;
			goto out;
		}
	}
out:
	if(ret==OK)
	{
		websSetStatus(wp, HTTP_CODE_OK);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] system set ip :%s mask:%s gateway:%s successful.\r\n",
			                      wp->username, ip, mask, gate);
		websWrite(wp,WEB_RET_CONFIG_OK);
		//websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"配置成功!\")\r\n</script>");
	}
	else
	{
		websSetStatus(wp, 200);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] system ip :%s mask:%s gateway:%s fail.\r\n",
			                                                                  wp->username, ip, mask, gate);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		//websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"配置失败!\")\r\n</script>");
	}
	websDone(wp);
}



static void web_SetHashMode(Webs *wp, char *path, char *query)
{
	pkt_type typev4set = 0;
	pkt_type typev6set = 0;
	pkt_type typel2set = 0;
	int i,err_flag=0;
	char	*ipv4_pro,*ipv4_sip,*ipv4_dip,*ipv4_sport,*ipv4_dport,*v4_tunel_out;
	char	*ipv6_pro,*ipv6_sip,*ipv6_dip,*ipv6_sport,*ipv6_dport,*v6_tunel_out;
	char *out_port,*smac,*dmac,*vlan_id,*ethtype;

	lbtypetab ipv4_tab[]={{websGetVar(wp, "CH4_pro", NULL),"protocol",FIELD_PROTOCOL},
						{websGetVar(wp, "CH4_sip", NULL),"sip",FIELD_IP4SRC},
						{websGetVar(wp, "CH4_dip", NULL),"dip",FIELD_IP4DST},
						{websGetVar(wp, "CH4_sport", NULL),"sport",FIELD_SRCL4},
						{websGetVar(wp, "CH4_dport", NULL),"dport",FIELD_DSTL4},
						{websGetVar(wp, "CH4_tun", NULL),"tunel-out",FIELD_IP_TUNNELOUT}};

	lbtypetab ipv6_tab[]={{websGetVar(wp, "CH6_pro", NULL),"protocol",FIELD_NEXT_HEAD},
						{websGetVar(wp, "CH6_sip", NULL),"sip",FIELD_IP6SRC},
						{websGetVar(wp, "CH6_dip", NULL),"dip",FIELD_IP6DST},
						{websGetVar(wp, "CH6_sport", NULL),"sport",FIELD_SRCL4},
						{websGetVar(wp, "CH6_dport", NULL),"dport",FIELD_DSTL4},
						{websGetVar(wp, "CH6_tun", NULL),"tunel-out",FIELD_IP_TUNNELOUT}};

	lbtypetab l2_tab[]={{websGetVar(wp, "L2_vid", NULL),"vid",FIELD_VLAN},
						{websGetVar(wp, "L2_etype", NULL),"etype",FIELD_ETHER_TYPE},
						{websGetVar(wp, "L2_smac", NULL),"smac",FIELD_FIELD_MACSA},
						{websGetVar(wp, "L2_dmac", NULL),"dmac",FIELD_FIELD_MACDA}};

	lbtypetab hash_mode[]={{websGetVar(wp, "hash_algorithm", NULL),"crc16ccitt",CRC16CCITT},
						{websGetVar(wp, "hash_algorithm", NULL),"crc16only",CRC16},
						{websGetVar(wp, "hash_algorithm", NULL),"crc16xor1",CRC16XOR1},
						{websGetVar(wp, "hash_algorithm", NULL),"crc16xor2",CRC16XOR2},
						{websGetVar(wp, "hash_algorithm", NULL),"crc16xor4",CRC16XOR4},
						{websGetVar(wp, "hash_algorithm", NULL),"crc16xor8",CRC16XOR8},
						{websGetVar(wp, "hash_algorithm", NULL),"crc32hi",CRC32HI},
						{websGetVar(wp, "hash_algorithm", NULL),"crc32lo",CRC32LO},
						{websGetVar(wp, "hash_algorithm", NULL),"xor16",XOR16},

	};

	for(i=0;i<sizeof(ipv4_tab)/sizeof(lbtypetab);i++)
	{
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "ipv4:%s, %s, %d\r\n",ipv4_tab[i].type,ipv4_tab[i].string,i);
		if((ipv4_tab[i].type!=NULL))
		{
			if((web_param_valid_check_no_or(ipv4_tab[i].type)) && (vosStrCmp(ipv4_tab[i].string,ipv4_tab[i].type)==0))
			{
				FILED_SET(typev4set,ipv4_tab[i].id);
		       	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "ipv4: %s ,%d\r\n",ipv4_tab[i].type,ipv4_tab[i].id);
			}
			else/*huangzhun add for app-scan*/
			{
				err_flag=1;
				websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
				goto out;
			}
		}
	}

	for(i=0;i<sizeof(ipv6_tab)/sizeof(lbtypetab);i++)
	{
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "ipv6:%s, %s, %d\r\n",ipv6_tab[i].type,ipv6_tab[i].string,i);
		if(ipv6_tab[i].type!=NULL)
		{
			if((web_param_valid_check_no_or(ipv6_tab[i].type))  && (vosStrCmp(ipv6_tab[i].string,ipv6_tab[i].type)==0))
			{
				FILED_SET(typev6set,ipv6_tab[i].id);
		       	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "ipv6: %s ,%d\r\n",ipv6_tab[i].type,ipv6_tab[i].id);
			}
			else/*huangzhun add for app-scan*/
			{
				err_flag=1;
				websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
				goto out;
			}
		}
	}

	for(i=0;i<sizeof(l2_tab)/sizeof(lbtypetab);i++)
	{
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "l2:%s,%s,%d\r\n",l2_tab[i].type,l2_tab[i].string,i);
		if(l2_tab[i].type!=NULL)
		{
			if((web_param_valid_check_no_or(l2_tab[i].type)) && (vosStrCmp(l2_tab[i].string,l2_tab[i].type)==0))
			{
				FILED_SET(typel2set,l2_tab[i].id);
		       	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "l2: %s ,%d\r\n",l2_tab[i].type,l2_tab[i].id);
			}
			else/*huangzhun add for app-scan*/
			{
				err_flag=1;
				websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
				goto out;
			}
		}
	}

	for(i=0;i<sizeof(hash_mode)/sizeof(lbtypetab);i++)
	{
		DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "hash_mode:%s,%s,%d\r\n",hash_mode[i].type,hash_mode[i].string,i);
		if(hash_mode[i].type!=NULL)
		{
			if((web_param_valid_check_no_or(hash_mode[i].type)) && (vosStrCmp(hash_mode[i].string,hash_mode[i].type)==0))
			{

				adp_loadbalance_hash_algorithm_profile_set(hash_mode[i].id);
				break;
			}
		}
	}
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "typev4set=%x\r\n",typev4set);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "typev6set=%x\r\n",typev6set);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "typel2set=%x\r\n",typel2set);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "hashmode=%x\r\n",hash_mode[i].id);

	/*clean first,then set*/
	adp_loadbalance_ipv4_set(0xffffff,0);
	adp_loadbalance_ipv6_set(0xffffff,0);
	adp_loadbalance_l2_set(0xffffff,0);

    	adp_loadbalance_l2_set(typel2set,1);
	adp_loadbalance_ipv4_set(typev4set,1);
	adp_loadbalance_ipv6_set(typev6set,1);


	websSetStatus(wp, 200);
out:
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if(err_flag)
	{
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		 LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] system set loadbalance hash mode fail.\r\n", wp->username);
	}
	else
	{
		websWrite(wp,WEB_RET_CONFIG_OK);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] system set loadbalance hash mode successful.\r\n", wp->username);
	}
	websDone(wp);
}

static void web_gethash(Webs *wp, char *path, char *query)
{
	uint32 filedmap = 0;
	uint hash_mode;
	int len = 0;
	char * data = NULL;
	cJSON *data_json = NULL;
    cJSON *lb_json = NULL;
	char ipv4factor[128]={0};
	char ipv6factor[128]={0};
	char hash_factor[9][32]={"crc16xor8", "crc16xor4", "crc16xor2", "crc16xor1",
		                   "crc16only", "xor16", "crc16ccitt", "crc32lo", "crc32hi"};
	char l2factor[64]={0};

    websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }

	adp_lb_global_profile_get(L2,&filedmap);
	if(filedmap!=0)
	{
		if(FILED_CHECK(filedmap, FIELD_FIELD_MACDA))
		{
			len += vosSnprintf(l2factor+len ,64, "dmac,");
		}
		if(FILED_CHECK(filedmap,FIELD_FIELD_MACSA))
		{
			len += vosSnprintf(l2factor+len ,64,"smac,");
		}
		if(FILED_CHECK(filedmap,FIELD_ETHER_TYPE))
		{
			len += vosSnprintf(l2factor+len ,64,"etype,");
		}
		if(FILED_CHECK(filedmap,FIELD_VLAN))
		{
			len += vosSnprintf(l2factor+len ,64,"vid,");
		}
		if (len)
		l2factor[len-1]='\0';
	}

	filedmap = 0;
	adp_lb_global_profile_get(IPV4,&filedmap);
	if(filedmap!=0)
	{
	    len = 0;
		if(FILED_CHECK(filedmap,FIELD_IP4DST))
		{
			len += vosSnprintf(ipv4factor+len,128,"dip,");
		}
		if(FILED_CHECK(filedmap,FIELD_IP4SRC))
		{
			len += vosSnprintf(ipv4factor+len,128,"sip,");
		}
		if(FILED_CHECK(filedmap,FIELD_DSTL4))
		{
			len += vosSnprintf(ipv4factor+len,128,"dport,");
		}
		if(FILED_CHECK(filedmap,FIELD_SRCL4))
		{
			len += vosSnprintf(ipv4factor+len,128,"sport,");
		}
		if(FILED_CHECK(filedmap,FIELD_PROTOCOL))
		{
			len += vosSnprintf(ipv4factor+len,128,"protocol,");
		}
		if(FILED_CHECK(filedmap,FIELD_IP_TUNNELOUT))
		{
			len += vosSnprintf(ipv4factor+len,128,"tunel-out,");
		}
		if (len)
		ipv4factor[len-1]='\0';
	}

	filedmap = 0;
	adp_lb_global_profile_get(IPV6,&filedmap);
	if(filedmap!=0)
	{
	    len = 0;
		if(FILED_CHECK(filedmap,FIELD_IP6DST))
		{
			len += vosSnprintf(ipv6factor+len,128,"dip,");
		}
		if(FILED_CHECK(filedmap,FIELD_IP6SRC))
		{
			len += vosSnprintf(ipv6factor+len,128,"sip,");
		}
		if(FILED_CHECK(filedmap,FIELD_DSTL4))
		{
			len += vosSnprintf(ipv6factor+len,128,"dport,");
		}
		if(FILED_CHECK(filedmap,FIELD_SRCL4))
		{
			len += vosSnprintf(ipv6factor+len,128,"sport,");
		}
		if(FILED_CHECK(filedmap,FIELD_NEXT_HEAD))
		{
			len += vosSnprintf(ipv6factor+len, 128,"protocol,");
		}
		if(FILED_CHECK(filedmap,FIELD_IP_TUNNELOUT))
		{
			len += vosSnprintf(ipv6factor+len, 128,"tunel-out,");
		}
		if (len)
		ipv6factor[len-1]='\0';
	}

	adp_loadbalance_hash_algorithm_profile_get(&hash_mode);

	cJSON_AddStringToObject(data_json, "ipv4factor", ipv4factor);
	cJSON_AddStringToObject(data_json, "ipv6factor", ipv6factor);
	cJSON_AddStringToObject(data_json, "L2factor", l2factor);
	cJSON_AddStringToObject(data_json, "hash_lg",(hash_mode<=CRC32HI)? hash_factor[hash_mode]:hash_factor[CRC16XOR8]);

	data = cJSON_Print(data_json);
	if(data == NULL)
	{
		cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
    	cJSON_Delete(data_json);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "data:%s\r\n",data);

	websWrite(wp,"%s",data);
	free(data);
	data = NULL;
	websDone(wp);
	return;
}

#if 0
static void web_getTrunk(Webs *wp, char *path, char *query)
{
	int 	i,j;
	int     trunk_num = 0;
	char 	*data=NULL;
	uint32  trkPortMember[PL_MAX_PORTNUM_PERTRUNK] = {0};
	char	trunk_id=0;
	char ports_str[512] = {0};
	int len = 0;
/* BEGIN: Added by liqi, 2016/11/2   PN:69542 */
	cJSON *data_json = NULL;
	cJSON *trunk_json = NULL;
	uint8    factor = 0;
	tSwPhyIfInfo  pPortphy;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	data_json = cJSON_CreateArray();   //创建json数组数据
	if(data_json == NULL)
	{
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    	for (trunk_id = 1; trunk_id <= PL_MAX_TRUNKNUM_SUPPORT; trunk_id++)
	{
		if(!adp_trunk_exit_check(trunk_id))
		{
			continue;
		}

		cJSON_AddItemToArray(data_json, trunk_json=cJSON_CreateObject());
		cJSON_AddNumberToObject(trunk_json, "trunkid", trunk_id);

		adp_trunk_psc_get(trunk_id, &factor);
		cJSON_AddNumberToObject(trunk_json, "factor", factor);

		adp_trunk_port_member_get(trunk_id,trkPortMember);

		memset(ports_str, 0, sizeof(ports_str));
		len = 0;
		for(i=0;i<PL_MAX_PORTNUM_PERTRUNK;i++)
		{
			if(trkPortMember[i]!=0)
			{
				memset(&pPortphy, 0, sizeof(pPortphy));
				adp_port_logic_to_phy_get(trkPortMember[i],&pPortphy);
				if (i != 0)
				{
					len += vosSnprintf(ports_str + len, 512,",");
				}
				if(pPortphy.subPort)
				{
					len += vosSnprintf(ports_str + len, 512,"%d.%d", pPortphy.port, pPortphy.subPort);
				}
				else
				{
					len += vosSnprintf(ports_str + len, 512,"%d", pPortphy.port);
				}
			}
		}
		cJSON_AddStringToObject(trunk_json, "ports", ports_str);
	}

	data = cJSON_Print(data_json);
	if(data == NULL)
	{
		cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	cJSON_Delete(data_json);


	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "data:%s\r\n",data);

	websWrite(wp,"%s",data);
	free(data);
	data=NULL;
	websDone(wp);
}

static void web_AddTrunk(Webs *wp, char *path, char *query)
{
	char *portid, *trunkid, *hashfactor;
	int hash_factor = 9;
	int trunk_opt,flag=0,ret=0;
	int i, port, card, trunk_id,port_num=0;
	char port_list[PORT_TOTOL_MAX_NUM]={0};

	card=1;
	portid = websGetVar(wp, "edtInPorts", NULL);
	trunkid = websGetVar(wp, "PortMapID", NULL);
	hashfactor = websGetVar(wp, "hashfactor", NULL);

	if(portid==NULL || trunkid==NULL || strlen(portid)<1 ||strlen(trunkid)<1 ||(!web_param_valid_check(trunkid)) ||(!web_param_valid_check(portid)))
	{
	    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] trunk add fail, input param error.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_PARMA_ERR);
		websDone(wp);
		return 0;
	}

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"portid=%s ,trunkid=%s hash:%s \r\n",portid,trunkid, hashfactor);
    	hash_factor = atoi(hashfactor);
	ret=web_port_list_calc(portid,port_list,&port_num);
	if(ret!=OK)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_PARMA_ERR);
		websDone(wp);
		return 0;
	}
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	trunk_id=atoi(trunkid);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG," trunk_id=%d\r\n",trunk_id);
	if(port_num > PL_MAX_PORTNUM_PERTRUNK)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] trunk %d add fail, port number already max\r\n", wp->username, trunk_id);
		websWrite(wp, WEB_RET_PARMA_ERR);
		websDone(wp);
		return;
	}
	else if (trunk_id <= 0 || trunk_id > PL_MAX_TRUNKNUM_SUPPORT)
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] trunk %d add fail, trunk id out of range\r\n", wp->username, trunk_id);
		websWrite(wp, WEB_RET_PARMA_ERR);
		websDone(wp);
		return;
	}
	else
	{
		trunk_opt=PL_TRKPORT_ADD;
		ret=adp_trunk_create(trunk_id);
		if(ret!=OK)
		{
			LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] trunk %d add fail.\r\n", wp->username, trunk_id);
			goto out;
		}
		ret = adp_trunk_psc_set(trunk_id, hash_factor);
		if(ret!=OK)
		{
			LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] trunk %d psc set fail.\r\n", wp->username, trunk_id);
			goto out;
		}
		for(i=0;i<port_num;i++)
		{
			port=port_list[i];
			if (check_port_is_invalid_state(port))
			{
				flag=1;
			    continue;
			}
			//ret|=add_trunk_group(port,card,trunk_id);
			ret|=adp_trunk_port_join(port,trunk_id);
		}
	}

out:
	if(ret!=OK)
	{
	    ret=adp_trunk_remove(trunk_id);
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] trunk %d add fail.\r\n", wp->username, trunk_id);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	if(flag)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] trunk %d add successful ignore invaid port.\r\n", wp->username, trunk_id);
		websWrite(wp, WEB_RET_PORT_INVAIT_CONFIG_OK);
	}
	else
	{
	    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] trunk %d add successful.\r\n", wp->username, trunk_id);
		websWrite(wp, WEB_RET_CONFIG_OK);
	}
	websDone(wp);

}

static void web_DelTrunk(Webs *wp, char *path, char *query)
{
	char	*option, *key, *value, *index;
	int num=1;
	int i=0;
	index =NULL;

	key = stok(wp->input.buf, "=", &value);
	if(key==NULL || strlen(key)<1)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	if (strcmp(key, "groupID")==0) {
		index = value;
		}
	if(index==NULL || strlen(index)<1 ||(!web_param_valid_check(index)))
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	num = atoi(index);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	if(OK==adp_trunk_remove(num))
	{
		websWrite(wp,WEB_RET_CONFIG_OK);
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] trunk %d delete successful.\r\n", wp->username, num);
	}
	else
	{
		LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] trunk %d delete fail.\r\n", wp->username, num);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
	}
	websDone(wp);
}
#endif
static void web_delPortMap(Webs *wp, char *path, char *query)
{
	int port=0;
	char	 *key, *value, *index;
	int num=1;
	index =NULL;

	key = stok(wp->input.buf, "=", &value);
	if(key==NULL || strlen(key)<1)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port map delete fail, input params error.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"value:%s,key:%s\r\n",value,key);
	if (strcmp(key, "groupID")==0) {
		index = value;
		}
	if(index==NULL || strlen(index)<1 ||(!web_param_valid_check(index)))
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	num = atoi(index);
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"num:%d\r\n",num);

	for(port=1;port<=PORT_TOTOL_MAX_NUM;port++)
	{
	    if (check_port_is_invalid_state(port))
        {
            continue;
        }
		adp_port_isolate_set(num, port, 0x7);
	}
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port map delete successful.\r\n", wp->username);
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	websWrite(wp, WEB_RET_CONFIG_OK);
	websDone(wp);
}
static void web_addPortMap(Webs *wp, char *path, char *query)
{
	int i=0,j=0,sport=0,port,port_num=0,flag=0;
	char separated,port_list[PORT_TOTOL_MAX_NUM]={0};

	char	*mapid, *portmap;

	mapid = websGetVar(wp, "PortMapID", NULL);
	portmap = websGetVar(wp, "edtInPorts", NULL);

	if(mapid==NULL || portmap==NULL || strlen(mapid)<1 ||strlen(portmap)<1 ||(!web_param_valid_check(portmap)) ||(!web_param_valid_check(mapid)))
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port map add fail, input params error.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return 0;
	}
	sport=atoi(mapid+2);
	web_port_list_calc(portmap,port_list,&port_num);
	if(check_port_is_invalid_state(sport))
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port map add fail, source port invalid.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp, WEB_RET_PORT_INVAIT_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	for(i=0;i<=port_num;i++)
	{
		for(port=1;port<=PORT_TOTOL_MAX_NUM;port++)
		{
			if(port_list[i]==port)
			{
				if (check_port_is_invalid_state(port))
		             {
							flag=1;
		                    continue;
		              }
				DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"sport =%d  dport= %d \r\n",sport,port);
				adp_port_isolate_set(sport, port, 0);
			}

		}
	}
	if(flag)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port map add successful, ignore dst port invalid.\r\n", wp->username);
		websWrite(wp, WEB_RET_PORT_INVAIT_CONFIG_OK);
	}
	else
	{
	    	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] port map add successful.\r\n", wp->username);
		websWrite(wp, WEB_RET_CONFIG_OK);
	}
	websDone(wp);

}

void get_portmap_by_sorce_port(int sport,int *pos,char *buff)
{
	int flag=0,dport,len=0,isolate=0;
	char *data=NULL;

	data=vosAlloc(200);
	if(data==NULL)
	{
		printf("memery malloc error!!\r\n");
		return;
	}
	memset(data,0,200);
	len += vosSprintf((data+len), "%d,bind_port:68b_", sport);
	for (dport = 1; dport<= PORT_TOTOL_MAX_NUM; dport++)
	{
		if (check_port_is_invalid_state(dport))
		{
		  len += vosSprintf((data+len), "0");
		}
		else
		{

			isolate = adp_port_isolate_get(sport, dport);
			if(isolate>0 || (sport==dport))
			{
		        	len += vosSprintf((data+len), "0");
			}
			else
			{
		        	len += vosSprintf((data+len), "1");
				if(sport!=dport)
				{
					flag++;
				}
			}
		}
		 if (0 == (dport % 4))
		{
            		len += vosSprintf((data+len), "_");
		}
	}
	len += vosSprintf((data+len), ";");
	if(flag>=1)
	{
		memcpy((buff+*pos),data,len);
		*pos+=len;
	}
	free(data);
	data=NULL;
}

static void web_getPortMap(Webs *wp, char *path, char *query)
{
	int sport,len=0;
	char *data=NULL;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	data=vosAlloc(6000);
	if(data==NULL)
	{
		printf("memery malloc error!!\r\n");
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	memset(data,0,6000);

	for (sport = 1; sport<= PORT_TOTOL_MAX_NUM; sport++)
	{
		if (check_port_is_invalid_state(sport))
		{
		    continue;
		}
		get_portmap_by_sorce_port(sport,&len,data);
	}
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"data=%s\n",data);

	websWrite(wp,"%s",data);
	free(data);
	data=NULL;
	websDone(wp);

}
#endif

static int showLanchTime(int eid, Webs *wp, int argc, char **argv)
{
	char data[48];
#ifndef __WEB_DEBUG__
	int year, month, day, hour, minute, second;
#endif

#ifdef __WEB_DEBUG__
    struct sysinfo info;
    time_t tm_val;
    struct tm *val;

    sysinfo(&info);
    tm_val = time(NULL);
    tm_val -= info.uptime;
    val = localtime(&tm_val);
    strftime(data, 48, "%F %T", val);
#else
	hdl_tm_t stime;
	stime=sysStartupTime;
	sprintf(data,"%d/%d/%d  %d:%.2d:%.2d",
		(2000+stime.tm_year),stime.tm_mon,stime.tm_mday,stime.tm_hour,stime.tm_min,stime.tm_sec);
#endif
	websWrite(wp, data);
    return 0;
}

#if 0
/* firmware update */
/*1：开始传输 2：传输失败 3：传输成功 4：开始激活  5：激活失败  6：激活成功 7：文件错误。*/
int update_status = 0;
int slot_update_status = 0;

/* 升级进度 */
int update_progress = 0;
int slot_update_progress = 0;

/* 请求次数 */
int upload_time = 0;
int slot_upload_time = 0;

/* 0:升级主控(注意，风扇程序是模仿成fpga程序，打包在主控程序里的)
 * 1:升级1号槽位    2:升级2号槽位   3:升级3号槽位   4:升级4号槽位  */
int update_slot = 0x00;

static void getUpdateStatus(Webs *wp, char *path, char *query)
{
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if(upload_time < 500)
	{
		upload_time++;
	}
	switch(update_status)
	{
		case 0:
			websWrite(wp, "%d:%d",update_status,0);
			update_progress=10;
			break;
		case 1:
		case 3:
		case 6:
			websWrite(wp, "%d:100",update_status);
			update_status=0;
			break;
		case 4:
			websWrite(wp, "%d:%d",update_status,(update_progress));
			break;
		case 2:
		case 5:
			websWrite(wp, "%d:%d",update_status,update_progress);
			update_status=0;
			break;
		case 7:
			websWrite(wp, "%d:0",update_status);
			update_status=0;
			break;
		default:
			websWrite(wp, "0:100");
			break;
	}
	websDone(wp);
}

static void getSlotUpdateStatus(Webs *wp, char *path, char *query)
{
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if(slot_upload_time < 500)
	{
		slot_upload_time++;
	}
    else
    {
        slot_upload_time = 0;
        //如果查询500次，也就是500s(8min)之后，还在查询，认为升级失败
        slot_update_status = 46;
    }
	switch(slot_update_status)
	{
		case 0:
        case 46:
            slot_update_progress = 0;
			break;
		case 53:
			slot_update_progress = 100;
			break;
		default:    // 之所以没有处理11是因为，升级过程中，直接将11和实际的进度传过去就好了
			break;
	}
    websWrite(wp, "%d:%d", slot_update_status, slot_update_progress);
	websDone(wp);
}

void web_upgrade_task(void)
{
	int ret=0;

    sleep(5);

    slot_update_status = 11;
    slot_update_progress = 5;

	if(update_slot==0x00)
	{
		ret=adp_firmware_upgrade(UPGRADE_WEB, "/mnt/ramdisk/fw_master.img",0);
	}
	else
	{
		ret=adp_firmware_upgrade_slave(UPGRADE_WEB, "/mnt/ramdisk/fw_slave.img",0);
	}

	if(ret < 0)
	{
	    printf("\nhdl_firmware_upgrade failed");
		update_status = 5;
        slot_update_status = 46;
	}
	else
	{
	    update_status = 6;
        slot_update_status = 53;
	}

    vosThreadExit(0);
}
void web_upgrade()
{
    uint id;

    id = vosThreadCreate("upgrade", 1024*1024, 100, web_upgrade_task, NULL); //16384
	if (0 == id)
	{
		printf("upgrade thread failed\r\n");
	}
}


/*update vxworks.gz task*/
int upgrade(uint upfd)
{
	int fp1,fp2;
	unsigned int fileSize,fileWriteLen;
	int readLen,nBytes,rc;
	struct stat  s;

	if (update_status==4)
    {
        return ERROR;
    }
	rc = stat("/mnt/ramdisk/FileEnm", &s);
	if (rc < 0)
	{
	    error("Cannot open upgrade file %s", "/mnt/ramdisk/FileEnm");
		update_status = 5;
        slot_update_status = 46;     //传输失败
		return ERROR;
	}

	update_status = 4;
	update_progress=5;
    slot_update_status = 0;     //开始升级，将升级状态和升级进度都给0
    slot_update_progress = 0;

	sleep(1);

	fileSize = (int) s.st_size;
	close(upfd);
	printf("fileSize = %d\r\n",fileSize);

#if 1
	if(update_slot==0x00)   //master
	{
    	rename("/mnt/ramdisk/FileEnm", "/mnt/ramdisk/fw_master.img");
	}
	else
	{
    	rename("/mnt/ramdisk/FileEnm", "/mnt/ramdisk/fw_slave.img");
	}
#endif
	web_upgrade();

	if(update_status==5 || update_status==7)
	{
		 return ERROR;
	}

	return OK;
}

static void UpdateSlot(Webs *wp, char *path, char *query)
{
    WebsKey     *s;
    WebsUpload  *up;
	char        *upfile;
    char        *varslot;
	int         ret=0;
    char        *slotId_str = NULL;
    int         slotTemp = 0;

    websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

    slotId_str = websGetVar(wp, "SlotId", NULL);

    printf("%s %s %d: slotId_str=%s\n", __FILE__, __FUNCTION__, __LINE__, slotId_str);
    slotTemp = str2int(slotId_str);

    if(slotTemp!=1 && slotTemp!=2 && slotTemp!=3 && slotTemp!=4)
    {
        websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
	    websWrite(wp, WEB_RET_CONFIG_FAIL);
	    websDone(wp);
    }
    else
    {
        update_slot = slotTemp;
    }

    ret=upgrade(wp->upfd);
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Update software version end.\r\n", wp->username);

	if(ret==OK)
	{
		websSetStatus(wp, 200);
		websWrite(wp, WEB_RET_CONFIG_OK);
		return;
	}
error:
	websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
	websWrite(wp, WEB_RET_CONFIG_FAIL);
	websDone(wp);
}

static void UpdateENM(Webs *wp, char *path, char *query)
{
    WebsKey         *s;
    WebsUpload  *up;
	char            *upfile,*varslot;
	int ret=0;

	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Update software version start.\r\n", wp->username);
    update_slot = 0x00;
	ret=upgrade(wp->upfd);
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Update software version end.\r\n", wp->username);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);
	if(ret==OK)
	{
		websSetStatus(wp, 200);
		websWrite(wp, WEB_RET_CONFIG_OK);
		return;
	}
error:
	websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
	websWrite(wp, WEB_RET_CONFIG_FAIL);
	websDone(wp);
}

static void web_update_configure(Webs *wp, char *path, char *query)
{
	int fp1,fp2,rc,keylen;
	unsigned int fileSize;
	char keyword[40]={0};
	struct stat  s;
	FILE *confp = NULL;

	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"web_update_configure start.\r\n");
	rc = stat("/mnt/ramdisk/FileImport", &s);
	if (rc < 0)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] open config file fail.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		error("Cannot open config file %s", "/mnt/flash/FileImport");
		if(g_lang_en)
		{
			websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"File open failed !\")\r\n</script>");
		}
		else
		{
			websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"文件开启失败!\")\r\n</script>");
		}
		websDone(wp);
		 return;
	}
	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteEndHeaders(wp);

	fileSize = (int) s.st_size;
	vosSystem("cp -f /mnt/ramdisk/FileImport /mnt/flash/");
	vosSystem("rm -rf /mnt/ramdisk/FileImport");
	DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"recaved update fileSize = %d\n",fileSize);
	confp = fopen("/mnt/flash/FileImport", "r");
	if (confp != NULL)
	{
		keylen=30;
		memcpy(keyword,"! configuration saved from vty",keylen);
		rc=check_file_by_keywords(keyword,keylen,confp);
		if(rc==OK)
		{
		    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] config file import successful.\r\n", wp->username);
			rename("/mnt/flash/FileImport", "/mnt/flash/start-config.conf");
			if(g_lang_en)
			{
				websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"File load successful !\")\r\n</script>");
			}
			else
			{
				websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"配置文件导入成功!\")\r\n</script>");
			}
			websDone(wp);
			return ;
		}
	}
	confp = fopen("/mnt/flash/FileImport", "r");
	if (confp != NULL)
	{
		keylen=25;
		memcpy(keyword,"! configuration board info",keylen);
		rc=check_file_by_keywords(keyword,keylen,confp);
		if(rc==OK)
		{
			confp = fopen("/mnt/flash/FileImport", "r");
			if (confp != NULL)
			{
				rc=board_info_process(confp);
				if(g_lang_en)
				{
					if(rc==0)
					{
						websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"successful !\")\r\n</script>");
					}
					else
					{
						websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed!\")\r\n</script>");
					}
				}
				else
				{
					if(rc==0){
						websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"设备信息导入成功!\")\r\n</script>");
					}else if(rc==2){
						websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"MAC地址长度超出范围!\")\r\n</script>");
					}else if(rc==3){
						websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"序列号长度超出范围!\")\r\n</script>");
					}else if(rc==4){
						websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"设备类型长度超出范围!\")\r\n</script>");
					}else{
						websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"设备信息导入失败!\")\r\n</script>");
					}
				}
				websDone(wp);
				return ;
			}
		}
	}
	if(g_lang_en)
	{
		websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed!\")\r\n</script>");
	}
	else
	{
		websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"文件导入失败!\")\r\n</script>");
	}
	websDone(wp);

}
#endif

static int ip1Show(int eid, Webs *wp, int argc, char **argv)
{
	char ipaddr[18]={"192.168.1.1"};
#ifndef __WEB_DEBUG__
	char ifname[10]={0};

	memcpy(ifname,MANAG_ETH_PORT,5);
	vosIPAddrGet(ifname,ipaddr);
#endif
	websWrite(wp, ipaddr);
    return 0;
}

static int ip2Show(int eid, Webs *wp, int argc, char **argv)
{
	char ipaddr[18]={"10.3.56.1"};
#ifndef __WEB_DEBUG__
	char ifname[10]={0};

	memcpy(ifname,MANAG_ETH_PORT_EXT,7);
	vosIPAddrGet(ifname,ipaddr);
#endif
	websWrite(wp, ipaddr);
    return 0;
}

static int macShow(int eid, Webs *wp, int argc, char **argv)
{
	char mac[20]={"6c:2e:33:15:c3:1d"};

#ifndef __WEB_DEBUG__
	char macaddr[6]={0};

	sys_get_mac_addr(macaddr);
	vosMacToStr(macaddr,mac);
#endif

	websWrite(wp, mac);
    return 0;
}

static int netmask1Show(int eid, Webs *wp, int argc, char **argv)
{
	char netmask[18]={"255.255.255.0"};
#ifndef __WEB_DEBUG__
	char ifname[10]={0};

	memcpy(ifname,MANAG_ETH_PORT,5);
	vosNetMaskGet(ifname,netmask);
#endif
	websWrite(wp, netmask);
    return 0;

}
static int netmask2Show(int eid, Webs *wp, int argc, char **argv)
{
	char netmask[18]={"255.255.255.0"};
#ifndef __WEB_DEBUG__
	char ifname[10]={0};

	memcpy(ifname,MANAG_ETH_PORT_EXT,7);
	vosNetMaskGet(ifname,netmask);
#endif
	websWrite(wp, netmask);
    return 0;

}

static int gateway1Show(int eid, Webs *wp, int argc, char **argv)
{
	char netmask[18]={"255.255.255.0"};
#ifndef __WEB_DEBUG__
	char ifname[10]={0};

	if(strlen(netgateway1)<18)
	{
		strcpy(netmask,netgateway1);
	}
#endif

	websWrite(wp, netmask);
    return 0;

}
static int gateway2Show(int eid, Webs *wp, int argc, char **argv)
{
	char netmask[18]={"255.255.255.0"};
#ifndef __WEB_DEBUG__
	char ifname[10]={0};

	if(strlen(netgateway2)<18)
	{
		strcpy(netmask,netgateway2);
	}
#endif

	websWrite(wp, netmask);
    return 0;

}


#ifndef __WEB_DEBUG__
static int dns1Show(int eid, Webs *wp, int argc, char **argv)
{
	char ipaddr[18]={"172.16.1.1"};

	websWrite(wp, ipaddr);
    return 0;

}
#endif

static int showDevSn(int eid, Webs *wp, int argc, char **argv)
{
#ifdef __WEB_DEBUG__
    websWrite(wp, "000504117055");
#else
    websWrite(wp, sys_get_hw_sn());
#endif

    return 0;
}

static int showDevType(int eid, Webs *wp, int argc, char **argv)
{
#ifdef __WEB_DEBUG__
    websWrite(wp, "OCML M1000");
#else
    websWrite(wp, sys_get_hw_mode());
#endif

    return 0;
}

#if 0
static void web_load_sys_config(Webs *wp, char *path, char *query)
{
	int len=0,Len_tol=0, rc=0;
	char *config_file=NULL,buf[128]={0};
	FILE *confp;
	int buffer_len = 10000;
    struct stat  s;

	websSetStatus(wp, 200);
	websWriteHeaders(wp, -1, 0);
	websWriteHeader(wp, "Content-Type", "%s", "application/octet-stream");

	websWriteHeader(wp, "Content-Disposition", "%s%s", "attachment; filename=","start_config.txt");
	websWriteEndHeaders(wp);

	//RUNNING_CONFIG_FILE
	memset(&s, 0, sizeof(struct stat));
	rc = stat(host.config, &s);

	confp = fopen (host.config, "r");
	if (confp == NULL)
	{
	    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] load config file fail.\r\n", wp->username);
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		printf ( "Can't open configuration file [%s]\n",host.config);
		if(g_lang_en)
		{
			websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"Failed!\")\r\n</script>");
		}
		else
		{
			websWrite(wp, "<script type=\"text/javascript\">\r\nalert(\"配置导出失败!\")\r\n</script>");
		}
		websWrite(wp, WEB_RET_EXPORT_FILE_ERR);
		websDone(wp);
		return ;
	}

	buffer_len =  ((int)s.st_size)? (int)s.st_size:10000;
	printf("config file size:%d\r\n", buffer_len);
	config_file=vosAlloc(buffer_len);
	if(config_file==NULL)
	{
		printf("Memery malloc error !! \r\n");
		websWrite(wp, WEB_RET_EXPORT_FILE);
		websDone(wp);
		return ;
	}
	memset(config_file,0,buffer_len);
	websSetStatus(wp, 200);
	while (fgets (buf, 128, confp))
	{
		len=strlen(buf);
		DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"len:%d %s\n",len,buf);
		if((Len_tol+len)>buffer_len)
		{
		    LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] load config file fail, file is too big.\r\n", wp->username);
			websWrite(wp, WEB_RET_EXPORT_FILE_ERR);
			websDone(wp);
			free(config_file);
			config_file=NULL;
			return ;
		}
		strcpy(config_file+Len_tol, buf);
		Len_tol+=len;
		len=0;
	}

 	fclose (confp);
	LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] load config file successful.\r\n", wp->username);
    DEBUG_TRACE(WEB_MODULE,LEVEL_SELFDEBUG,"load-config:len=%d \n",Len_tol);

	websWrite(wp, config_file);
	//websWriteSocket(wp, (char*)buf, sizeof(buf));
	websDone(wp);
	free(config_file);
	config_file=NULL;
}


/* BEGIN: Added by LQ, 2017/8/19   白盒波分业务相关  */


static int16 float2DecaInt16(float fValue)
{
	return (int16)(fValue*10);
}

static uint16 float2DecaUint16(float fValue)
{
	return (uint16)(fValue*10);
}
#endif

#ifdef __WEB_DEBUG__
#define RPSU_POWER_MAX_NUM 2
#define FAN_MAX_NUM 4
#endif
static void web_get_sysinfo(Webs *wp, char *path, char *query)
{
    uint8 i = 0;
#ifndef __WEB_DEBUG__
    char buffer[50] = {0};
    hdl_rpsu_info_t rpsu_info_temp;
#endif    
    char vccOut[RPSU_POWER_MAX_NUM][50] = {"power1_vccOut", "power2_vccOut"};
    char currentOut[RPSU_POWER_MAX_NUM][50] = {"power1_currentOut", "power2_currentOut"};
    char fanLevel[FAN_MAX_NUM][50] = {"fan1", "fan2", "fan3", "fan4"};
    
	char *data=NULL;
    cJSON *data_json = NULL;

	int cpu_used=30,mem_per=0;
#ifndef __WEB_DEBUG__
	unsigned long mem_used=0;
	mem_occupy_t mem_info;
#endif

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }

#ifndef __WEB_DEBUG__
	memset(&rpsu_info_temp, 0, sizeof(rpsu_info_temp));
	memcpy(&rpsu_info_temp, &g_sys_poll_info.sys_rpsu_info, sizeof (rpsu_info_temp));
#endif
	
#ifndef __WEB_DEBUG__
	for ( i=0 ; i < RPSU_POWER_MAX_NUM; ++i )
	{
		if(rpsu_info_temp.sys_rpsu_info_get[i].rpsu_abs && rpsu_info_temp.sys_rpsu_info_get[i].rpsu_power_good)
		{
			vosSprintf(buffer, "%.2f V", (float)rpsu_info_temp.sys_rpsu_info_get[i].rpsu_vout/100.0);
			cJSON_AddStringToObject(data_json, vccOut[i], buffer);
			vosSprintf(buffer, "%.2f A", (float)rpsu_info_temp.sys_rpsu_info_get[i].rpsu_iout/100.0);
			cJSON_AddStringToObject(data_json, currentOut[i], buffer);
		}
        else if(rpsu_info_temp.sys_rpsu_info_get[i].rpsu_abs && !rpsu_info_temp.sys_rpsu_info_get[i].rpsu_power_good)
        {
            cJSON_AddStringToObject(data_json, vccOut[i], "0.0 V");
            cJSON_AddStringToObject(data_json, currentOut[i], "0.0 A");
        }
        else
        {
        	cJSON_AddStringToObject(data_json, vccOut[i], "NA");
            cJSON_AddStringToObject(data_json, currentOut[i], "NA");
        }

	}
#else
	for ( i=0 ; i < RPSU_POWER_MAX_NUM; ++i ) {
        cJSON_AddStringToObject(data_json, vccOut[i], "0.0 V");
        cJSON_AddStringToObject(data_json, currentOut[i], "0.0 A");
    }
#endif
	
    for(i = 0; i < FAN_MAX_NUM; i++)
    {

#ifndef __WEB_DEBUG__
        if(g_sys_poll_info.sys_fan_module.fan_abs_get[i])
        {
            cJSON_AddNumberToObject(data_json, fanLevel[i], g_sys_poll_info.sys_fan_module.fan_level_get[i]);
        }
        else
        {
            cJSON_AddStringToObject(data_json, fanLevel[i], "NA");
        }
#else
        cJSON_AddStringToObject(data_json, fanLevel[i], "1");
#endif
    }

#ifndef __WEB_DEBUG__
	cpu_used=g_system_gloable_info.cpu_usage;

	/*获取MEMERY 利用率*/
	sys_get_memoccupy(&mem_info);
	mem_used=mem_info.total-mem_info.free;
	mem_per=((mem_used*1000)/mem_info.total);
#else
    mem_per = 200;
    cpu_used = 10;
#endif
    cJSON_AddNumberToObject(data_json, "memory_usage", (float)mem_per/10.0);
    cJSON_AddNumberToObject(data_json, "cpu_usage", cpu_used);

    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);

#ifndef __WEB_DEBUG__
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"sys_info=%s\r\n",data);
#endif

	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;

	return;
}

static void web_get_slot_info(Webs *wp, char *path, char *query)
{
    uint8 i = 0;
#ifndef __WEB_DEBUG__
    enmSlotContext_t* pCtx = NULL;
    devSlotType_t slotType;
    uint8 softVer[10]={0}, tempr[10]={0};
#endif
	char *data=NULL;
    cJSON *data_json = NULL;
    cJSON *row_json  = NULL;
    cJSON *slot_json = NULL;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
#ifndef __WEB_DEBUG__
    cJSON_AddNumberToObject(data_json, "onlineSlot", getActiveSlotCnt(0));
#else
    cJSON_AddNumberToObject(data_json, "onlineSlot", 1);
#endif
    cJSON_AddItemToObject(data_json, "rows", row_json=cJSON_CreateArray());

#ifndef __WEB_DEBUG__
    for(i = 1; i <= MAX_DEV_SLOT; i++)
    {
    	pCtx = getSlotCtxViaSlotId(0, i);
    	slotType = getDevSlotType(0, i);
    	vosSprintf(softVer, "%.1f", api_get_slot_firmware_version(0, i)/10.0);
    	vosSprintf(tempr, "%.1f", api_get_slot_temperature(0, i)/10.0);
        cJSON_AddItemToArray(row_json, slot_json=cJSON_CreateObject());
        cJSON_AddBoolToObject(slot_json, "status", pCtx->isActive);    // 不在位不显示
        cJSON_AddStringToObject(slot_json, "type", get_slot_description_by_type(slotType));
        cJSON_AddNumberToObject(slot_json, "mode", i-1);
        cJSON_AddStringToObject(slot_json, "softVer", softVer);
        cJSON_AddStringToObject(slot_json, "tempr", tempr);
    }
#else
    for(i = 1; i <= 3; i++) {
        cJSON_AddItemToArray(row_json, slot_json=cJSON_CreateObject());
        cJSON_AddBoolToObject(slot_json, "status", 1);    // 不在位不显示
        cJSON_AddStringToObject(slot_json, "type", "OCML M1000");
        cJSON_AddNumberToObject(slot_json, "mode", i - 1);
        cJSON_AddStringToObject(slot_json, "softVer", "1");
        cJSON_AddStringToObject(slot_json, "tempr", "1");
    }
#endif

    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);

#ifndef __WEB_DEBUG__
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"slot_info=%s\r\n",data);
#endif

	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;

	return;
}

static void web_get_olp_info(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    uint8 slotId = 0;
#ifndef __WEB_DEBUG__ 
    olpSlotData_t* pSlotData = NULL;
    enmSlotContext_t* pCtx = NULL;
#endif
    uint8  buffer[100] = {0};
 
	char *data=NULL;
    cJSON *data_json = NULL;

	slotId_str = websGetVar(wp, "slotID", NULL);
#ifndef __WEB_DEBUG__
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s\n",slotId_str);
	if(slotId_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
#endif

    slotId = atoi(slotId_str);

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
#ifndef __WEB_DEBUG__
	pCtx = getSlotCtxViaSlotId(0, slotId);
	if(pCtx->isActive == 1)
	{
		pSlotData = getDevOlpContext(0, slotId);	
		cJSON_AddStringToObject(data_json, "moduleId", api_get_olp_type(pSlotData->moduleId));
		cJSON_AddStringToObject(data_json, "workmode", api_get_olp_work_mode(pSlotData->workMode));
        cJSON_AddNumberToObject(data_json, "returnTime", pSlotData->returnTime);
        cJSON_AddNumberToObject(data_json, "switchTime", pSlotData->switchTime);
		cJSON_AddStringToObject(data_json, "workLine", pSlotData->workLine == OLP_PRI_SEND_PRI_RECV?0:1);
		//cJSON_AddStringToObject(data_json, "workLine", api_get_olp_work_line(pSlotData->workLine));
		api_get_olp_all_alarm(pSlotData->alarmInfo, buffer);
		cJSON_AddStringToObject(data_json, "alarmInfo", buffer);
        if (!is_ocml_module()) {
            cJSON_AddNumberToObject(data_json, "txPwr", getDevValue(&(pSlotData->txPwr))/100.0);
            cJSON_AddNumberToObject(data_json, "t1Pwr", getDevValue(&(pSlotData->t1Pwr))/100.0);
            cJSON_AddNumberToObject(data_json, "t2Pwr", getDevValue(&(pSlotData->t2Pwr))/100.0);
            cJSON_AddNumberToObject(data_json, "rxPwr", getDevValue(&(pSlotData->rxPwr))/100.0);
        }
        cJSON_AddNumberToObject(data_json, "r1Pwr", getDevValue(&(pSlotData->r1Pwr))/100.0);
        cJSON_AddNumberToObject(data_json, "r2Pwr", getDevValue(&(pSlotData->r2Pwr))/100.0);
        if (!is_ocml_module()) {
            cJSON_AddNumberToObject(data_json, "txAlmTh", getDevValue(&(pSlotData->txAlmTh))/100.0);
            cJSON_AddNumberToObject(data_json, "t1AlmTh", getDevValue(&(pSlotData->t1AlmTh))/100.0);
            cJSON_AddNumberToObject(data_json, "t2AlmTh", getDevValue(&(pSlotData->t2AlmTh))/100.0);
            cJSON_AddNumberToObject(data_json, "rxAlmTh", getDevValue(&(pSlotData->rxAlmTh))/100.0);
        }
        cJSON_AddNumberToObject(data_json, "r1AlmTh", getDevValue(&(pSlotData->r1AlmTh))/100.0);
        cJSON_AddNumberToObject(data_json, "r2AlmTh", getDevValue(&(pSlotData->r2AlmTh))/100.0);
        cJSON_AddNumberToObject(data_json, "r1SwitchTh", getDevValue(&(pSlotData->r1SwitchTh))/100.0);
        cJSON_AddNumberToObject(data_json, "r2SwitchTh", getDevValue(&(pSlotData->r2SwitchTh))/100.0);
        if (!is_ocml_module()) {
            cJSON_AddNumberToObject(data_json, "r1IL", getDevValue(&(pSlotData->r1IL))/100.0);
            cJSON_AddNumberToObject(data_json, "r2IL", getDevValue(&(pSlotData->r2IL))/100.0);		
            cJSON_AddNumberToObject(data_json, "t1IL", getDevValue(&(pSlotData->t1IL))/100.0);
            cJSON_AddNumberToObject(data_json, "t2IL", getDevValue(&(pSlotData->t2IL))/100.0);
        }
        if (is_ocml_module()) {
            cJSON_AddNumberToObject(data_json, "primaryLoss", getDevValue(&(pSlotData->t1IL))/100.0);
            cJSON_AddNumberToObject(data_json, "secondaryLoss", getDevValue(&(pSlotData->t2IL))/100.0);
        }

        cJSON_AddNumberToObject(data_json, "switchHy", getDevValue(&(pSlotData->switchHy))/100.0);
        cJSON_AddNumberToObject(data_json, "alarmHy", getDevValue(&(pSlotData->alarmHy))/100.0);		

	} 
#else
		cJSON_AddStringToObject(data_json, "moduleId", "OLP");
		cJSON_AddStringToObject(data_json, "workmode", "Auto-reversion");
        cJSON_AddNumberToObject(data_json, "returnTime", 3);
        cJSON_AddNumberToObject(data_json, "switchTime", 5);
		cJSON_AddStringToObject(data_json, "workLine", 0);
		cJSON_AddStringToObject(data_json, "alarmInfo", "r1 powerlow");
        cJSON_AddNumberToObject(data_json, "r1Pwr", -18);
        cJSON_AddNumberToObject(data_json, "r2Pwr", -5);
        cJSON_AddNumberToObject(data_json, "r1AlmTh", -20);
        cJSON_AddNumberToObject(data_json, "r2AlmTh", -15);
        cJSON_AddNumberToObject(data_json, "r1SwitchTh", 20);
        cJSON_AddNumberToObject(data_json, "r2SwitchTh", 25);
        cJSON_AddNumberToObject(data_json, "primaryLoss", 3);
        cJSON_AddNumberToObject(data_json, "secondaryLossRatio", 0.2);
        cJSON_AddNumberToObject(data_json, "primaryLossRatio", 0.2);
        cJSON_AddNumberToObject(data_json, "secondaryLoss", 3);

        cJSON_AddNumberToObject(data_json, "switchHy", 0.5);
        cJSON_AddNumberToObject(data_json, "alarmHy", 0.8);		
#endif
    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);
#ifndef __WEB_DEBUG__
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"olp_info=%s\r\n",data);
#endif

	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;

	return;
}


#if 0
static void web_set_olp_workmode(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    char* workmode_str = NULL;
    uint8 slotId = 0, workMode = 0;
    int32 ret = ERROR;

	slotId_str  = websGetVar(wp, "slotID", NULL);
    workmode_str  = websGetVar(wp, "workmode", NULL);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s,workmode:%s\n", slotId_str, workmode_str);
	if(slotId_str==NULL || workmode_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT || atoi(workmode_str) > 2)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId   = atoi(slotId_str);
    workMode = atoi(workmode_str);


    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

	if(workMode == 0)	
		workMode = OLP_MODE_AUTO_NONREVERSION;
	else if(workMode == 1)
		workMode = OLP_MODE_AUTO_REVERSION;
	else
		workMode = OLP_MODE_MANUAL;
	ret = api_set_olp_work_mode(0, slotId, workMode);

    if(ret == OK)
    {
        websWrite(wp,WEB_RET_CONFIG_OK);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set OLP work mode %s success.\r\n", wp->username, api_get_olp_work_mode(workMode));
    }
    else
    {
        websWrite(wp,WEB_RET_CONFIG_FAIL);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set OLP work mode %s fail.\r\n", wp->username, api_get_olp_work_mode(workMode));
    }

	websDone(wp);
	return;

}


static void web_set_olp_returntime(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    char* returntime_str = NULL;
    uint8 slotId = 0;
    uint16 returntime = 0;
    int32 ret = ERROR;

	slotId_str  = websGetVar(wp, "slotID", NULL);
    returntime_str  = websGetVar(wp, "returntime", NULL);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s,returntime:%s\n", slotId_str, returntime_str);
	if(slotId_str==NULL || returntime_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId   = atoi(slotId_str);
    returntime = (uint16)atoi(returntime_str);


    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

	ret = api_set_olp_return_time(0, slotId, returntime);

    if(ret == OK)
    {
        websWrite(wp,WEB_RET_CONFIG_OK);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set OLP return time %ds success.\r\n", wp->username, returntime);
    }
    else
    {
        websWrite(wp,WEB_RET_CONFIG_FAIL);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set OLP return time %ds fail.\r\n", wp->username, returntime);
    }

	websDone(wp);
	return;

}


static void web_set_olp_workline(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    char* workline_str = NULL;
    uint8 slotId = 0, workline = 0;
    int32 ret = ERROR;

	slotId_str  = websGetVar(wp, "slotID", NULL);
    workline_str  = websGetVar(wp, "workline", NULL);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s,workline:%s\n", slotId_str, workline_str);
	if(slotId_str==NULL || workline_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT || atoi(workline_str) > 1)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId   = atoi(slotId_str);
    workline = atoi(workline_str);

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

	if(workline == 0)	
		workline = OLP_PRI_SEND_PRI_RECV;   // 主发主收
	else
		workline = OLP_PRI_SEND_SECND_RECV; // 主发从收

	ret = api_set_olp_work_line(0, slotId, workline);

    if(ret == OK)
    {
        websWrite(wp,WEB_RET_CONFIG_OK);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set OLP work line %s success.\r\n", wp->username, api_get_olp_work_line(workline));
    }
    else
    {
        websWrite(wp,WEB_RET_CONFIG_FAIL);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set OLP work line %s fail.\r\n", wp->username, api_get_olp_work_line(workline));
    }

	websDone(wp);
	return;

}



static void web_set_olp_th_hy(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    char* type_str = NULL;
    char* value_str = NULL;
    uint8 slotId = 0;
    uint16 type = 0;
    float fValue = 0.0;
    int32 ret = ERROR;

	slotId_str  = websGetVar(wp, "slotID", NULL);
    type_str  = websGetVar(wp, "type", NULL);
    value_str  = websGetVar(wp, "value", NULL);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s, type:%s, value:%s\n", slotId_str, type_str, value_str);
	if(slotId_str==NULL || type_str==NULL || value_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId = atoi(slotId_str);
    type   = (uint16)atoi(type_str);
    fValue = atof(value_str);
    
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

	ret = api_set_olp_th_hy(0, slotId, type, fValue);

    if(ret == OK)
    {
        websWrite(wp,WEB_RET_CONFIG_OK);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set OLP threshold/hysteresis parameter success.\r\n", wp->username);
    }
    else
    {
        websWrite(wp,WEB_RET_CONFIG_FAIL);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set OLP threshold/hysteresis parameter fail.\r\n", wp->username);
    }

	websDone(wp);
	return;

}


static void web_set_olp_loss(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    char* type_str = NULL;
    char* value_str = NULL;
    uint8 slotId = 0; 
    uint16 type = 0; 
    float fValue = 0.0; 
    int32 ret = ERROR;

    slotId_str  = websGetVar(wp, "slotID", NULL);
    type_str  = websGetVar(wp, "type", NULL);
    value_str  = websGetVar(wp, "value", NULL);

    DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s, type:%s, value:%s\n", slotId_str, type_str, value_str);
    if(slotId_str==NULL || type_str==NULL || value_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT)
    {    
        websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
        websWriteHeaders(wp, -1, 0);
        websWriteEndHeaders(wp);
        websWrite(wp,WEB_RET_CONFIG_FAIL);
        websDone(wp);
        return;
    }    

    slotId = atoi(slotId_str);
    type   = (uint16)atoi(type_str);
    fValue = atof(value_str);
    
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    ret = api_set_olp_loss(0, slotId, type, fValue);

    if(ret == OK)
    {    
        websWrite(wp,WEB_RET_CONFIG_OK);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set OLP threshold/hysteresis parameter success.\r\n", wp->username);
    }    
    else 
    {    
        websWrite(wp,WEB_RET_CONFIG_FAIL);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set OLP threshold/hysteresis parameter fail.\r\n", wp->username);
    }    

    websDone(wp);
    return;

}
#endif

static void web_get_edfa1_info(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    uint8 slotId = 0;
    edfaSlotData_t* pSlotData = NULL;
    vgaPayload_t* pVgaData = NULL;
    enmSlotContext_t* pCtx = NULL;
    uint8 buffer[10] = {0};
 
	char *data=NULL;
    cJSON *data_json = NULL;

	slotId_str = websGetVar(wp, "slotID", NULL);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s\n",slotId_str);
	if(slotId_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId = atoi(slotId_str);

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    
	pCtx = getSlotCtxViaSlotId(0, slotId);
	if(pCtx->isActive == 1)
	{
		pSlotData = getDevEdfaContext(0, slotId);
		pVgaData  = (vgaPayload_t*)&pSlotData->vgaCtx;
		cJSON_AddNumberToObject(data_json, "chasisTemp", (int16)pVgaData->chasisTemp/10.0);
		cJSON_AddNumberToObject(data_json, "caseTemp", (int16)pVgaData->caseTemp/10.0);
        cJSON_AddNumberToObject(data_json, "stage1Pin", float2DecaInt16((int16)pVgaData->stage1Pin/100.0)/10.0);
        cJSON_AddNumberToObject(data_json, "stage1Pout", float2DecaInt16((int16)pVgaData->stage1Pout/100.0)/10.0);
		cJSON_AddNumberToObject(data_json, "gainActual", float2DecaInt16((int16)pVgaData->gainActual/100.0)/10.0);
		cJSON_AddNumberToObject(data_json, "psig", (int16)pVgaData->psig/10.0);
        cJSON_AddNumberToObject(data_json, "pump1Iop", (int16)pVgaData->pump1Iop/10.0);
        cJSON_AddNumberToObject(data_json, "pump1Temp", (int16)pVgaData->pump1Temp/10.0);
		cJSON_AddNumberToObject(data_json, "pump1Itec", (int16)pVgaData->pump1Itec/10.0);
		cJSON_AddNumberToObject(data_json, "pump1Vtec", (int16)pVgaData->pump1Vtec/10.0);
        cJSON_AddNumberToObject(data_json, "pump1Pwr", (int16)pVgaData->pump1Pwr/10.0);
        cJSON_AddNumberToObject(data_json, "pinTh", (int16)pVgaData->pinTh/100.0);
		cJSON_AddNumberToObject(data_json, "pinHy", (int16)pVgaData->pinHy/100.0);
		cJSON_AddNumberToObject(data_json, "poutTh", (int16)pVgaData->poutTh/100.0);
		cJSON_AddNumberToObject(data_json, "poutHy", (int16)pVgaData->poutHy/100.0);
		cJSON_AddNumberToObject(data_json, "gainSet", float2DecaInt16((int16)pVgaData->gainSet/100.0)/10.0);
		cJSON_AddNumberToObject(data_json, "gainTilt", float2DecaInt16((int16)pVgaData->gainTilt/100.0)/10.0);
		memcpy(buffer, pVgaData->sn, 6);
		cJSON_AddStringToObject(data_json, "sn", buffer);
		cJSON_AddStringToObject(data_json, "mcuInner", pVgaData->mcuInner);
		cJSON_AddStringToObject(data_json, "fpgaInner", pVgaData->fpgaInner);
		cJSON_AddStringToObject(data_json, "workMode", pVgaData->workMode);
		cJSON_AddStringToObject(data_json, "alarm", pVgaData->alarm);		
	
	} 

    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"edfa1_info=%s\r\n",data);

	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;

	return;
}


static void web_get_edfa2_info(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    uint8 slotId = 0;
    edfaSlotData_t* pSlotData = NULL;
    fgaPayload_t* pFgaData = NULL;
    enmSlotContext_t* pCtx = NULL;
	uint8 buffer[10] = {0};
	char *data=NULL;
    cJSON *data_json = NULL;

	slotId_str = websGetVar(wp, "slotID", NULL);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s\n",slotId_str);
	if(slotId_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId = atoi(slotId_str);

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    
	pCtx = getSlotCtxViaSlotId(0, slotId);
	if(pCtx->isActive == 1)
	{
		pSlotData = getDevEdfaContext(0, slotId);
		pFgaData  = (vgaPayload_t*)&pSlotData->fgaCtx;
		cJSON_AddNumberToObject(data_json, "mdTemp", (int16)pFgaData->mdTemp/10.0);
		cJSON_AddNumberToObject(data_json, "mdVcc", (int16)pFgaData->mdVcc/10.0);
        cJSON_AddNumberToObject(data_json, "pin", float2DecaInt16((int16)pFgaData->pin/100.0)/10.0);
        cJSON_AddNumberToObject(data_json, "pout", float2DecaInt16((int16)pFgaData->pout/100.0)/10.0);
		cJSON_AddNumberToObject(data_json, "psig", float2DecaInt16((int16)pFgaData->psig/100.0)/10.0);
		cJSON_AddNumberToObject(data_json, "gainSet", float2DecaInt16((int16)pFgaData->gainSet/100.0)/10.0);
		cJSON_AddNumberToObject(data_json, "gainActual", float2DecaInt16((int16)pFgaData->gainActual/100.0)/10.0);
		cJSON_AddNumberToObject(data_json, "gainTilt", (int16)pFgaData->gainTilt/10.0);
        cJSON_AddNumberToObject(data_json, "pump1Iop", (int16)pFgaData->pump1Iop/10.0);
        cJSON_AddNumberToObject(data_json, "pump1Temp", (int16)pFgaData->pump1Temp/10.0);
		cJSON_AddNumberToObject(data_json, "pump1Itec", (int16)pFgaData->pump1Itec/10.0);
		cJSON_AddNumberToObject(data_json, "pump1Vtec", (int16)pFgaData->pump1Vtec/10.0);
        cJSON_AddNumberToObject(data_json, "pump2Iop", (int16)pFgaData->pump2Iop/10.0);
        cJSON_AddNumberToObject(data_json, "pump2Temp", (int16)pFgaData->pump2Temp/10.0);
		cJSON_AddNumberToObject(data_json, "pump2Itec", (int16)pFgaData->pump2Itec/10.0);
		cJSON_AddNumberToObject(data_json, "pump2Vtec", (int16)pFgaData->pump2Vtec/10.0);
        cJSON_AddNumberToObject(data_json, "pinTh", (int16)pFgaData->pinTh/100.0);
		cJSON_AddNumberToObject(data_json, "pinHy", (int16)pFgaData->pinHy/100.0);
		cJSON_AddNumberToObject(data_json, "poutTh", (int16)pFgaData->poutTh/100.0);
		cJSON_AddNumberToObject(data_json, "poutHy", (int16)pFgaData->poutHy/100.0);
		memcpy(buffer, pFgaData->sn, 6);
		cJSON_AddStringToObject(data_json, "sn", buffer);
		cJSON_AddStringToObject(data_json, "mcuInner", pFgaData->mcuInner);
		cJSON_AddStringToObject(data_json, "fpgaInner", pFgaData->fpgaInner);
		cJSON_AddStringToObject(data_json, "workMode", pFgaData->workMode);
		cJSON_AddStringToObject(data_json, "alarm", pFgaData->alarm);		
	




    }

    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"edfa2_info=%s\r\n",data);

	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;

	return;
}

#if 0
static void web_get_ocm_info(Webs *wp, char *path, char *query)
{
    uint8 i = 0;
    char* slotId_str = NULL;
    char* ocmId_str = NULL;
    uint8 slotId = 0, ocmId = 0; 
    edfaSlotData_t* pSlotData = NULL;
    ocmPayload_t* pOcmData = NULL;    
    enmSlotContext_t* pCtx = NULL;
	char *data=NULL;
	float fValue = 60.5;
	uint8 buffer[10] = {0};
    cJSON *data_json = NULL;
    cJSON *row_json  = NULL;
    cJSON *wave_json = NULL;

	slotId_str  = websGetVar(wp, "slotID", NULL);
    ocmId_str  = websGetVar(wp, "ocmId", NULL);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s, ocmId:%s\n", slotId_str, ocmId_str);
	if(slotId_str==NULL || ocmId_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT || atoi(ocmId_str) > MAX_OCM_CNT)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
	
    slotId = atoi(slotId_str);
    ocmId  = atoi(ocmId_str);

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }

	pCtx = getSlotCtxViaSlotId(0, slotId);
	if(pCtx->isActive == 1)
	{
		pSlotData = getDevEdfaContext(0, slotId);
		pOcmData  = (vgaPayload_t*)&pSlotData->ocmCtx[ocmId-1];
		cJSON_AddNumberToObject(data_json, "totalPwr", (int16)pOcmData->totalPwr/100.0);
		cJSON_AddItemToObject(data_json, "rows", row_json=cJSON_CreateArray());
		for(i = 0; i < MAX_OCM_CHANNEL; i++)
		{
			cJSON_AddItemToArray(row_json, wave_json=cJSON_CreateObject());
			vosSprintf(buffer, "C%.1f", fValue-0.5*i);
			//cJSON_AddNumberToObject(wave_json, "waveLen", pOcmData->waveLen[i]/100.0+1500);
			cJSON_AddStringToObject(wave_json, "waveLen", buffer);
			cJSON_AddNumberToObject(wave_json, "power", float2DecaInt16((int16)pOcmData->pwr[i]/100.0)/10.0);
		}
	
	} 

    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"ocm_info=%s\r\n",data);

	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;

	return;
}
#endif

static void web_get_voa_info(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    uint8 slotId = 0;
    edfaSlotData_t* pSlotData = NULL;
    enmSlotContext_t* pCtx = NULL;

	char *data=NULL;
    cJSON *data_json = NULL;

	slotId_str = websGetVar(wp, "slotID", NULL);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s\n",slotId_str);
	if(slotId_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId = atoi(slotId_str);

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    
	pCtx = getSlotCtxViaSlotId(0, slotId);
	if(pCtx->isActive == 1)
	{
		pSlotData = getDevEdfaContext(0, slotId);
		cJSON_AddNumberToObject(data_json, "voaAtt1", float2DecaUint16(pSlotData->voaAtt1/100.0)/10.0);
		cJSON_AddNumberToObject(data_json, "voaAtt2", float2DecaUint16(pSlotData->voaAtt2/100.0)/10.0);		
	} 

    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"voa_info=%s\r\n",data);

	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;

	return;
}

#if 0
static void web_get_sfp_info(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    uint8 slotId = 0;
    edfaSlotData_t* pSlotData = NULL;
    sfpPayload_t* pSfpData = NULL;
    enmSlotContext_t* pCtx = NULL;
	uint8 tempStr[20] = {0};

	char *data=NULL;
    cJSON *data_json = NULL;

	slotId_str = websGetVar(wp, "slotID", NULL);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s\n",slotId_str);
	if(slotId_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId = atoi(slotId_str);

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    
	pCtx = getSlotCtxViaSlotId(0, slotId);
	if(pCtx->isActive == 1)
	{
		pSlotData = getDevEdfaContext(0, slotId);
		pSfpData  = (sfpPayload_t*)&pSlotData->sfpCtx;
		cJSON_AddNumberToObject(data_json, "txPwr", getDbm4Oeo(pSfpData->txPwr)/100.0);
		cJSON_AddNumberToObject(data_json, "rxPwr", getDbm4Oeo(pSfpData->rxPwr)/100.0);
		vosSnprintf(tempStr, sizeof(tempStr), "%d.%d", pSfpData->waveLen[0]*256+pSfpData->waveLen[1],pSfpData->waveLen[2]);
		cJSON_AddStringToObject(data_json, "waveLen", tempStr);
		cJSON_AddNumberToObject(data_json, "sfpTemp", float2DecaInt16((int16)pSfpData->sfpTemp/256.0)/10.0);	
		cJSON_AddNumberToObject(data_json, "vccVol", float2DecaUint16(pSfpData->vccVol/10000.0)/10.0);
		cJSON_AddNumberToObject(data_json, "txBias", float2DecaUint16(pSfpData->txBias*2/1000.0)/10.0);				
	} 

    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"sfp_info=%s\r\n",data);

	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;

	return;
}


static void web_set_edfa_th(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    char* edfaId_str = NULL;
    char* type_str = NULL;
    char* value_str = NULL;
    uint8 slotId = 0, edfaId = 0, type = 0;
    float fValue = 0.0;
    int32 ret = ERROR;

	slotId_str  = websGetVar(wp, "slotID", NULL);
    edfaId_str  = websGetVar(wp, "edfaNo", NULL);
    type_str  = websGetVar(wp, "type", NULL);
    value_str  = websGetVar(wp, "value", NULL);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s, edfaNo:%s, type:%s, value:%s\n", slotId_str, edfaId_str, type_str, value_str);
	if(slotId_str==NULL || edfaId_str == NULL || type_str==NULL || value_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT
		|| atoi(edfaId_str)> MAX_EDFA_CNT || atoi(type_str) > 2)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId = atoi(slotId_str);
    edfaId = atoi(edfaId_str);
    type   = atoi(type_str);
    fValue = atof(value_str);
    
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

	if(fValue < OPTICAL_TH_MIN || fValue > OPTICAL_TH_MAX)
	{
		printf("      Threshold para out of range(%d~%d)!\r\n", OPTICAL_TH_MIN, OPTICAL_TH_MAX);
	}
	else if(type == 1)
	{
		ret = api_set_edfa_input_th(0, slotId, edfaId, fValue);
	}
	else if(type == 2)
	{
		ret = api_set_edfa_output_th(0, slotId, edfaId, fValue);
	}

    if(ret == OK)
    {
        websWrite(wp,WEB_RET_CONFIG_OK);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set slot %d EDFA %d %s threshold %.1fdBm success.\r\n", wp->username, slotId, edfaId,
        	(type == 1) ? "input":"output", fValue);
    }
    else
    {
        websWrite(wp,WEB_RET_CONFIG_FAIL);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set slot %d EDFA %d %s threshold %.1fdBm fail.\r\n", wp->username, slotId, edfaId,
        	(type == 1) ? "input":"output", fValue);

    }

	websDone(wp);
	return;

}

static void web_set_edfa_gain(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    char* edfaId_str = NULL;
    char* value_str = NULL;
    uint8 slotId = 0, edfaId = 0;
    float fValue = 0.0;
    int32 ret = ERROR;

	slotId_str  = websGetVar(wp, "slotID", NULL);
    edfaId_str  = websGetVar(wp, "edfaNo", NULL);
    value_str  = websGetVar(wp, "value", NULL);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s, edfaNo:%s, value:%s\n", slotId_str, edfaId_str, value_str);
	if(slotId_str==NULL || edfaId_str == NULL || value_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT || atoi(edfaId_str)!=1)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId = atoi(slotId_str);
    edfaId = atoi(edfaId_str);
    fValue = atof(value_str);
    
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

	if(fValue > EDFA_GAIN_MAX || fValue < 0)
	{
		printf("      Gain para out of range(0~%d)!\r\n", EDFA_GAIN_MAX);
	}
	else
	{
		ret = api_set_edfa_gain(0, slotId, edfaId, fValue);
	}


    if(ret == OK)
    {
        websWrite(wp,WEB_RET_CONFIG_OK);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set slot %d EDFA %d gain %.2fdB success.\r\n", wp->username, slotId, edfaId, fValue);
    }
    else
    {
        websWrite(wp,WEB_RET_CONFIG_FAIL);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set slot %d EDFA %d gain %.2fdB fail.\r\n", wp->username, slotId, edfaId, fValue);

    }

	websDone(wp);
	return;

}


static void web_set_voa(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    char* voaId_str = NULL;
    char* value_str = NULL;
    uint8 slotId = 0, voaId = 0;
    float fValue = 0.0;
    int32 ret = ERROR;

	slotId_str  = websGetVar(wp, "slotID", NULL);
    voaId_str  = websGetVar(wp, "voaNo", NULL);
    value_str  = websGetVar(wp, "value", NULL);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s, voaNo:%s, value:%s\n", slotId_str, voaId_str, value_str);
	if(slotId_str==NULL || voaId_str == NULL || value_str==NULL || atoi(slotId_str) > MAX_DEV_SLOT|| atoi(voaId_str)> MAX_VOA_CNT)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}

    slotId = atoi(slotId_str);
    voaId  = atoi(voaId_str);
    fValue = atof(value_str);
    
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

	if(fValue < VOA_ATT_MIN|| fValue > VOA_ATT_MAX)
	{
		printf("      VOA attenuation para out of range(%d~%d)!\r\n", VOA_ATT_MIN, VOA_ATT_MAX);
	}
	else
	{
		ret = api_set_edfa_voa(0, slotId, voaId, fValue);
	}
	
    if(ret == OK)
    {
        websWrite(wp,WEB_RET_CONFIG_OK);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set slot %d EDFA VOA %d attenuation %.2fdB success.\r\n", wp->username, slotId, voaId, fValue);
    }
    else
    {
        websWrite(wp,WEB_RET_CONFIG_FAIL);
        LOG_TRACE(E_LOG_MOD_OPTREATE_INFO,"WEB:[%s] Set slot %d EDFA VOA %d attenuation %.2fdB fail.\r\n", wp->username, slotId, voaId, fValue);

    }

	websDone(wp);
	return;

}
#endif

static void web_get_voa(Webs *wp, char *path, char *query)
{
    char* slotId_str = NULL;
    char* voaId_str = NULL;
    uint8 slotId = 0, voaId = 0;
    uint16 tempValue = 0;
    int32 ret = ERROR;
	char *data=NULL;
    cJSON *data_json = NULL;

	slotId_str  = websGetVar(wp, "slotID", NULL);
    voaId_str  = websGetVar(wp, "voaNo", NULL);

#ifndef __WEB_DEBUG__
	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG, "slotID:%s, voaNo:%s\n", slotId_str, voaId_str);
	if(slotId_str==NULL || voaId_str == NULL || atoi(slotId_str) > MAX_DEV_SLOT|| atoi(voaId_str)> MAX_VOA_CNT)
	{
		websSetStatus(wp, HTTP_CODE_RANGE_NOT_SATISFIABLE);
		websWriteHeaders(wp, -1, 0);
		websWriteEndHeaders(wp);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
	}
#endif

    slotId = atoi(slotId_str);
    voaId  = atoi(voaId_str);
    
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);

#ifndef __WEB_DEBUG__
	ret = api_get_edfa_voa(0, slotId, voaId, (uint16*)&tempValue);
#endif

    data_json = cJSON_CreateObject();   //创建json数组数据
    if(data_json == NULL)
    {
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }

 	if(ret == OK)
 	{
 		cJSON_AddNumberToObject(data_json, "voaAtt", tempValue/100.0);
 	}
 	else
 	{
 		cJSON_AddStringToObject(data_json, "voaAtt", "NA");
 	}
 	  
    data = cJSON_Print(data_json);
    if(data == NULL)
    {
        cJSON_Delete(data_json);
		websWrite(wp,WEB_RET_CONFIG_FAIL);
		websDone(wp);
		return;
    }
    cJSON_Delete(data_json);

	DEBUG_TRACE(WEB_MODULE, LEVEL_SELFDEBUG,"get_voa_info=%s\r\n",data);

	websWrite(wp,"%s",data);
	websDone(wp);
	free(data);
	data=NULL;
	return;

}
/* END:   Added by LQ, 2017/8/19   PN: */


int web_main()
{
	char    *auth,*endpoints, *endpoint, *route, *tok;

	route = "route";
	auth = "auth.txt";

	g_lang_en=LANG_CHINESE;
    if (chdir(HOME_DIR_CH) < 0)
    {
        error("Can't change directory to %s", HOME_DIR_CH);
        return -1;
    }

	initPlatform();
    if (websOpen(HOME_DIR_CH, route) < 0) {
        error("Can't initialize server. Exiting.");
        return -1;
    }

	logHeader();

	if (websLoad(auth) < 0) {
	    error("Can't load %s", auth);
	    return -1;
	}

	endpoints = sclone(ME_GOAHEAD_LISTEN);
	for (endpoint = stok(endpoints, ", \t", &tok); endpoint; endpoint = stok(NULL, ", \t,", &tok))
	{
	    if (websListen(endpoint) < 0)
	   {
	        return -1;
	    }
	}

	wfree(endpoints);

	websDefineHandler("test", testHandler, 0, 0, 0);
	websAddRoute("/test", "test", 0);
#if ME_GOAHEAD_LEGACY
    printf("legacy Test\n");
	websUrlHandlerDefine("/legacy/", 0, 0, legacyTest, 0);
#endif
#if ME_GOAHEAD_JAVASCRIPT
    printf("asp Test\n");
	websDefineJst("aspTest", aspTest);
	websDefineJst("bigTest", bigTest);
	websDefineAction("test", actionTest);
	websDefineAction("showTest", showTest);
	websDefineAction("sessionTest", sessionTest);
#endif

	websDefineAction("login", loginServiceProc);
	websDefineAction("logout", logoutServiceProc);

	websDefineJst("showUsername", showUsername);//OK
	websDefineJst("showDevSn", showDevSn);
	websDefineJst("showDevType", showDevType);
	websDefineJst("showSoftVer", showSoftVer);
	websDefineJst("showSysTime", showSysTime);
	websDefineJst("showLanchTime", showLanchTime);
	websDefineJst("ip1Show", ip1Show);
	websDefineJst("showDevMac", macShow);
	websDefineJst("netmask1Show", netmask1Show);
	websDefineJst("gateway1Show", gateway1Show);
	websDefineJst("ip2Show", ip2Show);
	websDefineJst("netmask2Show", netmask2Show);
	websDefineJst("gateway2Show", gateway2Show);

#if 0   
	websDefineAction("web_getSysStatus", web_get_sys_status);
	websDefineAction("AddUser", web_add_user);
	websDefineAction("web_reFreshUser", web_option_user);
	websDefineAction("Modifypass", web_modify_user);
	websDefineAction("SetNetwork1", web_set_network1);
	websDefineAction("SetNetwork2", web_set_network2);
	websDefineAction("DelUser", web_del_user);
	websDefineAction("web_GetUserList", UserInfShow);
	websDefineAction("web_exportSysCfg", web_load_sys_config);
#endif
#if 0
	websDefineAction("web_setPortEnable", web_set_port_admin_state);
	websDefineAction("web_setSingSend", web_set_port_SingleTrans);
	websDefineAction("web_setCircle", web_set_port_loopback);
	websDefineAction("web_setSpeed", web_set_port_speed);
	websDefineAction("web_exportSysCfg", web_load_sys_config);
	websDefineAction("webSplitPort", web_set_port_separate);
	websDefineAction("web_getPortsAttr", web_get_port_attribute);
	websDefineAction("web_clearFlow", web_clear_flow);
	websDefineAction("addPortMap", web_addPortMap);
	websDefineAction("deleteMap", web_delPortMap);
	websDefineAction("web_getPortMap", web_getPortMap);
	websDefineAction("web_setHash", web_SetHashMode);
	websDefineAction("web_gethash", web_gethash);
	websDefineAction("web_getRule", web_get_rule);
	websDefineAction("web_delRule", web_del_rule);
	websDefineAction("web_delMulRules", web_del_rule_multi);
	websDefineAction("AddNewRule", web_add_rule);
	websDefineAction("web_getPortStatus", web_getPortStatus);
#endif

#if 0    
	websDefineAction("web_SaveCfg", web_save_cfg);
	websDefineAction("web_RcvCfg", web_restore_cfg);
	websDefineAction("web_SetTime", web_set_time);
	websDefineAction("web_Ntpd", web_set_ntp);
//	websDefineAction("web_language_change", web_language_change);
	websDefineAction("web_getNtpd", web_get_ntp);
	websDefineAction("web_getopticalparam", web_get_optical_mode_status);
	websDefineAction("web_reboot", web_sys_reboot);
	websDefineAction("web_getAllFlowInfos", web_getAllFlowInfos);
	websDefineAction("web_getFlowRate", web_getFlowRate);
	websDefineAction("getUpdateStatus", getUpdateStatus);
    websDefineAction("getSlotUpdateStatus", getSlotUpdateStatus);
	websDefineAction("UpdateENM", UpdateENM);
    websDefineAction("UpdateSlot", UpdateSlot);
	websDefineAction("FormImportCfg", web_update_configure);
	websDefineAction("web_GetConfig", web_get_running_config);
	websDefineAction("web_get_log_file", web_get_log_file);
	websDefineAction("web_getLog", web_get_log);
	websDefineAction("web_loadLogs", web_load_logs);
	websDefineAction("web_searchLog", web_search_log);
	websDefineAction("web_loadSrchlogs", web_load_search_logs);
  //  websDefineAction("web_Setport", web_set_port);
	websDefineAction("web_getWarning", web_get_sys_alarm);
#endif

    /*  业务相关 */
    websDefineAction("web_get_sysinfo", web_get_sysinfo);
    websDefineAction("web_get_slot_info", web_get_slot_info);
    websDefineAction("web_get_olp_info", web_get_olp_info);
#if 0
    websDefineAction("web_set_olp_workmode", web_set_olp_workmode);
    websDefineAction("web_set_olp_returntime", web_set_olp_returntime);
    websDefineAction("web_set_olp_workline", web_set_olp_workline);
    websDefineAction("web_set_olp_th_hy", web_set_olp_th_hy);
    websDefineAction("web_get_edfa1_info", web_get_edfa1_info);
    websDefineAction("web_get_edfa2_info", web_get_edfa2_info);
    websDefineAction("web_get_ocm_info", web_get_ocm_info);
    websDefineAction("web_get_voa_info", web_get_voa_info);
    websDefineAction("web_get_sfp_info", web_get_sfp_info);
    websDefineAction("web_set_edfa_th", web_set_edfa_th);
    websDefineAction("web_set_edfa_gain", web_set_edfa_gain);
    websDefineAction("web_set_voa", web_set_voa);
    websDefineAction("web_get_voa", web_get_voa);
#endif
#if ME_GOAHEAD_UPLOAD
    printf("upload Test\n");
	websDefineAction("uploadTest", uploadTest);
#endif

#if 0//ME_UNIX_LIKE && !MACOSX
	/*
	Service events till terminated
	*/
	if (websGetBackground()) {
        if (daemon(0, 0) < 0) {
            error("Can't run as daemon");
            return -1;
        }
    }
#endif
    printf("\nweb server is up.\r\n");

    websServiceEvents(&finished);
    logmsg(1, "Instructed to exit\n");
    websClose();
    return 0;
}

#ifdef __WEB_DEBUG__
int main(int argc, char *argv[])
{
    web_main();

    return 0;
}
#else
void web_init()
{
	//g_pWebThreadId = sal_thread_create("webtask", 96*1024, 100, web_main, 0);
   	 g_pWebThreadId = vosThreadCreate("webtask", 1024*1024, 100, web_main, 0);
	 if (g_pWebThreadId== 0)
	 {
	       DEBUG_TRACE(WEB_MODULE, LEVEL_DEBUG, "\r\web task create  error !!\r\n");
		printf("web task create error!!\r\n");
		return ;
	}
}
#endif
