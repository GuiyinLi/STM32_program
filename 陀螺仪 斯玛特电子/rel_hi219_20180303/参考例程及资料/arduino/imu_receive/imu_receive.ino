
#include "SoftwareSerial.h"

/* IMU 姿态接收程序
 *  硬件：arduino UNO
 *  连接 姿态模块TTL串口与 arudino UNO 2,3脚相连， GND与arduino GND连接。 使用软件模拟2,3脚串口读取姿态模块数据并用硬件串口(PIN1,2)显示
 *  由于软件串口占用arduino系统资源，建议将姿态模块输出频率调至20Hz以下
*/

SoftwareSerial mySerial01(2, 3); // RX, TX, 本例使用软串口跟模块通讯

/* 接收缓冲区 */
#define MAX_PACKET_LEN          (128)

/* 最终数据 */
uint8_t ID;  /* ID 数据包 */
int16_t AccRaw[3];  /* 原始化速度 */
int16_t GyoRaw[3];
int16_t MagRaw[3];
float Eular[3];   /* 欧拉角 */
int32_t Pressure; 

/* define a packet struct */
typedef struct
{
    uint32_t ofs;
    uint8_t buf[MAX_PACKET_LEN];
    uint16_t payload_len;
    uint16_t len;
}Packet_t;

void DispData(Packet_t *pkt);

/* state machine state */
enum status
{
    kStatus_Idle,
    kStatus_Cmd,
    kStatus_LenLow,
    kStatus_LenHigh,
    kStatus_CRCLow,
    kStatus_CRCHigh,
    kStatus_Data,
};

/* packet ID code */
typedef enum 
{
    kItemKeyStatus =            0x80,   /* key status           size: 4 */
    kItemID =                   0x90,   /* user programed ID    size: 1 */
    kItemUID =                  0x91,   /* Unique ID            size: 4 */
    kItemIPAdress =             0x92,   /* ip address           size: 4 */
    kItemAccRaw =               0xA0,   /* raw acc              size: 3x2 */
    kItemAccRawFiltered =       0xA1,   
    kItemAccLinear =            0xA2,
    kItemAccCalibrated =        0xA3,
    kItemGyoRaw =               0xB0,   /* raw gyro             size: 3x2 */
    kItemGyoRawFiltered =       0xB1,   
    kItemGyoCalibrated =        0xB3,
    kItemMagRaw =               0xC0,   /* raw mag              size: 3x2 */
    kItemMagRawFiltered =       0xC1,
    kItemMagCalibrated =        0xC3,
    kItemAtdE =                 0xD0,   /* eular angle          size:3x2 */
    kItemAtdQ =                 0xD1,   /* att q,               size:4x4 */
    kItemTemp =                 0xE0,   
    kItemPressure =             0xF0,   /* pressure             size:1x4 */
    kItemEnd =                  0xFF,   
}ItemID_t;


static Packet_t RxPkt;

/* CRC16 */
static void crc16_update(uint16_t *currectCrc, const uint8_t *src, uint32_t lengthInBytes)
{
    uint32_t crc = *currectCrc;
    uint32_t j;
    for (j=0; j < lengthInBytes; ++j)
    {
        uint32_t i;
        uint32_t byte = src[j];
        crc ^= byte << 8;
        for (i = 0; i < 8; ++i)
        {
            uint32_t temp = crc << 1;
            if (crc & 0x8000)
            {
                temp ^= 0x1021;
            }
            crc = temp;
        }
    } 
    *currectCrc = crc;
}

/* packet decode */
uint32_t Packet_Decode(uint8_t c)
{
    static uint16_t CRCReceived = 0; /* CRC value received from a frame */
    static uint16_t CRCCalculated = 0;  /* CRC value caluated from a frame */
    static uint8_t status = kStatus_Idle; /* state machine */
    static uint8_t crc_header[4] = {0x5A, 0xA5, 0x00, 0x00};
    
    switch(status)
    {
        case kStatus_Idle:
            if(c == 0x5A)
                status = kStatus_Cmd;
            break;
        case kStatus_Cmd:
            if(c == 0xA5)
                status = kStatus_LenLow;
            break;
        case kStatus_LenLow:
            RxPkt.payload_len = c;
            crc_header[2] = c;
            status = kStatus_LenHigh;
            break;
        case kStatus_LenHigh:
            RxPkt.payload_len |= (c<<8);
            crc_header[3] = c;
            status = kStatus_CRCLow;
            break;
        case kStatus_CRCLow:
            CRCReceived = c;
            status = kStatus_CRCHigh;
            break;
        case kStatus_CRCHigh:
            CRCReceived |= (c<<8);
            RxPkt.ofs = 0;
            CRCCalculated = 0;
            status = kStatus_Data;
            break;
        case kStatus_Data:
            RxPkt.buf[RxPkt.ofs++] = c;
            if(RxPkt.ofs >= RxPkt.payload_len)
            {
                /* calculate CRC */
                crc16_update(&CRCCalculated, crc_header, 4);
                crc16_update(&CRCCalculated, RxPkt.buf, RxPkt.ofs);
                
                /* CRC match */
                if(CRCCalculated == CRCReceived)
                {
                      /* 成功接收到一帧数据，开始显示 */
                      DispData(&RxPkt);
                }
                status = kStatus_Idle;
            }
            break;
        default:
              status = kStatus_Idle;
            break;
    }
}

/* 成功解析到一帧数据后的处理 */
void DispData(Packet_t *pkt)
{

  if(pkt->buf[0] == kItemID) /* user ID */
  {
    ID = pkt->buf[1];
  }
  if(pkt->buf[2] == kItemAccRaw)  /* Acc raw value */
  {
    memcpy(AccRaw, pkt->buf[3], 6);
  }
             
  if(pkt->buf[9] == kItemGyoRaw)  /* gyro raw value */
  {
    memcpy(GyoRaw, pkt->buf[10], 6);
  }
             
  if(pkt->buf[16] == kItemMagRaw)  /* mag raw value */
  {
    memcpy(MagRaw, pkt->buf[17], 6);
  }
  if(pkt->buf[23] == kItemAtdE)  /* atd E */
  {
    Eular[0] = ((float)(int16_t)(pkt->buf[24] + (pkt->buf[25]<<8)))/100;
    Eular[1] = ((float)(int16_t)(pkt->buf[26] + (pkt->buf[27]<<8)))/100;
    Eular[2] = ((float)(int16_t)(pkt->buf[28] + (pkt->buf[29]<<8)))/10;
  }
  
  /* 打印姿态角 */
  Serial.print("Pitch:");
  Serial.print(Eular[0]);
  Serial.print("  ");
  Serial.print("Roll:");
  Serial.print(Eular[1]);
  Serial.print("  ");

  Serial.print("Yaw:");
  Serial.print(Eular[2]);
  Serial.print("  ");
  Serial.print("\r\n");

}


void setup() 
{                
  Serial.begin(115200);
  Serial.print("hardware UART initilization complete\r\n");

  mySerial01.begin(115200);
  Serial.print("software UART initilization complete\r\n");
  Serial.print("begin decode\r\n");
}

void loop() 
{
  if (mySerial01.overflow()) 
  {
    Serial.println("SoftwareSerial overflow!"); 
  }
  
  /* 如果软件串口2/3 接收到姿态模块数据，则处理并显示 */
  if (mySerial01.available()) 
  {
    uint8_t ch = mySerial01.read();
    Packet_Decode(ch);
  }

//  delay(100);
}
