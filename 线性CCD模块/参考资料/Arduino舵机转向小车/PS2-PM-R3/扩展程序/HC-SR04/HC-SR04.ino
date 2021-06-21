#include <Servo.h>            //添加舵机库
Servo myservo;
Servo myservo1;                //定义2个舵机
int Echo = A5;                 // Echo回声脚
int Trig = A4;                 // Trig 触发脚
int PWM = 5;                   //定义电机PWM引脚
int DIR = 4;                   //定义电机方向引脚
int val = 200;                 //电机速度
 
int rightDistance = 0,leftDistance = 0,middleDistance = 0 ; 
 
int Distance_test()               // 量出前方距离 
{
  digitalWrite(Trig, LOW);        // 给触发脚低电平2μs
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);       // 给触发脚高电平10μs，这里至少是10μs
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);        // 持续给触发脚低电
  float Fdistance = pulseIn(Echo, HIGH);  // 读取高电平时间(单位：微秒)
  Fdistance= Fdistance/58;       //为什么除以58等于厘米，  Y米=（X秒*344）/2
  // X秒=（ 2*Y米）/344 ==》X秒=0.0058*Y米 ==》厘米=微秒/58
  return (int)Fdistance;
}  
 
void setup() { 
  Serial.begin(9600);             // 初始化串口
  pinMode(Echo, INPUT);            // 定义超声波输入脚
  pinMode(Trig, OUTPUT);           // 定义超声波输出脚  
  pinMode(PWM,OUTPUT);
  pinMode(DIR,OUTPUT);
  myservo.attach(3);               //定义转向舵机
  myservo1.attach(8);              //定义超声波舵机
  myservo.write(90);
} 
 
void loop() { 
  myservo1.write(90);
  delay(500);
  middleDistance = Distance_test();
  if(middleDistance <= 60) {                          //小于60厘米小车停止
    analogWrite(PWM,LOW);
    digitalWrite(DIR,LOW);
    
    myservo1.write(0);                                //前左右测距
    delay(1000);
    rightDistance = Distance_test();
    myservo1.write(90);
    delay(1000);
    myservo1.write(180);
    delay(1000);
    leftDistance = Distance_test();
    delay(500);
    myservo1.write(90);
    delay(1000);
    if(rightDistance < 20 && leftDistance < 20){    //后退
      myservo.write(90);
      digitalWrite(DIR,HIGH);
      analogWrite(PWM, val);
      delay(3000);
    }
    if(rightDistance > leftDistance){               //右转
      digitalWrite(DIR,LOW);
      analogWrite(PWM, val);
      myservo.write(120);
      delay(2500);
      myservo.write(90);
    }else if(rightDistance < leftDistance){         //左转
      digitalWrite(DIR,LOW);
      analogWrite(PWM, val);
      myservo.write(35);
      delay(2500);
      myservo.write(90);
    }else{                                          //前进
      digitalWrite(DIR,LOW);
      analogWrite(PWM, val);
    }
  }else
    digitalWrite(DIR,LOW);
    analogWrite(PWM, val);
}

