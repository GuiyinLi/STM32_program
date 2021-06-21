#include "rc522.h"
#include "spi.h"
#include "delay.h"
#include "string.h"
#include "oled.h"
#include "led.h"
#include "timer.h"
#include "key.h"
//////////////////////////////////////////////////////////
// M1卡分为16个扇区 每个扇区由四个块（0 1 2 3）组成
// 将16个扇区的64个块按绝对地址编号为：0~63
// 第0个扇区的块0（绝对地址）用于存放厂商代码 不可更改
// 每个扇区的块0 1 2为数据块 可存放数据
// 每个扇区的块3为控制块（绝对地址为块3、7、11。。。）包括密码A 存取控制 密码B

/*******************************
*连线方式:
*1--SDA  <----->PB12->RFID_CS
*2--SCK  <----->PB13
*3--MOSI <----->PB15
*4--MISO <----->PB14
*5--IRQ 悬空
*6--GND <----->GND
*7--RST <----->PB0
*8--VCC <----->VCC
************************************/
/*                                    全局变量                                         */
u8 Card_Type[2];    //卡类型  S50->0x04 S70->0x02
u8 SelectedSnr[4];  //卡片序列号->ID
u8 DefaultKey[6]={0xff,0xff,0xff,0xff,0xff,0xff}; //验证密钥
u8 Card_RFID[16];     //卡片信息

u8 card_1[4]={0xf0,0xd9,0x77,0x89};         //卡号 用于匹配
u8 card_2[4]={0x7B,0xB9,0xA1,0x0D};
u8 card_3[4]={0x00,0x00,0x00,0x00};         //卡号 用于匹配
u8 card_4[4]={0x00,0x00,0x00,0x00};

//unsigned char data1[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};

unsigned char data1[16] = {0x00,0x00,0x01,0x78,0xFF,0xFF,0xFE,0x87,0x00,0x00,0x01,0x78,0x01,0xFE,0x01,0xFE};
//M1卡的某一块写为如下格式，则该块为钱包，可接收扣款和充值命令
//4字节金额（低字节在前）＋4字节金额取反＋4字节金额＋1字节块地址＋1字节块地址取反＋1字节块地址＋1字节块地址取反 
unsigned char data2[4]  = {0,0,0,0x01};

Card_ID card_id;
u8 Read_Fail[]="Read Fail";
u8 Read_Success[]="Read OK";
u8 Write_Success[]="Write OK";
u8 Write_Fail[]="Write_Fail";
u8 S50[]="S50";
u8 S70[]="S70";
u8 Clear[]="          ";
u8 doller[]="$";
u8 status=MI_ERR;
int last_card_id=0;
/***************************************************************************************/
/* 初始化函数 */
char RC522_Init(void)
{
	SPI2_Init();
	Reset_RC522();
	M500PcdConfigISOType('A');  //设置工作方式
	return PcdHalt();
}

