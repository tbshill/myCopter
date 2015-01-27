
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