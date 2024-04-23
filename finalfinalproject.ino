#include <Stepper.h>

#include <Wire.h>
#include <I2C_RTC.h>

#define motorSteps 64
#define motorPin1  8 
#define motorPin2  9 
#define motorPin3  10
#define motorPin4  11

Stepper stepper(motorSteps, motorPin1, motorPin3, motorPin2, motorPin4); 

unsigned long remoteInput;

#include <IRremote.h>

IRrecv IR(3);

static DS1307 RTC;
int hour;


static int state = 0;
static int openCount = 0;

static boolean motorRunning = false;

void setup() {
  RTC.begin();
  RTC.setHourMode(CLOCK_H24);

  //RTC.setWeek(2);

  //RTC.setDate(14,04,24);
  RTC.setTime(16,58,00);

  Serial.begin(9600);
  IR.enableIRIn();
  stepper.setSpeed(500); 
}

void loop() {
  hour = RTC.getHours();
  int seconds = RTC.getSeconds();
  receiveIRSignal();

}

void receiveIRSignal(){
  if (hour == 8){
    motorRunning = true;
      state = 1;
      openCount +=1;
      if (motorRunning == true){
        stepper.step(6000);
        motorRunning = false;
      }
  }
  else if (hour == 20){
      motorRunning = true;
      state = 3;
      openCount -= 1;
      if (motorRunning == true){
        stepper.step(-6000);
        motorRunning = false;
      }
  }
  if (IR.decode()){
    remoteInput = IR.decodedIRData.decodedRawData;
    Serial.println(remoteInput, HEX);
    delay(300);
    IR.resume();
    if (openCount == 0){
      state = 0;
    }
    if (remoteInput == 0xF30CFF00 && (state != 4 && state != 2 && openCount < 2)){
      motorRunning = true;
      state = 1;
      openCount +=1;
      if (motorRunning == true){
        stepper.step(6000);
        motorRunning = false;
      }
    }
    else if (remoteInput == 0xE718FF00 && (state == 0) && (openCount < 1)){
      motorRunning = true;
      state = 2;
      openCount +=1;
      if (motorRunning == true){
        stepper.step(14000);
        motorRunning = false;
      }
    }
    else if (remoteInput == 0xA15EFF00 && (state != 0 && state != 4)){
      motorRunning = true;
      state = 3;
      Serial.println(state);
      openCount -= 1;
      if (motorRunning == true){
        stepper.step(-6000);
        motorRunning = false;
      }
    }
    else if (remoteInput == 0xF708FF00 && openCount == 0){
      motorRunning = true;
      state = 4;
      openCount = 0;
      if (motorRunning == true){
        stepper.step(-14000);
        motorRunning = false;
      }
    }
    else {
      motorRunning = false;
    }
  }
}
