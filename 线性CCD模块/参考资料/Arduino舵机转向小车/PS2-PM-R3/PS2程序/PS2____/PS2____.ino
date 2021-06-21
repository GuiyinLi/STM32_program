#include <Servo.h>
#include <PS2X_lib.h>  //for v1.6
PS2X ps2x;
Servo myservo;

#define PS2_DAT        A4   
#define PS2_CMD        A0   //PS2 pin
#define PS2_SEL        A1
#define PS2_CLK        A2
#define PWM            5   //Motor A's pin
#define DIR            4   //Motor A's pin
#define SERVOPIN       3   //servo pin

#define SERVOMIN 40
#define SERVOMID 90        //the angle of servo
#define SERVOMAX 120
#define TURNLEFT myservo.write(SERVOMIN)  //turn left
#define TURNMID myservo.write(SERVOMID)   //turn middle
#define TURNRIGHT myservo.write(SERVOMAX)  //turn right
#define MOTORFOWARD setMotor(255)   //motor foward
#define MOTORBACK setMotor(-255)     //motor back
#define MOTORSTOP setMotor(0)        //motor stop

bool EnableRockerR = 1;
bool EnableRockerL = 1;
int RMid = 0 ;
int LStop = 0 ;

#define pressures   false
#define rumble      false
int error = 0;
byte type = 0;
byte vibrate = 0;

void setup() {
  Serial.begin(57600);         // Baud rate
  pinMode(PWM, OUTPUT);         // Pin state
  pinMode(DIR, OUTPUT);
  myservo.attach(SERVOPIN);
  SERVOMID;                     // Servo middle     
  delay(300);
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

  if (error == 0) {
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true)");
    else
      Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }
  else if (error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
    case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
  }
}
void(* resetFunc) (void) = 0;

void loop() {
  if (error == 1) //skip loop if no controller found
    resetFunc();
  ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed

  if (ps2x.Button(PSB_START))        //will be TRUE as long as button is pressed
    Serial.println("Start is being held");
  if (ps2x.Button(PSB_SELECT))
    Serial.println("Select is being held");


  if (ps2x.ButtonPressed(PSB_PAD_UP)) {          //Foward pressed
    Serial.println("PAD_UP just pressed");
    MOTORFOWARD;
    EnableRockerL = 0;
  }else if(ps2x.ButtonReleased(PSB_PAD_UP)) {    //Foward released
    Serial.println(" PAD_UP just released");
    MOTORSTOP;
    EnableRockerL = 1;
    LStop = 0;
  }

  if (ps2x.ButtonPressed(PSB_PAD_DOWN)) {         //Down pressed
    Serial.println("PAD_DOWN just pressed");
    MOTORBACK;
    EnableRockerL = 0;
  }else if(ps2x.ButtonReleased(PSB_PAD_DOWN)) {   //Down released
    Serial.println(" PAD_DOWN just released");
    MOTORSTOP;
    EnableRockerL = 1;
    LStop = 0;
  }

/***********************************************************************
   Rocker program
   Through the rocker arm around before and after the change of
   resistance for motor speed control
   Up and Left（0 - 127）
   Down and Right （129 - 255）
   摇杆比例控制程序
      通过摇杆前后左右阻值的变化实现电机转速控制
      上左范围（0 - 127） 
      下右范围（129 - 255）
***********************************************************************/
  if (EnableRockerL) {
    if (0 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) <= 115) {
      digitalWrite(DIR, LOW);
      analogWrite(PWM, map(ps2x.Analog(PSS_LY), 0,115,255,0));
      LStop = 0;
    } else if (255 >= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) >= 140) {
      digitalWrite(DIR, HIGH);
      analogWrite(PWM, map(ps2x.Analog(PSS_LY), 140,255,0,255));
      LStop = 0;
    } else {
      if(LStop == 0){
        MOTORSTOP;
        LStop ++;
      }
    }
  }


  if (ps2x.ButtonPressed(PSB_CIRCLE)) {             //Right pressed
    Serial.println("Circle just pressed");         //will be TRUE if button was JUST pressed
    TURNRIGHT;
    EnableRockerR = 0;
  } else if (ps2x.ButtonReleased(PSB_CIRCLE)) {     //Right released
    Serial.println("Circle just released");
    TURNMID;
    EnableRockerR = 1;
    RMid = 0;
  }
  
  if (ps2x.ButtonPressed(PSB_SQUARE)) {            //Left pressed
    Serial.println("Square just pressed");        //will be TRUE if button was JUST pressed
    TURNLEFT;
    EnableRockerR = 0;
  } else if (ps2x.ButtonReleased(PSB_SQUARE)) {    //Left released
    Serial.println("Square just released");       //will be TRUE if button was JUST released
    TURNMID;
    EnableRockerR = 1;
    RMid = 0;
  }
  
  if (EnableRockerR) {                              //Left and Right
    if (0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) <= 115) {
      myservo.write(map(ps2x.Analog(PSS_RX), 0, 115, 40,88));
      RMid = 0;
    } else if (255 >= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) >= 140) {
      myservo.write(map(ps2x.Analog(PSS_RX), 140, 255, 92,120));
      RMid = 0;
    }else {
      if(RMid == 0){
        TURNMID;
        RMid ++;
      }
    }
  }

  if (ps2x.NewButtonState(PSB_CROSS)) {             //Stop
    Serial.println("X just changed");              //will be TRUE if button was JUST pressed OR released
    MOTORSTOP;
  }
  delay(50);
}

void setMotor(int Speed){                           //Motor drive function
  if (Speed > 0){
    digitalWrite(DIR, LOW);
    analogWrite(PWM, Speed);
  }else if(Speed < 0){
    digitalWrite(DIR, HIGH);
    analogWrite(PWM, - Speed);
  }else{
    analogWrite(PWM, LOW);
    digitalWrite(DIR, LOW);
  }
}
