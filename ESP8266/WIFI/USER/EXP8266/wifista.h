#ifndef _wifista_h
#define _wifista_h
#include "common.h"
int Sta_Connect(uint8_t *ipbuf,uint8_t *portnum );
void TCP_Client_Send(uint8_t *p);
void Wifi_Sta_Init(void);
extern uint8_t Wifi_Client_Send_Flag;

#endif



