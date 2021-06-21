#include <Servo.h>
#include <IRremote.h>

int RECV_PIN = 11; //红外线接收器OUTPUT端接在pin 11
int PWM = 5;      // 定义电机驱动引脚5，4
int DIR = 4;
int val = 255;     //电机占空比
Servo myservo;
IRrecv irrecv(RECV_PIN); // 定义IRrecv 对象来接收红外线信号
decode_results results; //解码结果放在decode_results构造的对象results里


void irdisplay(unsigned long value) // 按下按键后，显示本遥控对应的按键
{
switch(value){ //判定按下的是哪个按键，串口显示按钮名称
case 0xFF02FD:                //前进
 digitalWrite(DIR,LOW);
 analogWrite(PWM, 255 + val);
break;
case 0xFF9867:                //后退
 digitalWrite(DIR,HIGH);
 analogWrite(PWM, 255 - val);
break;
case 0xFFE01F:                //前 + 左
 digitalWrite(DIR,LOW);
 analogWrite(PWM, 255 + val);
 myservo.write(40);
break;
case 0xFF906F:               //前 + 右
 digitalWrite(DIR,LOW);
 analogWrite(PWM, 255 + val);
 myservo.write(120);
break;
case 0xFFA857:                //停止
 digitalWrite(PWM,LOW);
 digitalWrite(DIR,LOW);
 myservo.write(90);
}
}

void setup(){
  myservo.attach(3);     //舵机引脚
  pinMode(PWM,OUTPUT);
  pinMode(DIR,OUTPUT);  // 定义电机为输出引脚
  Serial.begin(9600);   //开启串口通信，并设定波特率为9600
  irrecv.enableIRIn();   //启动红外解码
  myservo.write(90);     //舵机初始角度
}

void loop() {
if (irrecv.decode(&results)) 
{//解码成功，收到一组红外线信号 
irdisplay(results.value); 
irrecv.resume(); 
} 
}
