#ifndef _WEB_H_
#define _WEB_H_

/*web error code define*/
#define  WEB_RET_CONFIG_OK                        			"0"     /*返回OK*/
#define  WEB_RET_CONFIG_FAIL                      			"1"     /*通用 返回失败*/
#define  WEB_RET_USER_IS_EXIST                    			"2"     /*用户名错误，必须以字母开头*/
#define  WEB_RET_USER_NAME_ERR                    		"3"     /*用户名错误，必须以字母开头*/
#define  WEB_RET_USER_LOGIN_PASSWORD_ERR           "4"     /*用户名密码校验错误*/
#define  WEB_RET_USER_LOGIN_PASSWORD_SIMPLE      "5"     /*用户名密码过于简单*/
#define  WEB_RET_USER_LOGIN_LOCK                  		"6"      /*账号输入次数多于3次，账号被锁定*/
#define  WEB_RET_USER_PASSWORD_MODIFY_ERR         "7"      /*密码修改失败 密码两次输入不一致*/
#define  WEB_RET_PARMA_ERR                        			"8"      /*参数输入错误*/
#define  WEB_RET_USER_PASSWORD_MODIFY_ERR2       "9"     /*密码不能与前三次相同*/
#define  WEB_RET_MAX_NUMBER_ERR                   		"10"     /*超出最大值范围*/
#define  WEB_RET_GET_OK                        				"11"     /*查询返回OK*/
#define  WEB_RET_GET_FAIL                      				"12"     /*查询返回失败*/
#define  WEB_RET_DEL_OK                        				"13"     /*删除返回OK*/
#define  WEB_RET_DEL_FAIL                      				"14"     /*删除返回失败*/
#define  WEB_RET_PORT_INVAIT_CONFIG_OK         		"15"     /*端口无效 可以忽略*/
#define  WEB_RET_PORT_INVAIT_CONFIG_FAIL       		"16"     /*端口无效 直接返回*/
#define  WEB_RET_CONFIG_OK_AND_REBOOT          		"17"     /*配置保存成功*/
#define  WEB_RET_STATUS_NO_CHANGE              		"18"     /*配置前后状态一样，配置重复*/
#define  WEB_RET_STATUS_CHANGE                 			"19"     /*状态发生变化*/
#define  WEB_RET_ACL_RULE_ID_MAX_ERR           		"20"     /*RULE ID 超出最大范围*/
#define  WEB_RET_ACL_RULE_ID_ALRELDY_EXIST     	"21"     /*RULE ID 已经存在*/
#define  WEB_RET_ACL_RULE_ID_NOT_EXIST     		"22"     /*RULE ID 已经存在*/
/*TODO*/
#define  WEB_RET_USER_DONOT_DEL_ADMIN			"23" 	/*管理员帐户不能删除!*/
#define  WEB_RET_NTP_OPEN							"24" 	/*NTP服务器同步开启*/
#define  WEB_RET_NTP_CLOSE						"25" 	/*NTP服务器同步关闭*/

#define  WEB_RET_MD_TX_DISABLE_FAIL						"30"  /* 设置模块激光器失败 */
#define  WEB_RET_MD_LOW_POWER_FAIL						"31"  /* 设置模块低功耗模式失败 */
#define  WEB_RET_MD_RESET_FAIL  						"32"  /* 模块复位失败 */
#define  WEB_RET_CFP2_WAVELENGTH_FAIL				    "33"  /* CFP2_ACO波长设置失败 */
#define  WEB_RET_CFP2_MODULATION_FAIL				    "34"  /* CFP2_ACO调制模式设置失败 */
#define  WEB_RET_CFP2_PRE_EMPHASIS_FAIL					"35"  /* CFP2_ACO预加重参数设置失败 */
#define  WEB_RET_CFP2_TX_POWER_FAIL						"36"  /* 设置CFP2_ACO发光功率失败 */
#define  WEB_RET_GEARBOX_LOOPBACK_FAIL					"37"  /* 设置Gearbox loopback模式失败 */
#define  WEB_RET_GEARBOX_INTERFACE_FAIL					"38"  /* 设置Gearbox interface模式失败 */
#define  WEB_RET_DSP_LOOPBACK_SET_FAIL					"39"  /* 设置DSP loopback模式失败 */
#define  WEB_RET_DSP_LOOPBACK_READ_FAIL					"40"  /* 读取DSP loopback模式失败 */
#define  WEB_RET_DSP_LINE_FEC_FAIL						"41"  /* 设置DSP线路侧FEC失败 */
#define  WEB_RET_DSP_OUTER_FEC_FAIL						"42"  /* 设置DSP Outer FEC失败 */
#define  WEB_RET_DSP_CYCLE_SLIP_INDEX_FAIL				"43"  /* 设置DSP CycleSlipIndex失败 */
#define  WEB_RET_DSP_LINE_MODULATION_FAIL				"44"  /* 设置DSP 线路侧调制模式失败 */
#define  WEB_RET_DSP_BCD_MODE_FAIL						"45"  /* 设置DSP BcdMode失败 */



#define WEB_RET_RESTORE_OK                     			"60"      /*恢复默认配置成功*/
#define WEB_RET_RESTORE_FAIL                   			"61"      /*恢复默认配置失败*/
#define WEB_RET_SYS_REBOOT                     			"62"      /*系统重启*/
#define WEB_RET_NOT_ALLOW_OPERATION            		"63"      /*无权操作*/

#define WEB_RET_MAC_LENTH_ERR                  			"64"      /*MAC 地址长度错误*/
#define WEB_RET_SN_LENTH_ERR                   			"65"      /* 序列号 长度错误*/
#define WEB_RET_DEV_TYPE_ERR                   			"66"      /* 设备类型错误*/

#define WEB_RET_IMPORT_FILE_ERR                			"67"      /* 导入文件错误*/
#define WEB_RET_IMPORT_FILE                    			"68"      /* 导入文件成功*/
#define WEB_RET_EXPORT_FILE_ERR                			"69"      /* 导出文件错误*/
#define WEB_RET_EXPORT_FILE                    			"70"      /* 导出文件成功*/
#define WEB_RET_OPEN_FILE_ERR                    			"71"      /* 开启文件失败*/

#endif

