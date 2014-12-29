#include <SPI.h>
#include <math.h>

#define ToD(x) (x/131)
#define ToG(x) (x*9.80665/16384)

#define xAxis 0
#define yAxis 1
#define zAxis 2

#define Aoffest 0.8
#define ChipSelPin1 53

#define lRED 27
#define lYELLOW 26
#define lBLUE 25

#define M1 12
#define M2 11
#define M3 8
#define M4 7
#define M5 6
#define M6 3
#define M7 2
#define M8 5

#define A0 54
#define A1 55
#define A2 56
#define A3 57
#define A4 58
#define A5 59
#define A6 60
#define A7 61
#define A8 621

#define mFR M4 //M8
#define mFL M6 //M6
#define mBR M8 //M5
#define mBL M5 //M4

#define lFR A7 //RED WIRE
#define lFL A6 //BROWN
#define lBL A4 //YELLOW
#define lBR A5 //WHITE

#define BAUDRATE 9600

//COMMUNICATION 
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

boolean established = false;

boolean arm = false;
float Pgain = 1;
float Igain = 0;
float Dgain = 0;

int p = 0;
int yi= 0;
int xi= 0;
int t = 0;
int mfr = 0;
int mfl = 0;
int mbl = 0;
int mbr = 0;


void setup(){
  pinMode(lRED, OUTPUT);
  pinMode(lYELLOW, OUTPUT);
  pinMode(lBLUE, OUTPUT);
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
  
  //
  pinMode(40,OUTPUT);
  digitalWrite(40,HIGH);
  
  Serial.begin(BAUDRATE);
  
  analogWrite(mFR,100);
  analogWrite(mFL,100);
  analogWrite(mBR,100);
  analogWrite(mBL,100);
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  delay(100);
  pinMode(ChipSelPin1,OUTPUT);
  ConfigureMPU6000();

    
}

void loop(){
  if(arm == false){ //IF Quad is not armed
    analogWrite(mFR, 100); //keep all motors off
    analogWrite(mFL, 100);
    analogWrite(mBR, 100);
    analogWrite(mBL, 100);
    digitalWrite(lYELLOW,HIGH);
  }
  if(established == false){
    if(Serial.available() > 0){
      delay(50);
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
      delay(100);
      int b1 = Serial.read();
      int b2 = Serial.read();
      int b3 = Serial.read();
      int b4 = Serial.read();
      int b5 = Serial.read();
      
      if(b1 == ARM){
        arm = true;
        digitalWrite(lYELLOW, LOW);
      }
      else if(b1 == UNARM){
        arm = false;
      }
      else if(b1 == TEST && arm == false){ //TEST MOTORS
        digitalWrite(lYELLOW, HIGH);
        digitalWrite(lRED, HIGH);
        digitalWrite(lBLUE,HIGH);
        delay(500);
        flashLED(250);
        flashLED(250);
        flashLED(250);
        
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
        
        flashLED(500);
        
      }
      else if(b1 == NAV){
        if(b2 == ALL){
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
        else if(b2 == LFR){
          if(b3 == ON){
            analogWrite(lFR,255);
          }
          else if(b3 == OFF){
            analogWrite(lFR,0);
          }
        }
        else if(b2 == LFL){
          if(b3 == ON){
            analogWrite(lFL,255);
          }
          else if(b3 == OFF){
            analogWrite(lFL,0);
          }
        }
        else if(b2 == LBL){
          if(b3 == ON){
            analogWrite(lBL,255);
          }
          else if(b3 == OFF){
            analogWrite(lBL,0);
          }
        }
        else if(b2 == LBR){
          if(b3 == ON){
            analogWrite(lBR,255);
          }
          else if(b3 == OFF){
            analogWrite(lBR,0);
          }
        }
      }//END OF NAV
      else if(b1 == CONTROL){
        Power = b2;
        XStrafe = b3;
        YStrafe = b4;
        Turn = b5;
        
        int fr = Power + YStrafe + XStrafe + Turn;
        int fl = Power + YStrafe + XStrafe + Turn;
        int br = Power + YStrafe + XStrafe + Turn;
        int bl = Power + YStrafe + XStrafe + Turn;
        
        analogWrite(mFR,fr);
        analogWrite(mFL,fl);
        analogWrite(mBR,br);
        analogWrite(mBL,bl);
      }
    }//END OF SERIAL
    
  }//END OF ESTABLISHED
}

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


