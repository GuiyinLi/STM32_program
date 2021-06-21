#ifndef _wifiap_h
#define _wifiap_h
#include "common.h"
uint8_t Set_Server(uint8_t* SSID,uint8_t* PASS,uint8_t* portnum );
extern uint8_t Wifi_Server_Send_Flag;
void Wifi_Ap_Init(void);
void TCP_Server_Send(uint8_t *p,uint8_t id);
#endif




