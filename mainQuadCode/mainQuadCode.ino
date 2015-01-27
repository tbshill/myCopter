#include <SPI.h> //Includes
#include <math.h>
#include "Definitions.h"
#include "Communication_Definitions.h"

boolean established = false; //global variable that shows if the quadcopter has established a connection with the raspberry pi. 
//Motors can't run  if this is false

boolean arm = false; //gloabl variable that shows if the quadcopter has been armed. Motors can't run if this is false

//PID gains. I dont know what the actual cosntants are going to be.
float Pgain = 1;
float Igain = 0;
float Dgain = 0;

int fr = 0;
int fl = 0;
int br = 0;
int bl = 0;

int accel_x = 0;
int accel_y = 0;
int accel_z = 0;
int gyro_x = 0;
int gyro_y = 0;
int gyro_z = 0;

int Power = 0;
int XStrafe = 0;
int YStrafe = 0;
int Turn = 0;

void setup(){ //Setup
  //Set the onboard LED's to output
  pinMode(lRED, OUTPUT);
  pinMode(lYELLOW, OUTPUT);
  pinMode(lBLUE, OUTPUT);
  //They are active low, so this code turns the LEDs off
  digitalWrite(lRED, HIGH);
  digitalWrite(lBLUE,HIGH);
  digitalWrite(lYELLOW,HIGH);
  
  //MOTOR OUTPUT
  pinMode(M1,OUTPUT);
  pinMode(M2,OUTPUT);
  pinMode(M3,OUTPUT);
  pinMode(M4,OUTPUT);
  pinMode(M5,OUTPUT);
  pinMode(M6,OUTPUT);
  pinMode(M7,OUTPUT);
  pinMode(M8,OUTPUT);
  
  //SPI wrong select pin
  pinMode(40,OUTPUT);
  digitalWrite(40,HIGH);
  
  Serial.begin(BAUDRATE); //Initiate Serial communications
  
  //Kills the twitching motors. The motors don't start turning untill 147
  analogWrite(mFR,100);
  analogWrite(mFL,100);
  analogWrite(mBR,100);
  analogWrite(mBL,100);
  
  //Configure the SPI communication with the MPU6000
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  delay(100);
  pinMode(ChipSelPin1,OUTPUT);
  ConfigureMPU6000(MPU6000);
}

