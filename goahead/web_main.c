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

#include "cJSON.h"

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

static int showUsername(int eid, Webs *wp, int argc, char **argv)
{
	websWrite(wp, wp->username);

	return 0;
}

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

    data_json = cJSON_CreateObject();   //����json��������
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

	/*��ȡMEMERY ������*/
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

    data_json = cJSON_CreateObject();   //����json��������
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
        cJSON_AddBoolToObject(slot_json, "status", pCtx->isActive);    // ����λ����ʾ
        cJSON_AddStringToObject(slot_json, "type", get_slot_description_by_type(slotType));
        cJSON_AddNumberToObject(slot_json, "mode", i-1);
        cJSON_AddStringToObject(slot_json, "softVer", softVer);
        cJSON_AddStringToObject(slot_json, "tempr", tempr);
    }
#else
    for(i = 1; i <= 3; i++) {
        cJSON_AddItemToArray(row_json, slot_json=cJSON_CreateObject());
        cJSON_AddBoolToObject(slot_json, "status", 1);    // ����λ����ʾ
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

    data_json = cJSON_CreateObject();   //����json��������
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

    data_json = cJSON_CreateObject();   //����json��������
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

    data_json = cJSON_CreateObject();   //����json��������
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

    data_json = cJSON_CreateObject();   //����json��������
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

    data_json = cJSON_CreateObject();   //����json��������
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

    websDefineAction("web_get_sysinfo", web_get_sysinfo);
    websDefineAction("web_get_slot_info", web_get_slot_info);
    websDefineAction("web_get_olp_info", web_get_olp_info);

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
