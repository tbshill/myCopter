import serial
import time
import RPi.GPIO as gpio
class Copter:
    def __init__(self,propFile = ""):
        self.properties = {}
        self.arm = False
        self.propFile = propFile
        self.pilot = serial.Serial('/dev/ttyACM0',9600)
    def Arm(self):
        self.arm = True
    def unArm(self):
        self.arm = False
    def isArm(self):
        return self.arm
    def establish(self):
        while True:
            print(self.pilot.read())
    def send(self,data):
        self.pilot.write("\xFF")
        print("wrote 0xFF")
        print(self.pilot.read().encode('hex'))
    def startGyro(self):
        print(0x01)
    def stopGyro(self):
        print(0x02)
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
