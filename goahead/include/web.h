#ifndef _WEB_H_
#define _WEB_H_

/*web error code define*/
#define  WEB_RET_CONFIG_OK                        			"0"     /*����OK*/
#define  WEB_RET_CONFIG_FAIL                      			"1"     /*ͨ�� ����ʧ��*/
#define  WEB_RET_USER_IS_EXIST                    			"2"     /*�û������󣬱�������ĸ��ͷ*/
#define  WEB_RET_USER_NAME_ERR                    		"3"     /*�û������󣬱�������ĸ��ͷ*/
#define  WEB_RET_USER_LOGIN_PASSWORD_ERR           "4"     /*�û�������У�����*/
#define  WEB_RET_USER_LOGIN_PASSWORD_SIMPLE      "5"     /*�û���������ڼ�*/
#define  WEB_RET_USER_LOGIN_LOCK                  		"6"      /*�˺������������3�Σ��˺ű�����*/
#define  WEB_RET_USER_PASSWORD_MODIFY_ERR         "7"      /*�����޸�ʧ�� �����������벻һ��*/
#define  WEB_RET_PARMA_ERR                        			"8"      /*�����������*/
#define  WEB_RET_USER_PASSWORD_MODIFY_ERR2       "9"     /*���벻����ǰ������ͬ*/
#define  WEB_RET_MAX_NUMBER_ERR                   		"10"     /*�������ֵ��Χ*/
#define  WEB_RET_GET_OK                        				"11"     /*��ѯ����OK*/
#define  WEB_RET_GET_FAIL                      				"12"     /*��ѯ����ʧ��*/
#define  WEB_RET_DEL_OK                        				"13"     /*ɾ������OK*/
#define  WEB_RET_DEL_FAIL                      				"14"     /*ɾ������ʧ��*/
#define  WEB_RET_PORT_INVAIT_CONFIG_OK         		"15"     /*�˿���Ч ���Ժ���*/
#define  WEB_RET_PORT_INVAIT_CONFIG_FAIL       		"16"     /*�˿���Ч ֱ�ӷ���*/
#define  WEB_RET_CONFIG_OK_AND_REBOOT          		"17"     /*���ñ���ɹ�*/
#define  WEB_RET_STATUS_NO_CHANGE              		"18"     /*����ǰ��״̬һ���������ظ�*/
#define  WEB_RET_STATUS_CHANGE                 			"19"     /*״̬�����仯*/
#define  WEB_RET_ACL_RULE_ID_MAX_ERR           		"20"     /*RULE ID �������Χ*/
#define  WEB_RET_ACL_RULE_ID_ALRELDY_EXIST     	"21"     /*RULE ID �Ѿ�����*/
#define  WEB_RET_ACL_RULE_ID_NOT_EXIST     		"22"     /*RULE ID �Ѿ�����*/
/*TODO*/
#define  WEB_RET_USER_DONOT_DEL_ADMIN			"23" 	/*����Ա�ʻ�����ɾ��!*/
#define  WEB_RET_NTP_OPEN							"24" 	/*NTP������ͬ������*/
#define  WEB_RET_NTP_CLOSE						"25" 	/*NTP������ͬ���ر�*/

#define  WEB_RET_MD_TX_DISABLE_FAIL						"30"  /* ����ģ�鼤����ʧ�� */
#define  WEB_RET_MD_LOW_POWER_FAIL						"31"  /* ����ģ��͹���ģʽʧ�� */
#define  WEB_RET_MD_RESET_FAIL  						"32"  /* ģ�鸴λʧ�� */
#define  WEB_RET_CFP2_WAVELENGTH_FAIL				    "33"  /* CFP2_ACO��������ʧ�� */
#define  WEB_RET_CFP2_MODULATION_FAIL				    "34"  /* CFP2_ACO����ģʽ����ʧ�� */
#define  WEB_RET_CFP2_PRE_EMPHASIS_FAIL					"35"  /* CFP2_ACOԤ���ز�������ʧ�� */
#define  WEB_RET_CFP2_TX_POWER_FAIL						"36"  /* ����CFP2_ACO���⹦��ʧ�� */
#define  WEB_RET_GEARBOX_LOOPBACK_FAIL					"37"  /* ����Gearbox loopbackģʽʧ�� */
#define  WEB_RET_GEARBOX_INTERFACE_FAIL					"38"  /* ����Gearbox interfaceģʽʧ�� */
#define  WEB_RET_DSP_LOOPBACK_SET_FAIL					"39"  /* ����DSP loopbackģʽʧ�� */
#define  WEB_RET_DSP_LOOPBACK_READ_FAIL					"40"  /* ��ȡDSP loopbackģʽʧ�� */
#define  WEB_RET_DSP_LINE_FEC_FAIL						"41"  /* ����DSP��·��FECʧ�� */
#define  WEB_RET_DSP_OUTER_FEC_FAIL						"42"  /* ����DSP Outer FECʧ�� */
#define  WEB_RET_DSP_CYCLE_SLIP_INDEX_FAIL				"43"  /* ����DSP CycleSlipIndexʧ�� */
#define  WEB_RET_DSP_LINE_MODULATION_FAIL				"44"  /* ����DSP ��·�����ģʽʧ�� */
#define  WEB_RET_DSP_BCD_MODE_FAIL						"45"  /* ����DSP BcdModeʧ�� */



#define WEB_RET_RESTORE_OK                     			"60"      /*�ָ�Ĭ�����óɹ�*/
#define WEB_RET_RESTORE_FAIL                   			"61"      /*�ָ�Ĭ������ʧ��*/
#define WEB_RET_SYS_REBOOT                     			"62"      /*ϵͳ����*/
#define WEB_RET_NOT_ALLOW_OPERATION            		"63"      /*��Ȩ����*/

#define WEB_RET_MAC_LENTH_ERR                  			"64"      /*MAC ��ַ���ȴ���*/
#define WEB_RET_SN_LENTH_ERR                   			"65"      /* ���к� ���ȴ���*/
#define WEB_RET_DEV_TYPE_ERR                   			"66"      /* �豸���ʹ���*/

#define WEB_RET_IMPORT_FILE_ERR                			"67"      /* �����ļ�����*/
#define WEB_RET_IMPORT_FILE                    			"68"      /* �����ļ��ɹ�*/
#define WEB_RET_EXPORT_FILE_ERR                			"69"      /* �����ļ�����*/
#define WEB_RET_EXPORT_FILE                    			"70"      /* �����ļ��ɹ�*/
#define WEB_RET_OPEN_FILE_ERR                    			"71"      /* �����ļ�ʧ��*/

#endif

