/***
   android app 连接 bluetooth 4.0 ble 实现远程通信
   根据数据指令判断 - 默认小车控制指令：0X70/112，实现局域网遥控功能
 
   通信数据格式：[$M>](数据头)+[data length] +[code] +[data] +[checksum]
    例1: 24 4d 3e 00 72 72
      24  数据头1   $
      4d  数据头2   M
      3e  数据头3   >
      00  data length 无数据
      72  code  指令
      72  checksum  检验和 除数据头外的数据异或得到
    例2: 24 4d 3e 02 72 64 64 70
      24  数据头1   $
      4d  数据头2   M
      3e  数据头3   >
      02  data length 数据字节数
      72  code  指令
      64  data 1
      64  data 2
      70  checksum  检验和
 
   连接：(可根据实际情况更改)
 
   by yfrobot -- [url=http://www.yfrobot.com]www.yfrobot.com[/url]
   24/5/2016
*/
#include <Servo.h>
#include <SoftwareSerial.h>

#define DEBUG 1
SoftwareSerial mySerial(12, 11); // RX, TX
Servo myservo; 
#define C_Joystick 112       // Joystick code
#define PWM       5         // left motor
#define DIR       4         // left motor
#define SERVOPIN   3         //servo's pin
#define SERVOMID   90
 
#define INBUF_SIZE 128
static uint8_t inBuf[INBUF_SIZE];
static uint8_t dataLenght;
static uint8_t checksum;
static uint8_t indRX;
static uint8_t cmdMSP;
 
uint8_t read8() {
  return inBuf[indRX++] & 0xff;
}
 
uint8_t readThorttle = 128;
uint8_t readDirection = 128;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  myservo.attach(SERVOPIN);
  SERVOMID;
  delay(10);
 
  pinMode(PWM, OUTPUT);
  pinMode(DIR, OUTPUT);
}
 
void loop() {
  // put your main code here, to run repeatedly:
  BleReceive();
  setMotor(readThorttle, readDirection);
}
 
/**
   evaluate command -- 数据处理
*/
void evaluateCommand() {
  switch (cmdMSP) {
    case C_Joystick:
      readThorttle = read8();
      readDirection = read8();
      //      if (DEBUG) {
      //        mySerial.print("th  - ");
      //        mySerial.print(readThorttle);
      //        mySerial.print("di  - ");
      //        mySerial.println(readDirection);
      //      }
      break;
  }
}
 
void setMotor(int m1Speed , int dire) //电机驱动函数
{
  if (m1Speed < 128) {
      digitalWrite(DIR , HIGH);
      analogWrite(PWM, map(m1Speed, 0, 127, 0, 255));
      mySerial.println("forward");
    }else if (m1Speed > 128 ) {
      digitalWrite(DIR, LOW);
      analogWrite(PWM, map(m1Speed, 129, 255, 0, 255));
      mySerial.println("back");
    }else {
    digitalWrite(DIR, LOW);
    analogWrite(PWM, LOW);
  }
  if (dire < 128){
    myservo.write(map(dire,0,127,40,89));
  }else if (dire > 128){
    myservo.write(map(dire,129,255,91,130));
  }else {
    myservo.write(SERVOMID);
  }
  delay(50);
}

  

 
/**
    receive data function
*/
void BleReceive() {
  uint8_t c;
  static uint8_t dire_offset;
  static uint8_t dataSize;
  static enum _serial_state {
    IDLE,
    HEADER_START,
    HEADER_M,
    HEADER_ARROW,
    HEADER_SIZE,
    HEADER_CMD,
  } c_state = IDLE;
  while (Serial.available() > 0) {
    c = Serial.read();                               // 读串口缓冲区
    if (c_state == IDLE) {                           //串口状态空闲 等待HEADER_START状态的到来
      c_state = (c == '$') ? HEADER_START : IDLE;    //判定是$字符吗？是则进入HEADER_START状态
      if (c_state == IDLE) {}// evaluateOtherData(c); // evaluate all other incoming serial data
    } else if (c_state == HEADER_START) {
 
      c_state = (c == 'M') ? HEADER_M : IDLE;        //判定是M字符吗？是则进入HEADER_M状态
    } else if (c_state == HEADER_M) {
 
      c_state = (c == '>') ? HEADER_ARROW : IDLE;    //判定是>字符吗？是则进入HEADER_ARROW状态
    } else if (c_state == HEADER_ARROW) {            //是ARROW字符，进入HEADER_ARROW状态，判定缓冲区的大小
      if (c > INBUF_SIZE) {                          // now we are expecting the payload size 我们期望足够的数据占用缓冲区
        c_state = IDLE;                              //数据位置不够 退出循环
        continue;                                    //不执行该while循环包含的后面的语句，跳出开始下一轮循环
      }
      dataSize = c;
      dataLenght = dataSize;
      dire_offset = 0;
      checksum = 0;
      indRX = 0;
      checksum ^= c;                                 //校验和 1  -  dataSize
      c_state = HEADER_SIZE;                         // the command is to follow 接收到数据长度，进入HEADER_SIZE状态
    } else if (c_state == HEADER_SIZE) {
      cmdMSP = c;                                    //接收 指令(code)
      checksum ^= c;                                 //校验和 2  -  code
      c_state = HEADER_CMD;                          //接收到指令，进入HEADER_CMD状态
    } else if (c_state == HEADER_CMD && dire_offset < dataSize) {
      checksum ^= c;                                 //校验和 3  -  data
      inBuf[dire_offset++] = c;
    } else if (c_state == HEADER_CMD && dire_offset >= dataSize) {
      if (checksum == c) {// compare calculated and transferred checksum
        evaluateCommand();                    //数据处理程序
      }
      c_state = IDLE;                         //返回IDLE状态
    }
  }
}
