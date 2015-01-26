import serial
import time
import RPi.GPIO as gpio
class Copter:
    def __init__(self,propFile = ""):
        self.properties = {}
        self.arm = False
        self.propFile = propFile
        self.pilot = serial.Serial('/dev/ttyS0',9600) #Try and automate the Com port
    def Arm(self):
        self.arm = True
        self.pilot.write("\x00")
    def unArm(self):
        self.arm = False
        self.pilot.write("\x01")
    def isArm(self):
        return self.arm
    def establish(self):
        self.pilot.write("\xFF\xFF\xFF\xFF")
    def killConnection(self):
        self.pilot.close()
    def send(self,data):
        self.pilot.write(data)
        print("sent")
    def startGyro(self): #Not used
        print(0x01)
    def stopGyro(self): #Not used
        print(0x02)
    def control(self,p,y,x,t):
        data = bytearray()
        print(int(p)+int(x))
        data.append(0x03)
        data.append(int(p))
        data.append(int(y))
        data.append(int(x))
        data.append(int(t))
        
        self.pilot.write(data)
    def test(self):
        if self.arm == False:
            print("Testing motors. Please see if the motors turn on in the fallowing order: FR, FL BR, BL")
            self.pilot.write("\x04\x00\x00\x00")
        else:
            print("Please unarm the quadcopter and try again")
    def updateProperties(self):
        file = open(self.propFile,'wr')
        data = ""
        for prop in self.properties:
            data = data+ prop + "="+self.properties[prop]
        print(data)
        file.write(data)
    def getProperties(self,propF):
        properties = {}
        file = open(r"/quad/properties.txt")
        lines = file.readlines()
        for line in lines:
            #TODO: get rid of \n
            l = line.split("=")
            properties[l[0]] = l[1]
        self.properties = properties
    def serialMonitor(self):
        while(True):
            inp = raw_input("SM> ")
            if inp == "quit":
                break
            print(inp)
