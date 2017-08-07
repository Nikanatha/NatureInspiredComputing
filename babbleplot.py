# -*- coding: utf-8 -*-
"""
Created on Sun Aug  6 19:17:44 2017

@author: jakob
"""

import matplotlib.pyplot as plt
import numpy as np
import re
#import map

path = "/home/jakob/studium/master3/NIC/FORK/NatureInspiredComputing/babble.txt"

controller = []
valueSystem = []
controllercollumn = []
valueSystemcollumn = []

"""flags for the next line"""
controllerlineflag = 0
valuesystemlineflag = 0
nodeaddlineflag = 0
datalineflag = 0

controllercheck = "Controller"
valuesystemcheck = "Value"
nodeaddcheck = "Added"
datacheck = "Angle"

#binBoundaries = np.arange(0,1024,60)

def check(tocheckstring):
    global controllerlineflag
    global valuesystemlineflag
    global nodeaddlineflag
    global datalineflag
    
    if controllercheck in tocheckstring:
        controllerlineflag = 1
    elif valuesystemcheck in tocheckstring:
        valuesystemlineflag = 1
    elif nodeaddcheck in tocheckstring:
        nodeaddlineflag = 1 
    elif datacheck in tocheckstring:
        datalineflag = 1
    else:
        print "no codewords in the line"
    return 0;
    
def burnflag():
    global controllerlineflag
    global valuesystemlineflag
    global nodeaddlineflag
    global datalineflag
    controllerlineflag = 0
    valuesystemlineflag = 0
    nodeaddlineflag = 0
    datalineflag = 0

with open(path, "r") as fileread:
    #read line by line
    for line in fileread:
        #check for added controller Value set the flags
        check(line)
        if (controllerlineflag == 1) and (datalineflag ==1):
            #splitalongside ==
            splitline = line.split("==")
            #extract all positive integers rom string
            append = []
            append.append( [int(s) for s in splitline[0].split() if s.isdigit()] )
            append.append( re.findall(r"[-+]?\d*\.\d+|\d+", splitline[1]))
            controller.append(np.hstack(append))
            burnflag()
        elif (nodeaddlineflag == 1):
            burnflag()
        elif (valuesystemlineflag == 1) and (datalineflag ==1):
            splitline = line.split("==")
            append = []
            append.append( [int(s) for s in splitline[0].split() if s.isdigit()] )
            append.append( re.findall(r"[-+]?\d*\.\d+|\d+", splitline[1]))
            valueSystem.append(np.hstack(append))
            burnflag()



for iter in range (0,8):
        controllercollumn.append ([row[iter] for row in controller])
for iter in range (0,8):
        valueSystemcollumn.append ([row[iter] for row in valueSystem])


controllerarray = np.asarray(controllercollumn[7])
valueSystemArray = np.asarray(valueSystemcollumn[7])
controllerspeed = controllerarray.astype(np.float)
valueSystemspeed = valueSystemArray.astype(np.float)

controlleranglearray = np.asarray(controllercollumn[6])
valueSystemanglearray = np.asarray(valueSystemcollumn[6])
controllerangle = controlleranglearray.astype(np.float)
valueSystemangle = valueSystemanglearray.astype(np.float)

speeddelta =map(float.__sub__, controllerspeed , valueSystemspeed)
angledelte =map(float.__sub__, controllerangle , valueSystemangle)



fig, axs = plt.subplots(2, 2)
axs[0, 0].plot(np.arange(0,len(speeddelta),1),speeddelta)
axs[0, 0].set_title("controllerspeed-valuesystemspeed")
axs[0, 1].plot(np.arange(0,len(valueSystemspeed),1), valueSystemspeed)
axs[0, 1].set_title("valueSystemSpeed")
axs[1, 0].plot(np.arange(0,len(controllerspeed),1), controllerspeed)
axs[1, 0].set_title("ControllerSpeed")
axs[1, 1].axis('off')
axs[1, 1].text(-0.1, 0.7, r'Deltameanspeed: %s '%(np.mean(speeddelta)), fontsize=15)
axs[1, 1].text(-0.1, 0.5, r'ValueSystemmeanspeed: %s '%(np.mean(valueSystemspeed)), fontsize=15)
axs[1, 1].text(-0.1, 0.3, r'Controllermeanspeed: %s '%(np.mean(controllerspeed)), fontsize=15)

fig2, axs = plt.subplots(2, 2)
axs[0, 0].plot(np.arange(0,len(angledelte),1),angledelte)
axs[0, 0].set_title("controllerangle-Valuesystemangle")
axs[0, 1].plot(np.arange(0,len(valueSystemangle),1), valueSystemangle)
axs[0, 1].set_title("valueSystemangle")
axs[1, 0].plot(np.arange(0,len(controllerangle),1), controllerangle)
axs[1, 0].set_title("controllerangle")
axs[1, 1].axis('off')


#axs[1, 1].imshow(np.random.random((100, 100)))