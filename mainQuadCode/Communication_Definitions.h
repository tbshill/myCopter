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
		
		#define ON 0x01
		#define OFF 0x00

#define CONTROL 0x03
#define TEST 0x04