void Reset_RC522(void)
{
	PcdReset();
	PcdAntennaOff();

	PcdAntennaOn();
}
/*****************************基础函数**********************************************************/

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(u8   req_code,u8 *pTagType)
{
	char   status;  
	u8   unLen;
	u8   ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);
	WriteRawRC(BitFramingReg,0x07);
	SetBitMask(TxControlReg,0x03);
 
	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

	if ((status == MI_OK) && (unLen == 0x10))
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   status = MI_ERR;   }
   
	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(u8 *pSnr)
{
    char   status;
    u8   i,snr_check=0;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);
	
    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++){   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
        }
         if (snr_check != ucComMF522Buf[i]){   
				      status = MI_ERR;   
				 }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(u8 *pSnr)
{
    char   status;
    u8   i;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(u8   auth_mode,u8   addr,u8 *pKey,u8 *pSnr)
{
    char   status;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    memcpy(&ucComMF522Buf[2], pKey, 6); 
    memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          p [OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(u8   addr,u8 *p )
{
    char   status;
    u8   unLen;
    u8   i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	  if ((status == MI_OK) && (unLen == 0x90))
 //   if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(p , ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(p +i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          p [IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(u8   addr,u8 *p )
{
    char   status;
    u8   unLen;
    u8   i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, p , 16);
        for (i=0; i<16; i++)
        {    
        	ucComMF522Buf[i] = *(p +i);   
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    uint8_t   status;
    uint8_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(u8 *pIn ,u8   len,u8 *pOut )
{
    u8   i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIn +i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOut [0] = ReadRawRC(CRCResultRegL);
    pOut [1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
	 
	  RFID_RESET=1;
    delay_us(1); 
	  RFID_RESET=0;
    delay_us(1);
	  RFID_RESET=1;
    delay_us(1);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
	  WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_us(1);
    
    WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
	
	  WriteRawRC(TxAutoReg,0x40);       //
   
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//设置RC632的工作方式 
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(u8   type)
{
   if (type == 'A')                     //ISO14443_A
   { 
       ClearBitMask(Status2Reg,0x08);
       WriteRawRC(ModeReg,0x3D);//3F
       WriteRawRC(RxSelReg,0x86);//84
       WriteRawRC(RFCfgReg,0x7F);   //4F
   	   WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
	     WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
	     WriteRawRC(TPrescalerReg,0x3E);
	     delay_us(1000);
       PcdAntennaOn();
   }
   else{ return 1; }
   
   return MI_OK;
}
/////////////////////////////////////////////////////////////////////
//功    能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
u8 ReadRawRC(u8   Address)
{
    u8   ucAddr;
    u8   ucResult=0;
	  RFID_CS=0;
	
    ucAddr = ((Address<<1)&0x7E)|0x80;
	
	SPI2_ReadWriteByte(ucAddr);
	ucResult=SPI2_ReadWriteByte(0);
	  RFID_CS = 1;
   return ucResult;
}

/////////////////////////////////////////////////////////////////////
//功    能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(u8   Address, u8   value)
{  
    u8   ucAddr;

	  RFID_CS=0;
	
    ucAddr = ((Address<<1)&0x7E);

	 SPI2_ReadWriteByte(ucAddr);
	 SPI2_ReadWriteByte(value);
	RFID_CS=1;
	
	  delay_us(15);
}
/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(u8   reg,u8   mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(u8   reg,u8   mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pIn [IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOut [OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(u8   Command, 
                 u8 *pIn , 
                 u8   InLenByte,
                 u8 *pOut , 
                 u8 *pOutLenBit)
{
    char   status = MI_ERR;
    u8   irqEn   = 0x00;
    u8   waitFor = 0x00;
    u8   lastBits;
    u8   n;
    long   i;
    switch (Command){
    case PCD_AUTHENT:
			   irqEn   = 0x12;
			   waitFor = 0x10;
			   break;
		case PCD_TRANSCEIVE:
			  irqEn   = 0x77;
			  waitFor = 0x30;
			  break;
		default:
			break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);	//清所有中断位
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);	 	//清FIFO缓存
    
    for (i=0; i<InLenByte; i++){   
		WriteRawRC(FIFODataReg, pIn [i]);   
		}
    WriteRawRC(CommandReg, Command);	  
    
    if (Command == PCD_TRANSCEIVE){    
		    SetBitMask(BitFramingReg,0x80); 
 		}	 //开始传送
    										 
    //i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
	  i = 100000;
    do 
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0){    
        if(!(ReadRawRC(ErrorReg)&0x1B)){
            status = MI_OK;
            if (n & irqEn & 0x01){   
						    status = MI_NOTAGERR;
						}
            if (Command == PCD_TRANSCEIVE){
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits){  
   								*pOutLenBit = (n-1)*8 + lastBits;  
								}else{  
   								*pOutLenBit = n*8;   
								}
                if (n == 0){   
								    n = 1;    
								}
                if (n > MAXRLEN){  
       						  n = MAXRLEN;   
								 }
                for (i=0; i<n; i++){   
								    pOut [i] = ReadRawRC(FIFODataReg);  
								}
            }
        }else{   
				     status = MI_ERR;  
 				}
        
    }
    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE); 
    return status;
}

/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
    u8   i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03)){
        SetBitMask(TxControlReg, 0x03);
    }
}


/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}


/* ---------------- */
/* RFID信息处理函数 */
/* snr:扇区号1-15   */
/* status 状态      */
/* ---------------- */
u32 money=0;
void RFID_Handle(u8 snr )
{
	vu8 key;                      //uchar
	static u8 check_times=0;
	static  u8 check=1;  //防止卡片一直通信 
	key=KEY_Scan(0);     //不支持连续按
	if(snr<=0)   //扇区非法 停止执行
		return ;
	status = PcdRequest(PICC_REQALL, Card_Type);//寻卡
	if((status==MI_OK))  //寻卡成功
	{
		status = PcdAnticoll(SelectedSnr);  //防冲撞
		if(status==MI_OK)  //防冲撞成功
		{
			status = PcdSelect(SelectedSnr);//选定卡片
			if(status==MI_OK)  //选定卡片成功
			{
				Find_CardID();  //进行卡号匹配
				Show_Type();
				Show_ID();      //显示卡号
				status = PcdAuthState(PICC_AUTHENT1A, (snr*4), DefaultKey, SelectedSnr);//校验1号扇区密码 密码位于每一扇区第3块 A密钥  饭卡无法读取是因为密钥不正确
				if(status==MI_OK)  //验证成功
				{
					check_times=0;
				  
					if(key==KEY0_PRES&&status==MI_OK)
					{
					  status = PcdWrite((snr*4), data1);//从扇区的起始块写   PB4
						if(status==MI_OK)
							OLED_P8x16Str(35,0,Write_Success);
						else
							OLED_P8x16Str(35,0,Write_Fail);
					}
          else if(status==MI_OK&&check)   //若写卡则不进行读卡
					{
					  status = PcdValue(PICC_DECREMENT,(snr*4),data2);  //扣款
						check=0;
            if(status==MI_OK)
						{
					    status=PcdRead((snr*4),Card_RFID);    //读卡
							if(status==MI_OK)  //读卡成功
					    {
								money=Card_RFID[0]<<24|Card_RFID[1]<<16|Card_RFID[2]<<8|Card_RFID[3];
						    //status = PcdBakValue((snr*4), ((snr-1)*4));//块备份
								//if(status==MI_OK)
								//{
						      OLED_ShowNum(16,6,money,6,16); 
								  OLED_P8x16Str(64,6,doller);
								//}
					    }
					    else
						    OLED_P8x16Str(45,6,Read_Fail); 
						}
					}
					
				}
			}
		}
	}
	else
	{
		check_times++;
		if(check_times==3)
		{
			led=1;    //未检测到卡 指示灯灭
		  OLED_Clear();
			check_times=0;
			check=1;
		}
		led_bit=0; //停止闪烁
		last_card_id=-1;  //清除上次卡片的记录
		PcdReset();
    PcdAntennaOff(); 
    PcdAntennaOn(); 
	  PcdHalt();      //进入休眠模式
	}
}

/* ----------------------- */
/* 对卡号进行匹配          */
/* ----------------------- */
void Find_CardID()
{

	if((SelectedSnr[0]==card_1[0])&&(SelectedSnr[1]==card_1[1])&&(SelectedSnr[2]==card_1[2])&&(SelectedSnr[3]==card_1[3]))
  {
		card_id=card_id_1;
  }
	else if((SelectedSnr[0]==card_2[0])&&(SelectedSnr[1]==card_2[1])&&(SelectedSnr[2]==card_2[2])&&(SelectedSnr[3]==card_2[3]))
  {
		card_id=card_id_2;
  }
	else if((SelectedSnr[0]==card_3[0])&&(SelectedSnr[1]==card_3[1])&&(SelectedSnr[2]==card_3[2])&&(SelectedSnr[3]==card_3[3]))
  {
		card_id=card_id_3;
  }
	else if((SelectedSnr[0]==card_4[0])&&(SelectedSnr[1]==card_4[1])&&(SelectedSnr[2]==card_4[2])&&(SelectedSnr[3]==card_4[3]))
  {
		card_id=card_id_4;
  }
	else
		card_id=unknow_card;
}

/* ----------------------- */
/* 显示卡号及提示信息      */
/* ----------------------- */
void Show_ID()
{
	if(card_id==unknow_card)
	{
		led_bit=1;
	}
	else
	{
		led_bit=0;                        
		led=0;
	}
	if(last_card_id!=card_id)        //卡号发生改变
	  OLED_ShowNum(45,4,card_id,3,16);
	last_card_id=card_id;
}
/* ----------------------- */
/* 显示卡片类型            */
/* ----------------------- */	
void Show_Type()
{
	if(Card_Type[0]==0x04)
		OLED_P8x16Str(45,2,S50);
	else if(Card_Type[0]==0x02)
		OLED_P8x16Str(45,2,S70);
}

/* 显示信息清除 */
void OLED_Clear()
{
	OLED_P8x16Str(35,0,Clear);
	OLED_P8x16Str(45,2,Clear);
	OLED_P8x16Str(45,4,Clear);
	OLED_P8x16Str(16,6,Clear);
}

/* -------------- */
/* 按键保存和删除 */
/* save->0        */
/* delete->1      */
/* -------------- */

void card_control(u8 status)
{
	if(SAVE&&card_id==unknow_card)
	{
	}
	else if(DELETE)
	{
	}
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                 
char PcdValue(u8 dd_mode,u8 addr,u8 *pValue)
{
    char status;
    u8  unLen;
    u8 ucComMF522Buf[MAXRLEN]; 
    //u8 i;
	
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        memcpy(ucComMF522Buf, pValue, 4);
        //for (i=0; i<16; i++)
        //{    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(u8 sourceaddr, u8 goaladdr)
{
    char status;
    u8  unLen;
    u8 ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}
				


