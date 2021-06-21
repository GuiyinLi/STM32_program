/////////////////////
/*
GY53----MINI
VCC----VCC
GND----GND
1:RX---TX,send A5 45 EA to GY-53
2:TX---RX
3:MINI_TX---FT232_RX
*/
//////////////////
unsigned char re_Buf[11],counter=0;
unsigned char sign=0;
byte data_Buf[3]={0};
void setup() {
   Serial.begin(9600);  
  delay(1);    
  Serial.write(0XA5); 
  Serial.write(0X45);    //初始化,连续输出模式
  Serial.write(0XEA);    //初始化,连续输出模式
}

void loop() {
  unsigned char i=0,sum=0;
  uint16_t distance=0;
  if(sign)
  {   
     sign=0;
     for(i=0;i<7;i++)
      sum+=re_Buf[i]; 
     if(sum==re_Buf[i] )        //检查帧头，帧尾
     {  	       
          data_Buf[0]=re_Buf[4];
          data_Buf[1]=re_Buf[5];
          data_Buf[2]=re_Buf[6];
          distance=data_Buf[0]<<8|data_Buf[1];
          
           Serial.print("distance: ");
           Serial.print( distance);
           Serial.print(" mm,mode: ");
           Serial.println( data_Buf[2]);     
   }
  } 

}
void serialEvent() {
  while (Serial.available()) {   
    re_Buf[counter]=(unsigned char)Serial.read();
    if(counter==0&&re_Buf[0]!=0x5A) return;      // 检查帧头         
    counter++;       
    if(counter==8)                //接收到数据
    {    
       counter=0;                 //重新赋值，准备下一帧数据的接收 
       sign=1;
    }      
  }
}
