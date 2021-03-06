#!/usr/bin/python3
#!clear
import optparse
import sys
import curses
from Copter import Copter

class Colors:
    header = '\033[95m'
    blue = '\033[94m'
    green= '\033[92m'
    warning= '\033[93m'
    fail = '\033[91m'
    end = '\033[0m'
    bold = '\033[1m'

def getProperties():
    properties = {}
    file = open(r"/quad/properties.txt")
    lines = file.readlines()
    for line in lines:
        #TODO: get rid of \n
        l = line.split("=")
        properties[l[0]] = l[1:-2]
    return properties

if __name__ == "__main__":
    p = optparse.OptionParser()
    p.add_option('--propFile', '-p', default=r"/quad/properties.txt")
    options, arguments = p.parse_args()
    
    print("")
    print(Colors.green+ "Welcome to Quadcopter"+Colors.end)
    quad = Copter(options.propFile) 
    quad.properties = getProperties() #Load Properties
    
    while(True):
        inp = raw_input(Colors.blue+"::> "+Colors.end)
        x = inp.split("-")
        if x[0] == "establish":
            quad.establish()
        elif x[0] == "control":
            quad.control(x[1],x[2],x[3],x[4])
        elif x[0] == "close":
            quad.killConnection()
        elif x[0]== "properties":
            if len(x) == 1:
                print("")
                print("PROPERTIES")
                print("=================================")
                for prop in quad.properties:
                    print(prop+" : "+quad.properties[prop])
            if len(x) == 2:
                if x[1] == "save":
                    quad.updateProperties()
                else:
                    print(quad.properties[x[1]])
            if len(x) == 3:
                quad.properties[x[1]] = x[2]+"\n"
        elif x[0] == "arm":
            quad.Arm()
        elif x[0] == "unarm":
            quad.unArm()
        elif x[0] == "isarm":
            print(quad.isArm())
        elif x[0] == "listen":
            while(1):
                bs = '\b' * 30
                sys.stdout.write(quad.angles())        
        elif x[0] == "clear":
            print('\033c')
        elif x[0] == "nav":
            if x[1] == "all":
                if x[2] == "on":
                    quad.send("\x02\x00\x01\x00")
                elif x[2] == "off":
                    quad.send("\x02\x00\x00\x00")
            elif x[1] == "fr":
                if x[2] == "on":
                    quad.send("\x02\x01\x01\x00")
                elif x[2] == "off":
                    quad.send("\x02\x01\x00\x00")
            elif x[1] == "fl":
                if x[2] == "on":
                    quad.send("\x02\x02\x01\x00")
                elif x[2] == "off":
                    quad.send("\x02\x02\x00\x00")
            elif x[1] == "br":
                if x[2] == "on":
                    quad.send("\x02\x03\x01\x00")
                elif x[2] == "off":
                    quad.send("\x02\x03\x00\x00")
            elif x[1] == "bl":
                if x[2] == "on":
                    quad.send("\x02\x04\x01\x00")
                elif x[2] == "off":
                    quad.send("\x02\x04\x00\x00")
        elif x[0] == "propFile":
            print(quad.propFile)
        elif x[0] == "send":
            print("Sending: " +x[1])
            quad.send(x[1])
        elif x[0] == "gyro":
            if x[1] == "start":
                quad.startGyro()
            if x[1] == "stop":
                quad.stopGyro()
        elif x[0] == "serialmonitor":
            quad.serialMonitor()
        elif x[0] == "test":
            quad.test()
        elif x[0] == "help":
            t = """
establish:   establish connection with ardupiolet
properties:  return the properties
    properties-&key: returns value at key
    properties-&key-&value: sets the value equal to the key
    properties-save: saves the properties for future use
arm:         arm the quadcopter
unarm:       unarm the quadcopter
isarm:       displays if the quadcopter is armed
test:        Spins each motor to test if they work
nav:
    nav-all-on/off: turns all navigation lights either on or off
    nav-fr/fl/br/bl-on/off: tunrs on a specific navigation light on or off

"""
            print(t)
        elif x[0] == "quit":
            break
        elif x[0] == "":
            pass
        
        else:
            print(Colors.fail+"!!! Not Valid -- use 'help' !!!"+Colors.end)