void loop(){

  if(arm == false){ //If Quad is not armed
    analogWrite(mFR, 100); //keep all motors off
    analogWrite(mFL, 100);
    analogWrite(mBR, 100);
    analogWrite(mBL, 100);
    digitalWrite(lYELLOW,HIGH); //Yellow Light is Off
  }

  if(established == false){
    if(Serial.available() > 0){
      delay(50); //allows time for all incoming bits to be read
      int b1 = Serial.read();
      int b2 = Serial.read();
      int b3 = Serial.read();
      int b4 = Serial.read();
      arm = false;
      if( b1 == 255 && 
          b2 == 255 && 
          b3 == 255 && 
          b4 == 255){
        established = true;
        digitalWrite(lBLUE, LOW);
      }
    }
  }
  else{ //IF ESTABLISHED
    if(Serial.available() > 0){
      delay(100); // TODO: I want to see how small I can get the number
      int b1 = Serial.read(); // Read all the incomming bytes. 
      int b2 = Serial.read();
      int b3 = Serial.read();
      int b4 = Serial.read();
      int b5 = Serial.read();
      
      if(b1 == ARM){
        arm = true; //Arms the quadcopter.
        digitalWrite(lYELLOW, LOW); //Turns Yellow LED on
      }//END OF ARM
      else if(b1 == UNARM){
        arm = false; //Un arms the quadcopter. 
      }//END OF UNARM
      else if(b1 == TEST && arm == false){ //TEST MOTORS motors spin one at a time int the fallowing order: FR, FL, BR, BL. The motorts spin slow enough to safely stop them with a finger.
        //DISCLAMER: JUST BECAUSE I WAS ABLE TO DO IT WITH MY QUAD COPTER DOESN'T MEAN YOU SHOULD STICK YOUR FINGER IN SPINNING BLADES. YOUR BLADES MAY BE GOING FASTER THAN MINE.
        //DONT STICK YOUR FINGERS IN BETWEEN PROPELLERS WHILE THEY ARE SPINING
        //Starts by flashing the LEDs to alert the user that the motors are about to spin. 
        digitalWrite(lYELLOW, HIGH);
        digitalWrite(lRED, HIGH);
        digitalWrite(lBLUE,HIGH);
        delay(500);
        flashLED(250);
        flashLED(250);
        flashLED(250);
        
        //each motor spins by its self for one second. 
        analogWrite(mFR,148);
        delay(1000);
        analogWrite(mFR,100);
        analogWrite(mFL,148);
        delay(1000);
        analogWrite(mFL,100);
        analogWrite(mBR,148);
        delay(1000);
        analogWrite(mBR,100);
        analogWrite(mBL,148);
        delay(1000);
        analogWrite(mBL,100);
        
        //Flashes the LEDs to tell the user that the test is over
        flashLED(500);  
      }//END OF TEST
      else if(b1 == NAV){ //If the user wants to turn on the navigation lights
        //Which light does the user want to turn on or off
        if(b2 == ALL){ //All lights
          if(b3 == ON){
            analogWrite(lFR,255);
            analogWrite(lFL,255);
            analogWrite(lBL,255);
            analogWrite(lBR,255);
          }
          else if(b3 == OFF){
            analogWrite(lFR,0);
            analogWrite(lFL,0);
            analogWrite(lBL,0);
            analogWrite(lBR,0);
          }
        }
        else if(b2 == LFR){ //Front Right
          if(b3 == ON){analogWrite(lFR,255);} //use an anolg write because a digital write turn on ligts at a very dimm setting. 
          else if(b3 == OFF){analogWrite(lFR,0);}
        }
        else if(b2 == LFL){ //Front Left
          if(b3 == ON){analogWrite(lFL,255);}
          else if(b3 == OFF){analogWrite(lFL,0);}
        }
        else if(b2 == LBL){ //Back Left
          if(b3 == ON){analogWrite(lBL,255);}
          else if(b3 == OFF){analogWrite(lBL,0);}
        }
        else if(b2 == LBR){ //Back Right
          if(b3 == ON){analogWrite(lBR,255);}
          else if(b3 == OFF){analogWrite(lBR,0);}
        }
      }//END OF NAV
      else if(b1 == CONTROL){ // Going to be the most used
        Power = b2; //I explain these variables here: http://myquadcopters.blogspot.com/2014/06/mapping-thrust-x-y-tilts-and-turning.html
        XStrafe = b3;
        YStrafe = b4;
        Turn = b5;
      }//END OF CONTROL
    }//END OF SERIAL

    accel_x = AccelX(MPU6000); //gets the Accel_x value
    accel_y = AccelY(MPU6000); //gets the Accel_y value
    accel_z = AccelZ(MPU6000); //gets the Accel_z value
    gyro_x  = GyroX(MPU6000); // gets the Gyro_x value
    gyro_y  = GyroY(MPU6000); // gets the Gyro_y value
    gyro_z  = GyroZ(MPU6000); // gets the Gyro_z value

    int desired_x = 0; //Angles 
    int desired_y = 0; //Angles
    int desired_z = 0; //Angles

    int actual_x = tan(accel_z/accel_x); //Angles
    int actual_y = tan(accel_z/accel_y); //Angles

    int error_x = actual_x - desired_x; //Difference in angles
    int error_y = actual_y - desired_y; //Difference in angles
    
    fr = Power - YStrafe - XStrafe + Turn; // | 1 -1 -1  1 |
    fl = Power - YStrafe + XStrafe - Turn; // | 1 -1  1  1 |
    br = Power + YStrafe - XStrafe - Turn; // | 1 1  -1 -1 |
    bl = Power + YStrafe + XStrafe + Turn; // | 1 1   1  1 |

    //TODO: Stability/PID controller
    
    analogWrite(mFR,fr); //Writes motor commands from user input
    analogWrite(mFL,fl);
    analogWrite(mBR,br);
    analogWrite(mBL,bl);

    //Serial.print(actual_x);
    //Serial.print("\t");
    //Serial.println(actual_y);

  }//END OF 'ELSE' ESTABLISHED
}
//Function to flashe the onboard LEDs. Used in the motor test.
void flashLED(int time){
  digitalWrite(lYELLOW, LOW);
  digitalWrite(lRED, LOW);
  digitalWrite(lBLUE,LOW);
  delay(time);
  digitalWrite(lYELLOW, HIGH);
  digitalWrite(lRED, HIGH);
  digitalWrite(lBLUE,HIGH);
  delay(time);
}
void SPIwrite(byte reg, byte data, int ChipSelPin){
  uint8_t dump;
  digitalWrite(ChipSelPin, LOW);
  dump = SPI.transfer(reg);
  dump = SPI.transfer(data);
  digitalWrite(ChipSelPin,HIGH);
}
uint8_t SPIread(byte reg, int ChipSelPin){
  uint8_t dump;
  uint8_t return_value;
  uint8_t addr=reg|0x80;
  digitalWrite(ChipSelPin,LOW);
  dump = SPI.transfer(addr);
  return_value = SPI.transfer(0x00);
  digitalWrite(ChipSelPin,HIGH);
  return(return_value);
}
int AccelX(int ChipSelPin){
  uint8_t h = SPIread(0x3B,ChipSelPin);
  uint8_t l = SPIread(0x3C,ChipSelPin);
  uint16_t Acclx = h<<8 |l;
 return(Acclx); 
}
int AccelY(int ChipSelPin){
  uint8_t h = SPIread(0x3D,ChipSelPin);
  uint8_t l = SPIread(0x3E,ChipSelPin);
  uint16_t Accely = h<<8 |l;
 return(Accely); 
}
int AccelZ(int ChipSelPin){
  uint8_t h = SPIread(0x3F,ChipSelPin);
  uint8_t l = SPIread(0x40,ChipSelPin);
  uint16_t Accelz = h<<8 |l;
 return(Accelz); 
}
int GyroX(int ChipSelPin){
  uint8_t h = SPIread(0x43,ChipSelPin);
  uint8_t l = SPIread(0x44,ChipSelPin);
  uint16_t Gyrox = h<<8 |l;
 return(Gyrox); 
}
int GyroY(int ChipSelPin){
  uint8_t h = SPIread(0x45,ChipSelPin);
  uint8_t l = SPIread(0x46,ChipSelPin);
  uint16_t Gyroy = h<<8 |l;
 return(Gyroy); 
}
int GyroZ(int ChipSelPin){
  uint8_t h = SPIread(0x47,ChipSelPin);
  uint8_t l = SPIread(0x48,ChipSelPin);
  uint16_t Gyroz = h<<8 |l;
 return(Gyroz); 
}
void ConfigureMPU6000(int ChipSelPin){
  SPIwrite(0x6B,0x80,ChipSelPin);
  delay(150);
  SPIwrite(0x6B,0x03,ChipSelPin);
  delay(150);
  SPIwrite(0x6A,0x10,ChipSelPin);
  delay(150);
  SPIwrite(0x19,0x00,ChipSelPin);
  delay(150);
  SPIwrite(0x1A,0x03,ChipSelPin);
  delay(150);
  SPIwrite(0x1B,0x00,ChipSelPin);
  delay(150);
  SPIwrite(0x1C,0x00,ChipSelPin);
  delay(150);
}
