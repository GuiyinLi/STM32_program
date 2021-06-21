#ifndef _common_h
#define _common_h
#include "usart.h"
#include "string.h"
#include "stm32f1xx_hal.h"

void atk_8266_at_response(uint8_t mode);
uint8_t* atk_8266_check_cmd(uint8_t *str);
uint8_t atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);
uint8_t atk_8266_quit_trans(void);
uint8_t atk_8266_apsta_check(void);
void atk_8266_get_wanip(uint8_t* ipbuf);
uint8_t atk_8266_consta_check(void);
int WIFI_Init(int work_mode,int con_mode,uint8_t* wifista_ssid,uint8_t* wifista_password);
void atk_8266_send_data(uint8_t *data);
void Mode_Switch(void);
//用户配置参数
extern const uint8_t* portnum;			//连接端口
 
extern const uint8_t* wifista_ssid;		//WIFI STA SSID
extern const uint8_t* wifista_encryption;//WIFI STA 加密方式
extern const uint8_t* wifista_password; 	//WIFI STA 密码

extern const uint8_t* wifiap_ssid;		//WIFI AP SSID
extern const uint8_t* wifiap_encryption;	//WIFI AP 加密方式
extern const uint8_t* wifiap_password; 	//WIFI AP 密码

extern uint8_t* kbd_fn_tbl[2];
extern const uint8_t* ATK_ESP8266_ECN_TBL[5];
extern uint8_t AP_STA_MODE;
extern uint8_t AP_STA_SWITCH;
#endif


