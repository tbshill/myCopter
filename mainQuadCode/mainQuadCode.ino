
#include <SPI.h> //Includes
#include <math.h>

#define ToD(x) (x/131)
#define ToG(x) (x*9.80665/16384)

#define xAxis 0
#define yAxis 1
#define zAxis 2


#define Aoffest 0.8
#define ChipSelPin1 53 //SPI select pin for the MPU6000
#define MPU6000 53

//arduino pins for the LED lights on the Ardupilot
#define lRED 27 
#define lYELLOW 26
#define lBLUE 25

//Arduino Pins for the motor ports on the Ardupilot
#define M1 12
#define M2 11
#define M3 8
#define M4 7
#define M5 6
#define M6 3
#define M7 2
#define M8 5

//Arduino Pins for the Analog Ports on the ardupilot
#define A0 54
#define A1 55
#define A2 56
#define A3 57
#define A4 58
#define A5 59
#define A6 60
#define A7 61
#define A8 621 //This isnt right

//Defines which motor is pluged into which Motor port (Not Arduino Pin)
#define mFR M4 //M8
#define mFL M6 //M6
#define mBR M8 //M5
#define mBL M5 //M4

//Defines which Navigation Led is pluged into which Analog Port(Not Arduino Pin)
#define lFR A7 //RED WIRE
#define lFL A6 //BROWN
#define lBL A4 //YELLOW
#define lBR A5 //WHITE

//Defines the Baudrate
#define BAUDRATE 9600

//COMMUNICATION 
//This is a list of constants for the serial communication between the Raspberry Pi and the Ardupilot.
//All of these are being created as needed in the code. 
#define ARM 0x00
#define UNARM 0x01
#define NAV 0x02
#define ALL 0x00
#define LFR 0x01
#define LFL 0x02
#define LBR 0x03
#define LBL 0x04
#define TEST 0x04
#define ON 0x01
#define OFF 0x00

boolean established = false; //global variable that shows if the quadcopter has established a connection with the raspberry pi. 
//Motors can't run  if this is false

boolean arm = false; //gloabl variable that shows if the quadcopter has been armed. Motors can't run if this is false

//PID gains. I dont know what the actual cosntants are going to be.
float Pgain = 1;
float Igain = 0;
float Dgain = 0;

void setup(){ //Setup
  //Set the onboard LED's to output
  pinMode(lRED, OUTPUT);
  pinMode(lYELLOW, OUTPUT);
  pinMode(lBLUE, OUTPUT);
  //The are active low, so this code turns the LEDs off
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
  ConfigureMPU6000();
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
        int Power = b2; //I explain these variables here: http://myquadcopters.blogspot.com/2014/06/mapping-thrust-x-y-tilts-and-turning.html
        int XStrafe = b3;
        int YStrafe = b4;
        int Turn = b5;
        
        int fr = Power + YStrafe + XStrafe + Turn;
        int fl = Power + YStrafe + XStrafe + Turn;
        int br = Power + YStrafe + XStrafe + Turn;
        int bl = Power + YStrafe + XStrafe + Turn;

        //TODO: Stability/PID controller
        
        analogWrite(mFR,fr); //Writes motor commands from user input
        analogWrite(mFL,fl);
        analogWrite(mBR,br);
        analogWrite(mBL,bl);
      }//END OF CONTROL
    }//END OF SERIAL

    int accel_x = AccelX(MPU6000);
    int accel_y = AccelY(MPU6000);
    int accel_z = AccelZ(MPU6000);
    int gyro_x  = GyroX(MPU6000);
    int gyro_y  = GyroY(MPU6000);
    int gyro_z  = GyroZ(MPU6000);

    int desired_x = 0; //Angles 
    int desired_y = 0; //Angles
    int desired_z = 0; //Angles

    int actual_x = tan(accel_z/accel_x);
    int actual_y = tan(accel_z/accel_y);

    int error_x = actual_x - desired_x;
    int error_y = actual_y - desired_y;

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

//The below code is not my own. It is the communication with the MPU6000(Accelerometer and Gyroscope) and I took it from then internet. I have a blog post about where I found it. 
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
void ConfigureMPU6000(){
  SPIwrite(0x6B,0x80,ChipSelPin1);
  delay(150);
  SPIwrite(0x6B,0x03,ChipSelPin1);
  delay(150);
  SPIwrite(0x6A,0x10,ChipSelPin1);
  delay(150);
  SPIwrite(0x19,0x00,ChipSelPin1);
  delay(150);
  SPIwrite(0x1A,0x03,ChipSelPin1);
  delay(150);
  SPIwrite(0x1B,0x00,ChipSelPin1);
  delay(150);
  SPIwrite(0x1C,0x00,ChipSelPin1);
  delay(150);
}